## **ESP32 FreeRTOS Deadlock Demo**

This project demonstrates **deadlock** scenarios in FreeRTOS and shows how to prevent them using a  **mutex hierarchy** .

---

## **1. Overview**

* A **deadlock** occurs when two or more tasks are **waiting indefinitely** for resources held by each other.
* In this demo, we simulate deadlock with  **two tasks** :
  1. **Task A** – High priority, accesses two shared mutexes (`mutex_1` and `mutex_2`).
  2. **Task B** – Low priority, accesses the same mutexes but originally in reverse order.
* Original deadlock scenario:
  * Task A holds `mutex_1` and waits for `mutex_2`.
  * Task B holds `mutex_2` and waits for `mutex_1`.
  * Both tasks are blocked indefinitely.

---

## **2. Solution: Mutex Hierarchy**

* To prevent deadlock, both tasks  **acquire mutexes in the same order** :
  * `mutex_1` → `mutex_2`
* This ensures **no circular waiting** occurs:
  * Task A and Task B cannot block each other indefinitely.
  * Tasks complete their critical sections safely.

---

## **3. Task Flow**

1. **Task A** :

* Takes `mutex_1` → prints message
* Takes `mutex_2` → prints message
* Performs critical section work
* Releases `mutex_2` and `mutex_1`

1. **Task B** :

* Follows the  **same order** : `mutex_1` → `mutex_2`
* Critical section execution is identical

1. **Delays** :

* Small delays between mutex acquisitions simulate realistic task timing.
* After completing work, each task sleeps briefly before the next cycle.

---

## **4. Running the Program**

1. Open the code in **Wokwi ESP32** or **Arduino IDE** with an ESP32 board.
2. Upload and run the sketch.
3. Open the **Serial Monitor** at 115200 baud.
4. Observe the task execution without deadlock:
   * Task A and Task B take turns entering the critical section.
   * No task is blocked indefinitely.

---

## **5. Example Serial Output**

<pre class="overflow-visible!" data-start="2026" data-end="2328"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>-</span><span>--FreeRTOS</span><span> Deadlock Demo (Fixed with Mutex Hierarchy)---
Task </span><span>A</span><span> took mutex </span><span>1</span><span>
Task </span><span>A</span><span> took mutex </span><span>2</span><span>
Task </span><span>A</span><span> doing some work
Task </span><span>A</span><span> going </span><span>to</span><span> sleep
Task </span><span>B</span><span> took mutex </span><span>1</span><span>
Task </span><span>B</span><span> took mutex </span><span>2</span><span>
Task </span><span>B</span><span> doing some work
Task </span><span>B</span><span> going </span><span>to</span><span> sleep
Task </span><span>A</span><span> took mutex </span><span>1</span><span>
Task </span><span>A</span><span> took mutex </span><span>2</span><span>
Task </span><span>A</span><span> doing some work
...
</span></span></code></div></div></pre>

* Tasks acquire mutexes  **in the same order** , preventing deadlocks.
* Both tasks execute critical sections repeatedly without blocking.

---

## **6. Notes**

* **Core usage:** Only core 1 is used for predictable task execution.
* **FreeRTOS mutexes** automatically provide  **priority inheritance** , helping with task scheduling.
* **Alternative methods** for deadlock prevention:
  * **Timeouts** on mutex acquisition.
  * **Global arbitrator mutex** .
* For this exercise, the **mutex hierarchy** method is simple and effective.
