#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ---------- Core selection (same as your original) ----------
#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

// ---------- Globals / heap allocations (NOT on task stack) ----------
static int *g_buf = nullptr;            // 100 * 4 = 400 bytes on heap
static const size_t BUF_LEN = 100;

// Helper: print stack watermark (min remaining stack) for this task
static void printStackWatermark(const char *tag) {
    UBaseType_t words_remaining = uxTaskGetStackHighWaterMark(NULL);
    // On ESP32, stack "size" and watermark are in words (4 bytes each)
    Serial.printf("[%s] Stack high-water mark: %u words (~%u bytes)\n",
                    tag, (unsigned)words_remaining, (unsigned)(words_remaining * 4));
}

// ---------- Task ----------
void testTask(void *parameter) {
    int a = 1;  // small local is fine

    // Sanity check: buffer must exist
    configASSERT(g_buf != NULL);

    // First print: see how much stack we have right after starting
    printStackWatermark("testTask start");

    while (1) {
        // Use the heap buffer (not a big local array!)
        for (size_t i = 0; i < BUF_LEN; i++) {
        g_buf[i] = a + 1;
        }

        Serial.println(g_buf[0]);

        // Periodically print remaining stack to prove we are safe
        static uint32_t counter = 0;
        if ((counter++ % 20) == 0) { // about every 2 seconds if delay=100 ms
        printStackWatermark("testTask loop");
        // Optional: show free heap too
        Serial.printf("[heap] Free heap: %u bytes\n", (unsigned)xPortGetFreeHeapSize());
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ---------- FreeRTOS stack-overflow hook (prints & halts/reset) ----------
extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  // Use low-level print because Serial may not be safe here
    ets_printf("\n*** Stack overflow in task: %s ***\n", pcTaskName ? pcTaskName : "unknown");
  // Halt; on some boards you may prefer esp_restart();
    abort();
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println();
    Serial.println("--- FreeRTOS Memory Demo (fixed) ---");

    // Allocate the buffer on HEAP (thread-safe FreeRTOS allocator)
    g_buf = (int *)pvPortMalloc(BUF_LEN * sizeof(int));
    if (!g_buf) {
        Serial.println("FATAL: pvPortMalloc failed for g_buf");
        abort();
    }

    // Create task with a safer stack depth.
    // NOTE: On ESP32, this parameter is IN WORDS (4 bytes each).
    // 2048 words = ~8 KB, comfortable for Serial prints and small locals.
    const uint32_t STACK_WORDS = 2048;

    BaseType_t ok = xTaskCreatePinnedToCore(
                        testTask,
                        "Test Task",
                        STACK_WORDS,
                        NULL,
                        1,
                        NULL,
                        app_cpu);

    if (ok != pdPASS) {
        Serial.println("FATAL: xTaskCreatePinnedToCore failed");
        abort();
    }

    // We no longer need Arduino's default task
    vTaskDelete(NULL);
}

void loop() {
  // never reached
}