# **README.md**

## **ESP32 ISR + FreeRTOS Timer Assignment**

This project demonstrates how to use **FreeRTOS tasks, binary semaphores, and timers** on an ESP32 to handle ADC readings.

It includes two modes:

1. **Print raw ADC readings every second**
2. **Print a rolling average of the last five ADC readings**

The program is compatible with **Arduino-ESP32 v3.x** and can be run on the  **Wokwi simulator** .

---

## **1. Overview**

* A **software timer** triggers an ADC read every 1 second.
* An **ISR-style callback** signals a binary semaphore after each reading.
* A **task** waits for the semaphore and prints the reading, either raw or as a rolling average.
* The **rolling average** keeps track of the last five readings for smoother output.

---

## **2. Features**

### **Mode 1 — Raw ADC Values**

* Prints the current analog reading from `A0` every second.
* Demonstrates **binary semaphore synchronization** between timer ISR and task.

### **Mode 2 — Rolling Average**

* Computes the average of the  **last 5 ADC readings** .
* Prints the result every second.
* Demonstrates **buffered data handling** and semaphore usage.

---

## **3. How It Works**

1. **Binary Semaphore**
   * Used to signal the task that new ADC data is ready.
   * Ensures the task waits for data instead of busy-waiting.
2. **FreeRTOS Software Timer**
   * Replaces the older `hw_timer_t` API from Arduino-ESP32 v2.x.
   * Triggers a callback every 1 second to read ADC.
3. **Task**
   * Waits for semaphore (`xSemaphoreTake`)
   * Prints ADC value (raw or rolling average)
   * Loops forever

---

## **4. Running the Program**

### **Step-by-Step**

1. Open **Wokwi ESP32 Project**
2. Replace `sketch.ino` with the provided code
3. Run simulation
4. Open the **Serial Monitor**
5. Enter:
   * `1` → Mode 1: raw ADC values
   * `2` → Mode 2: rolling average
6. Observe the output printed every second
7. Take a **screenshot** of the Serial Monitor for submission

---

## **5. Example Output**

### **Mode 1**

<pre class="overflow-visible!" data-start="2187" data-end="2326"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>===</span><span></span><span>FreeRTOS</span><span></span><span>ISR</span><span></span><span>+</span><span></span><span>Timer</span><span></span><span>Demo</span><span></span><span>===</span><span>
</span><span>Select mode:</span><span></span><span>1</span><span>=Raw,</span><span></span><span>2</span><span>=Rolling</span><span></span><span>average</span><span>
</span><span>Selected mode:</span><span></span><span>1</span><span>
</span><span>Raw ADC:</span><span></span><span>512</span><span>
</span><span>Raw ADC:</span><span></span><span>515</span><span>
</span><span>Raw ADC:</span><span></span><span>509</span><span>
</span><span>...</span><span>
</span></span></code></div></div></pre>

### **Mode 2**

<pre class="overflow-visible!" data-start="2343" data-end="2573"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>=</span><span>=</span><span>=</span><span> FreeRTOS ISR </span><span>+</span><span> Timer Demo </span><span>=</span><span>=</span><span>=</span><span>
</span><span>Select</span><span> mode: </span><span>1</span><span>=</span><span>Raw, </span><span>2</span><span>=</span><span>Rolling average
Selected mode: </span><span>2</span><span>
Rolling </span><span>avg</span><span> (</span><span>last</span><span></span><span>1</span><span>): </span><span>512</span><span>
Rolling </span><span>avg</span><span> (</span><span>last</span><span></span><span>2</span><span>): </span><span>513</span><span>
Rolling </span><span>avg</span><span> (</span><span>last</span><span></span><span>3</span><span>): </span><span>510</span><span>
Rolling </span><span>avg</span><span> (</span><span>last</span><span></span><span>4</span><span>): </span><span>511</span><span>
Rolling </span><span>avg</span><span> (</span><span>last</span><span></span><span>5</span><span>): </span><span>512</span><span>
...
</span></span></code></div></div></pre>

---

## **6. Notes**

* Uses **core 1** to ensure predictable task execution and Serial output.
* Semaphore guarantees  **task only prints when new ADC data is ready** .
* Rolling buffer size is **5 readings** for smoother visualization.
* Fully compatible with  **Arduino-ESP32 v3.x / Wokwi** .
