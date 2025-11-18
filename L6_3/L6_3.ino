#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Use core 1
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

#define ADC_PIN A0
#define ROLLING_SIZE 5

// Semaphore
SemaphoreHandle_t bin_sem;

// ADC value
volatile uint16_t adc_val = 0;

// Rolling buffer
uint16_t rolling[ROLLING_SIZE];
uint8_t rolling_index = 0;
bool rolling_full = false;

// Timer handle
TimerHandle_t timer_handle;

// ISR equivalent: called by FreeRTOS timer
void IRAM_ATTR timerCallback(TimerHandle_t xTimer) {
  adc_val = analogRead(ADC_PIN);
  xSemaphoreGiveFromISR(bin_sem, NULL);
}

// Task to print ADC values
void printTask(void *parameters) {
  int mode = *((int *)parameters);

  while (1) {
    xSemaphoreTake(bin_sem, portMAX_DELAY);

    if (mode == 1) {
      Serial.print("Raw ADC: ");
      Serial.println(adc_val);
    } else if (mode == 2) {
      rolling[rolling_index] = adc_val;
      rolling_index = (rolling_index + 1) % ROLLING_SIZE;
      if (rolling_index == 0) rolling_full = true;

      uint32_t sum = 0;
      uint8_t count = rolling_full ? ROLLING_SIZE : rolling_index;
      for (uint8_t i = 0; i < count; i++) sum += rolling[i];
      uint16_t avg = sum / count;

      Serial.print("Rolling avg (last ");
      Serial.print(count);
      Serial.print("): ");
      Serial.println(avg);
    }
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println("=== FreeRTOS ISR + Timer Demo ===");
  Serial.println("Select mode: 1=Raw, 2=Rolling average");

  while (Serial.available() <= 0) vTaskDelay(10 / portTICK_PERIOD_MS);
  int mode = Serial.parseInt();
  Serial.print("Selected mode: ");
  Serial.println(mode);

  // Create semaphore
  bin_sem = xSemaphoreCreateBinary();
  if (!bin_sem) {
    Serial.println("Failed to create semaphore");
    ESP.restart();
  }

  int *mode_ptr = (int *)malloc(sizeof(int));
  *mode_ptr = mode;

  // Task
  xTaskCreatePinnedToCore(
    printTask,
    "PrintTask",
    4096,
    mode_ptr,
    2,
    NULL,
    app_cpu
  );

  // Create software timer at 1 Hz (1000 ms)
  timer_handle = xTimerCreate(
    "ADCTimer",
    pdMS_TO_TICKS(1000),
    pdTRUE, // auto-reload
    NULL,
    timerCallback
  );

  if (timer_handle != NULL) {
    xTimerStart(timer_handle, 0);
  }
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}