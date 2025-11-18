#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

TickType_t cs_wait = 250;
TickType_t med_wait = 5000;

// Use a mutex to enable priority inheritance
SemaphoreHandle_t lock;

// Task L (low priority)
void doTaskL(void *parameters) {
  TickType_t timestamp;
  while (1) {
    Serial.println("Task L trying to take lock...");
    timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    xSemaphoreTake(lock, portMAX_DELAY);

    Serial.print("Task L got lock. Spent ");
    Serial.print((xTaskGetTickCount() * portTICK_PERIOD_MS) - timestamp);
    Serial.println(" ms waiting for lock. Doing some work...");

    timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS) - timestamp < cs_wait);

    Serial.println("Task L releasing lock.");
    xSemaphoreGive(lock);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Task M (medium priority)
void doTaskM(void *parameters) {
  TickType_t timestamp;
  while (1) {
    Serial.println("Task M doing some work...");
    timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS) - timestamp < med_wait);
    Serial.println("Task M done!");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Task H (high priority)
void doTaskH(void *parameters) {
  TickType_t timestamp;
  while (1) {
    Serial.println("Task H trying to take lock...");
    timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    xSemaphoreTake(lock, portMAX_DELAY);

    Serial.print("Task H got lock. Spent ");
    Serial.print((xTaskGetTickCount() * portTICK_PERIOD_MS) - timestamp);
    Serial.println(" ms waiting for lock. Doing some work...");

    timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
    while ((xTaskGetTickCount() * portTICK_PERIOD_MS) - timestamp < cs_wait);

    Serial.println("Task H releasing lock.");
    xSemaphoreGive(lock);

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("---FreeRTOS Priority Inversion Demo---");

  // Create a mutex to resolve priority inversion
  lock = xSemaphoreCreateMutex();
  if (lock == NULL) {
    Serial.println("Failed to create mutex!");
    ESP.restart();
  }

  // Start tasks
  xTaskCreatePinnedToCore(doTaskL, "Task L", 2048, NULL, 1, NULL, app_cpu);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(doTaskH, "Task H", 2048, NULL, 3, NULL, app_cpu);
  xTaskCreatePinnedToCore(doTaskM, "Task M", 2048, NULL, 2, NULL, app_cpu);

  vTaskDelete(NULL);
}

void loop() {
  // Never reaches here
}
