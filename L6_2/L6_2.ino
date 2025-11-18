#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Use only core 1 for demo
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// ===== Settings =====
static const int fixed_num_tasks = 5;

// Message struct for passing to tasks
typedef struct Message {
  char body[64];   // make room for longer words/sentences
  uint8_t len;
} Message;

// Semaphore
static SemaphoreHandle_t sem_params;

// ===== Task Function =====
void myTask(void *parameters) {
  // parameters points to a heap-allocated Message (allocated by setup)
  Message *pmsg = (Message *)parameters;

  // Copy into local variable ASAP (safe on the task's own stack)
  Message msg = *pmsg;

  // We no longer need the heap copy - free it
  vPortFree(pmsg);

  // Notify creator that parameter has been consumed
  xSemaphoreGive(sem_params);

  // Use the local copy
  Serial.print("Received: ");
  Serial.print(msg.body);
  Serial.print(" | len: ");
  Serial.println(msg.len);

  // Simulate work and then delete task
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);
}

// ===== Utility: Split sentence into words =====
int splitWords(char *sentence, char words[][64], int maxWords) {
  int count = 0;
  char *token = strtok(sentence, " ");
  while (token != NULL && count < maxWords) {
    strncpy(words[count], token, 63);
    words[count][63] = '\0';
    count++;
    token = strtok(NULL, " ");
  }
  return count;
}

// ===== Main Setup =====
void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println("\n=== FreeRTOS Counting Semaphore Demo ===");
  Serial.println("Select mode:");
  Serial.println("1 = Five-task synchronized demo");
  Serial.println("2 = Sentence word-task generator");

  // Wait for input
  while (Serial.available() <= 0) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  int mode = Serial.parseInt();
  Serial.print("Selected mode: ");
  Serial.println(mode);

  // -----------------------------
  // MODE 1: Five task demo
  // -----------------------------
  if (mode == 1) {
    Serial.println("\n--- MODE 1: Creating 5 synchronized tasks ---");

    char text[] = "All your base";
    int n = fixed_num_tasks;

    // Create counting semaphore initialized to 0 with max count = n
    sem_params = xSemaphoreCreateCounting(n, 0);

    for (int i = 0; i < n; i++) {
      // Allocate a Message for each task on heap
      Message *m = (Message *)pvPortMalloc(sizeof(Message));
      if (m == NULL) {
        Serial.println("ERROR: pvPortMalloc failed");
        while (1) vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
      strncpy(m->body, text, sizeof(m->body) - 1);
      m->body[sizeof(m->body) - 1] = '\0';
      m->len = strlen(m->body);

      char name[16];
      sprintf(name, "Task_%d", i);

      // Use a larger stack size (bytes). 4096 is safer for Serial + prints.
      const uint32_t stackSize = 4096;

      BaseType_t res = xTaskCreatePinnedToCore(
        myTask,
        name,
        stackSize,
        (void *)m,
        1,
        NULL,
        app_cpu
      );

      if (res != pdPASS) {
        Serial.print("Task create failed for ");
        Serial.println(name);
        vPortFree(m);
      }
    }

    // Wait for all tasks to read their messages
    for (int i = 0; i < n; i++) {
      xSemaphoreTake(sem_params, portMAX_DELAY);
    }

    Serial.println("All tasks created\n");
    return;
  }

  // -----------------------------
  // MODE 2: Dynamic sentence tasks
  // -----------------------------
  else if (mode == 2) {
    Serial.println("\n--- MODE 2: Enter a sentence ---");

    // Wait for user to type a sentence
    while (Serial.available() <= 0) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    char sentence[512];
    int len = Serial.readBytesUntil('\n', sentence, sizeof(sentence) - 1);
    sentence[len] = '\0';

    Serial.print("Received sentence: ");
    Serial.println(sentence);

    // Split into words
    const int maxWords = 50;
    char words[maxWords][64];
    int n = splitWords(sentence, words, maxWords);

    if (n <= 0) {
      Serial.println("No words found.");
      return;
    }

    Serial.print("Word count: ");
    Serial.println(n);

    // Create counting semaphore
    sem_params = xSemaphoreCreateCounting(n, 0);

    // Create N tasks
    for (int i = 0; i < n; i++) {
      Message *m = (Message *)pvPortMalloc(sizeof(Message));
      if (m == NULL) {
        Serial.println("ERROR: pvPortMalloc failed");
        while (1) vTaskDelay(1000 / portTICK_PERIOD_MS);
      }

      strncpy(m->body, words[i], sizeof(m->body) - 1);
      m->body[sizeof(m->body) - 1] = '\0';
      m->len = strlen(m->body);

      char name[16];
      sprintf(name, "Word_%d", i);

      const uint32_t stackSize = 4096;

      BaseType_t res = xTaskCreatePinnedToCore(
        myTask,
        name,
        stackSize,
        (void *)m,
        1,
        NULL,
        app_cpu
      );

      if (res != pdPASS) {
        Serial.print("Task create failed for ");
        Serial.println(name);
        vPortFree(m);
      }
    }

    // Wait for all tasks to read their messages
    for (int i = 0; i < n; i++) {
      xSemaphoreTake(sem_params, portMAX_DELAY);
    }

    Serial.println("All word-tasks created\n");
    return;
  }

  else {
    Serial.println("\nInvalid mode.");
  }
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
