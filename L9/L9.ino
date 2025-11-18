#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;
static const TickType_t time_hog = 200;  // CPU hog time

static void hog_delay(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i++) {
    for (uint32_t j = 0; j < 40000; j++) {
      asm("nop");
    }
  }
}

// Task L
void doTaskL(void *parameters) {
  char str[30];
  while (1) {
    sprintf(str, "Task L, Core %i\r\n", xPortGetCoreID());
    Serial.print(str);
    hog_delay(time_hog);
  }
}

// Task H
void doTaskH(void *parameters) {
  char str[30];
  while (1) {
    sprintf(str, "Task H, Core %i\r\n", xPortGetCoreID());
    Serial.print(str);
    hog_delay(time_hog);
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println("\n---FreeRTOS MultiCore Demo---");

  // ==== Option 1: Pin both tasks to Core 0 ====
  // xTaskCreatePinnedToCore(doTaskL, "Task L", 2048, NULL, 1, NULL, pro_cpu);
  // xTaskCreatePinnedToCore(doTaskH, "Task H", 2048, NULL, 2, NULL, pro_cpu);

  // ==== Option 2: Separate cores ====
  xTaskCreatePinnedToCore(doTaskL, "Task L", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(doTaskH, "Task H", 2048, NULL, 2, NULL, 1);

  // ==== Option 3: No affinity ====
  // xTaskCreatePinnedToCore(doTaskL, "Task L", 2048, NULL, 1, NULL, tskNO_AFFINITY);
  // xTaskCreatePinnedToCore(doTaskH, "Task H", 2048, NULL, 2, NULL, tskNO_AFFINITY);

  vTaskDelete(NULL);
}

void loop() {}
