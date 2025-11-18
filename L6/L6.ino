#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = LED_BUILTIN;
static SemaphoreHandle_t bin_sem;

//*****************************************************************************
// Task: Blink LED using received delay
void blinkLED(void *parameters) {

  int num = *(int *)parameters;

  // Release semaphore to allow setup() to continue
  xSemaphoreGive(bin_sem);

  Serial.print("Received: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);

  while (1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(num / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}

//*****************************************************************************
// setup()
void setup() {

  long int delay_arg;

  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println();
  Serial.println("---FreeRTOS Binary Semaphore Demo---");
  Serial.println("Enter a number for delay (milliseconds)");

  // Wokwi needs small delays for Serial.available() to work
  while (Serial.available() <= 0) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  delay_arg = Serial.parseInt();
  Serial.print("Sending: ");
  Serial.println(delay_arg);

  bin_sem = xSemaphoreCreateBinary();

  xTaskCreatePinnedToCore(
      blinkLED,
      "Blink LED",
      2048,
      (void *)&delay_arg,
      1,
      NULL,
      app_cpu
  );

  xSemaphoreTake(bin_sem, portMAX_DELAY);

  Serial.println("Done!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
