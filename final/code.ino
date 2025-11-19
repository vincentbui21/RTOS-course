/*
  ESP32-S3 RTOS Traffic Light System (ESP-IDF version)
  - 2 red, 2 yellow, 2 green LEDs
  - 2 white pedestrian LEDs
  - 2 buttons (INPUT_PULLUP)
  - 2 buzzers
  - FreeRTOS tasks for concurrency
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <stdbool.h>

// ------------------------------
// PIN MAP
// ------------------------------
#define RED1      2
#define YELLOW1   3
#define GREEN1    4

#define RED2      5
#define YELLOW2   6
#define GREEN2    7

#define PED1      8
#define PED2      9

#define BUZZ1     10
#define BUZZ2     11

#define BTN1      12
#define BTN2      13

// ------------------------------
// RTOS Handles
// ------------------------------
TaskHandle_t trafficTaskHandle;
TaskHandle_t pedestrianTaskHandle;
TaskHandle_t buzzerTaskHandle;
QueueHandle_t pedRequestQueue;

// ------------------------------
// Timing Config (ms)
// ------------------------------
const int GREEN_TIME   = 5000;
const int YELLOW_TIME  = 2000;
const int RED_TIME     = 4000;
const int PED_TIME     = 6000;

// ------------------------------
// HELPER FUNCTIONS
// ------------------------------
void setTrafficLights(bool r1, bool y1, bool g1, bool r2, bool y2, bool g2) {
    gpio_set_level(RED1, r1);
    gpio_set_level(YELLOW1, y1);
    gpio_set_level(GREEN1, g1);

    gpio_set_level(RED2, r2);
    gpio_set_level(YELLOW2, y2);
    gpio_set_level(GREEN2, g2);
}

void setPedestrian(bool p1, bool p2) {
    gpio_set_level(PED1, p1);
    gpio_set_level(PED2, p2);
}

void buzz(bool on) {
    gpio_set_level(BUZZ1, on);
    gpio_set_level(BUZZ2, on);
}

bool buttonPressed(int pin) {
    return gpio_get_level(pin) == 0; // LOW = pressed
}

// ------------------------------
// TASK: Traffic Light Controller
// ------------------------------
void trafficTask(void *p) {
    int pedReq = 0;
    while (true) {
        if (xQueueReceive(pedRequestQueue, &pedReq, 0) == pdTRUE) {
            // Transition to red safely
            setTrafficLights(0,1,0, 0,1,0);
            vTaskDelay(YELLOW_TIME / portTICK_PERIOD_MS);

            setTrafficLights(1,0,0, 1,0,0);
            vTaskDelay(500 / portTICK_PERIOD_MS);

            // Notify pedestrian task
            xTaskNotify(pedestrianTaskHandle, 1, eSetValueWithoutOverwrite);
        }

        // Normal state: GREEN
        setTrafficLights(0,0,1, 0,0,1);
        vTaskDelay(GREEN_TIME / portTICK_PERIOD_MS);

        // Check pedestrian again
        if (xQueueReceive(pedRequestQueue, &pedReq, 0) == pdTRUE) {
            setTrafficLights(0,1,0, 0,1,0);
            vTaskDelay(YELLOW_TIME / portTICK_PERIOD_MS);

            setTrafficLights(1,0,0, 1,0,0);
            vTaskDelay(500 / portTICK_PERIOD_MS);

            xTaskNotify(pedestrianTaskHandle, 1, eSetValueWithoutOverwrite);
            continue;
        }

        // YELLOW
        setTrafficLights(0,1,0, 0,1,0);
        vTaskDelay(YELLOW_TIME / portTICK_PERIOD_MS);

        // RED
        setTrafficLights(1,0,0, 1,0,0);
        vTaskDelay(RED_TIME / portTICK_PERIOD_MS);
    }
}

// ------------------------------
// TASK: Pedestrian Light Handler
// ------------------------------
void pedestrianTask(void *p) {
    uint32_t signal = 0;
    while (true) {
        xTaskNotifyWait(0,0,&signal,portMAX_DELAY);

        // Flash white LEDs
        for (int i=0;i<6;i++) {
            setPedestrian(1,1);
            vTaskDelay(300 / portTICK_PERIOD_MS);
            setPedestrian(0,0);
            vTaskDelay(300 / portTICK_PERIOD_MS);
        }

        // Steady ON pedestrian
        setPedestrian(1,1);

        // Notify buzzer task
        xTaskNotify(buzzerTaskHandle, 1, eSetValueWithoutOverwrite);

        vTaskDelay(PED_TIME / portTICK_PERIOD_MS);

        // End pedestrian phase
        setPedestrian(0,0);
    }
}

// ------------------------------
// TASK: Buzzer Pattern
// ------------------------------
void buzzerTask(void *p) {
    uint32_t sig = 0;
    while (true) {
        xTaskNotifyWait(0,0,&sig,portMAX_DELAY);

        uint32_t start = xTaskGetTickCount();
        while (xTaskGetTickCount() - start < (PED_TIME / portTICK_PERIOD_MS)) {
            buzz(1);
            vTaskDelay(150 / portTICK_PERIOD_MS);
            buzz(0);
            vTaskDelay(150 / portTICK_PERIOD_MS);
        }
    }
}

// ------------------------------
// TASK: Button Handlers
// ------------------------------
void buttonTask(void *p) {
    int pin = (int)p;
    while (true) {
        if (buttonPressed(pin)) {
            int msg = 1;
            xQueueSend(pedRequestQueue, &msg, 0);
            vTaskDelay(500 / portTICK_PERIOD_MS); // debounce
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

// ------------------------------
// SETUP (ESP-IDF app_main)
// ------------------------------
void app_main(void) {
    // ----------------- Pins -----------------
    gpio_config_t io_conf;

    // Outputs
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL<<RED1)|(1ULL<<YELLOW1)|(1ULL<<GREEN1)|
                            (1ULL<<RED2)|(1ULL<<YELLOW2)|(1ULL<<GREEN2)|
                            (1ULL<<PED1)|(1ULL<<PED2)|
                            (1ULL<<BUZZ1)|(1ULL<<BUZZ2));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Inputs (buttons)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = ((1ULL<<BTN1)|(1ULL<<BTN2));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1; // INPUT_PULLUP
    gpio_config(&io_conf);

    // ----------------- Queue -----------------
    pedRequestQueue = xQueueCreate(4,sizeof(int));

    // ----------------- Tasks -----------------
    xTaskCreate(trafficTask,"traffic",4096,NULL,1,&trafficTaskHandle);
    xTaskCreate(pedestrianTask,"pedestrian",4096,NULL,2,&pedestrianTaskHandle);
    xTaskCreate(buzzerTask,"buzzer",4096,NULL,1,&buzzerTaskHandle);
    xTaskCreate(buttonTask,"btn1",2048,(void*)BTN1,3,NULL);
    xTaskCreate(buttonTask,"btn2",2048,(void*)BTN2,3,NULL);
}
