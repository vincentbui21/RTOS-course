#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Globals
SemaphoreHandle_t mutex_1;
SemaphoreHandle_t mutex_2;

//*****************************************************************************
// Tasks

// Task A (high priority)
void doTaskA(void *parameters) {
  while (1) {
    // Take mutexes in order
    xSemaphoreTake(mutex_1, portMAX_DELAY);
    Serial.println("Task A took mutex 1");
    vTaskDelay(1 / portTICK_PERIOD_MS);

    xSemaphoreTake(mutex_2, portMAX_DELAY);
    Serial.println("Task A took mutex 2");

    // Critical section
    Serial.println("Task A doing some work");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Release mutexes
    xSemaphoreGive(mutex_2);
    xSemaphoreGive(mutex_1);

    Serial.println("Task A going to sleep");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Task B (low priority)
void doTaskB(void *parameters) {
  while (1) {
    // Take mutexes in same order as Task A
    xSemaphoreTake(mutex_1, portMAX_DELAY);
    Serial.println("Task B took mutex 1");
    vTaskDelay(1 / portTICK_PERIOD_MS);

    xSemaphoreTake(mutex_2, portMAX_DELAY);
    Serial.println("Task B took mutex 2");

    // Critical section
    Serial.println("Task B doing some work");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Release mutexes
    xSemaphoreGive(mutex_2);
    xSemaphoreGive(mutex_1);

    Serial.println("Task B going to sleep");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// Main

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("---FreeRTOS Deadlock Demo (Fixed with Mutex Hierarchy)---");

  // Create mutexes
  mutex_1 = xSemaphoreCreateMutex();
  mutex_2 = xSemaphoreCreateMutex();

  // Start tasks
  xTaskCreatePinnedToCore(doTaskA, "Task A", 1024, NULL, 2, NULL, app_cpu);
  xTaskCreatePinnedToCore(doTaskB, "Task B", 1024, NULL, 1, NULL, app_cpu);

  vTaskDelete(NULL);
}

void loop() {
  // Execution never reaches here
}
