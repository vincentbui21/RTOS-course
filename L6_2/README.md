## **FreeRTOS Counting Semaphore Assignment**

This project demonstrates the use of **counting semaphores** in FreeRTOS through two separate exercises:

1. **Synchronizing five task creations using a counting semaphore**
2. **Creating a dynamic number of tasks based on a user-entered sentence, where each task receives one word**

The program runs on an ESP32 using FreeRTOS and is fully compatible with  **Wokwi** .

---

## **1. Overview**

FreeRTOS allows multiple tasks to run independently while sharing system resources.

A **counting semaphore** helps coordinate these tasks by counting events or signals.

In this assignment:

* Each task receives a `Message` struct containing text.
* When a task starts, it **signals the semaphore** to confirm it successfully read the input parameter.
* The main program waits for **all tasks** to signal before printing a completion message.

The project includes  **two selectable modes** , chosen by entering `1` or `2` in the Serial Monitor.

---

## **2. Features**

### **Mode 1 – Fixed 5-Task Synchronized Demo**

* Creates 5 tasks.
* All tasks receive the same message: `"All your base"`.
* Each task prints the text and its length.
* A counting semaphore ensures the main program waits until all tasks finish reading the shared message.

### **Mode 2 – Sentence Word-Task Generator**

* Asks the user to input a sentence.
* Splits the sentence into individual words.
* Creates one task per word.
* Each task receives its  **own Message struct** , containing only its assigned word.
* Counting semaphore ensures synchronization.

---

## **3. How It Works**

### **Counting Semaphore**

A counting semaphore is created with:

* **Maximum count:** number of tasks
* **Initial count:** 0

Every task performs:

1. Copy parameter
2. `xSemaphoreGive()` to signal completion
3. Print its message
4. Delete itself

The main program performs:

1. Creates all tasks
2. Calls `xSemaphoreTake()` repeatedly
3. Continues only after *all tasks have signaled*

---

## **4. Running on Wokwi**

### **Step-by-step**

1. Open **wokwi.com**
2. Create a new ESP32 project
3. Replace the `sketch.ino` with the provided program
4. Open the Serial Monitor
5. Reset or start simulation
6. When asked:
   * Type **1** for the 5-task demo
   * Type **2** for the dynamic sentence mode
7. For Mode 2, type any sentence, such as:
   <pre class="overflow-visible!" data-start="2627" data-end="2665"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>embedded systems are </span><span>fun</span><span>
   </span></span></code></div></div></pre>

---

## **5. Example Outputs**

### **Mode 1 Output**

<pre class="overflow-visible!" data-start="2721" data-end="3109"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>===</span><span></span><span>FreeRTOS</span><span></span><span>Counting</span><span></span><span>Semaphore</span><span></span><span>Demo</span><span></span><span>===</span><span>
</span><span>Select mode:</span><span>
</span><span>1</span><span></span><span>=</span><span></span><span>Five-task</span><span></span><span>synchronized</span><span></span><span>demo</span><span>
</span><span>2</span><span></span><span>=</span><span></span><span>Sentence</span><span></span><span>word-task</span><span></span><span>generator</span><span>
</span><span>Selected mode:</span><span></span><span>1</span><span>

</span><span>---</span><span></span><span>MODE 1:</span><span></span><span>Creating</span><span></span><span>5</span><span></span><span>synchronized</span><span></span><span>tasks</span><span></span><span>---</span><span>
</span><span>Received:</span><span></span><span>All</span><span></span><span>your</span><span></span><span>base</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>13</span><span>
</span><span>Received:</span><span></span><span>All</span><span></span><span>your</span><span></span><span>base</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>13</span><span>
</span><span>Received:</span><span></span><span>All</span><span></span><span>your</span><span></span><span>base</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>13</span><span>
</span><span>Received:</span><span></span><span>All</span><span></span><span>your</span><span></span><span>base</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>13</span><span>
</span><span>Received:</span><span></span><span>All</span><span></span><span>your</span><span></span><span>base</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>13</span><span>
</span><span>All</span><span></span><span>tasks</span><span></span><span>created</span><span></span><span>in</span><span></span><span>MODE</span><span></span><span>1</span><span>
</span></span></code></div></div></pre>

### **Mode 2 Output**

<pre class="overflow-visible!" data-start="3133" data-end="3401"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>Enter a sentence:</span><span>
</span><span>hello</span><span></span><span>from</span><span></span><span>embedded</span><span></span><span>systems</span><span></span><span>class</span><span>

</span><span>---</span><span></span><span>MODE 2:</span><span></span><span>Creating</span><span></span><span>5</span><span></span><span>tasks</span><span></span><span>based</span><span></span><span>on</span><span></span><span>sentence</span><span></span><span>---</span><span>
</span><span>Received:</span><span></span><span>hello</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>5</span><span>
</span><span>Received:</span><span></span><span>from</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>4</span><span>
</span><span>Received:</span><span></span><span>embedded</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>8</span><span>
</span><span>Received:</span><span></span><span>systems</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>7</span><span>
</span><span>Received:</span><span></span><span>class</span><span></span><span>|</span><span></span><span>len:</span><span></span><span>5</span><span>
</span><span>All</span><span></span><span>tasks</span><span></span><span>created</span><span></span><span>in</span><span></span><span>MODE</span><span></span><span>2</span><span>
</span></span></code></div></div></pre>

---

## **6. Files Included**

* `L6_2.ino` – Full implementation of both modes
* `README.md` – Documentation for assignment submission

---

## **7. Notes**

* All tasks run on **core 1** to keep output ordered and easy to observe.
* Each task has its own stack allocation to avoid stack overflow.
* Word buffers are capped at 19 characters to match the `Message` struct.
