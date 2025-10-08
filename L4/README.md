# FreeRTOS Memory Demo (ESP32-S3)

## Overview

This lab demonstrates a **stack overflow problem** in FreeRTOS on the **ESP32-S3** and how to fix it.

The issue occurs when a task uses a large local array, exceeding its small stack size.

The goal is to diagnose the error and apply safe memory management techniques.

---

## Problem

In the original code:

<pre class="overflow-visible!" data-start="635" data-end="676"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre! language-cpp"><span><span>int</span><span> b[</span><span>100</span><span>]; </span><span>// about 400 bytes</span><span>
</span></span></code></div></div></pre>

This array is created inside the task’s stack, which only has 1024 bytes.

Together with function calls like `Serial.println()`, this leads to a **stack overflow** and random resets.

---

## Solution

Two changes fix the issue:

1. **Move the large buffer off the task stack**
   * Allocate it globally or on the heap using `pvPortMalloc()`.
2. **Increase the task’s stack size**
   * From `1024` to `2048` words (≈8 KB) to give enough margin.

Additional safeguards were added:

* Stack-overflow hook: `vApplicationStackOverflowHook()`
* Stack usage check: `uxTaskGetStackHighWaterMark()`

---

## Fixed Code Summary

* The buffer `b[100]` is now a **global pointer** (`g_buf`) allocated once with `pvPortMalloc()`.
* The stack size for `xTaskCreatePinnedToCore()` is increased to  **2048 words** .
* The code periodically prints stack high-water marks and free heap size for monitoring.

---

## Expected Behavior

* Serial monitor shows:
  <pre class="overflow-visible!" data-start="1634" data-end="1785"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>--- FreeRTOS Memory Demo (fixed) ---</span><span>
  </span><span>2</span><span>
  [testTask </span><span>start</span><span>] Stack high</span><span>-</span><span>water mark: XXXX words (</span><span>~</span><span>YYYY bytes)
  [heap] </span><span>Free</span><span> heap: ZZZZZ bytes
  </span></span></code></div></div></pre>
* The program runs continuously without resets or overflow errors.
* Stack high-water mark remains safely above 300 words.

---

## How to Run

1. Open the fixed code in **Arduino IDE** or  **Wokwi ESP32 simulator** .
2. Upload or start the simulation.
3. Open **Serial Monitor** at 115200 baud.
4. Verify the expected printout and no resets.

---

## Key Learning Points

* FreeRTOS task stacks are limited — avoid large local variables.
* Use the heap (`pvPortMalloc`) or global memory for big buffers.
* Check stack margins with `uxTaskGetStackHighWaterMark()`.
* Proper stack sizing prevents silent crashes in real-time systems.

---

## Conclusion

By reallocating memory and increasing stack size, the stack overflow was eliminated.

The task now runs stably, demonstrating safe **memory management practices in FreeRTOS** on ESP32.
