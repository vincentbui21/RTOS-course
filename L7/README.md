# **README.md**

## **ESP32 FreeRTOS Priority Inversion Demo**

This project demonstrates **priority inversion** and its resolution using **priority inheritance** on the ESP32 with FreeRTOS.

---

## **1. Overview**

* **Priority inversion** occurs when a **low-priority task** holds a resource needed by a  **high-priority task** , and a **medium-priority task** preempts the low-priority task, effectively blocking the high-priority task.
* This demo uses three tasks:
  1. **Task L** – Low priority, holds the lock.
  2. **Task M** – Medium priority, performs work independent of the lock.
  3. **Task H** – High priority, needs the lock held by Task L.
* Initially, a **binary semaphore** causes priority inversion.
* Replacing it with a **mutex** enables  **priority inheritance** , allowing Task L to temporarily inherit Task H’s priority, preventing preemption by Task M.

---

## **2. Features**

* Simulates priority inversion scenario.
* Resolves the issue using a  **mutex with priority inheritance** .
* Demonstrates how FreeRTOS handles task scheduling and synchronization.
* Provides timing information to illustrate wait times for the lock.

---

## **3. How It Works**

1. **Mutex for Critical Section**
   * Task L takes the mutex and performs work in a critical section.
   * Task H waits for the mutex. The mutex temporarily boosts Task L’s priority to match Task H’s.
   * Task M cannot preempt Task L during this period.
2. **Task Flow**
   * Task L prints messages when acquiring and releasing the mutex.
   * Task H prints messages when waiting and working with the mutex.
   * Task M executes independently but cannot block Task H indirectly anymore.
3. **Timing**
   * `cs_wait` = 250 ms (time Task L/H spend in critical section)
   * `med_wait` = 5000 ms (time Task M spends working)

---

## **4. Running the Program**

1. Open **Wokwi ESP32 Project** or Arduino IDE with an ESP32 board.
2. Paste the provided `sketch.ino` code.
3. Run the simulation.
4. Open the  **Serial Monitor** .
5. Observe the output:
   * Task L acquires the mutex first.
   * Task H waits for the mutex.
   * Task L temporarily inherits Task H’s priority, preventing Task M from preempting it.
   * Task H acquires the mutex as soon as Task L releases it.

---

## **5. Example Output**

<pre class="overflow-visible!" data-start="2447" data-end="2733"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>Task</span><span> L trying to take lock...
</span><span>Task</span><span> L got lock. Spent </span><span>1</span><span> ms waiting </span><span>for</span><span> lock. Doing some work...
</span><span>Task</span><span> H trying to take lock...
</span><span>Task</span><span> L releasing lock.
</span><span>Task</span><span> H got lock. Spent </span><span>1</span><span> ms waiting </span><span>for</span><span> lock. Doing some work...
</span><span>Task</span><span> H releasing lock.
</span><span>Task</span><span> M doing some work...
</span><span>Task</span><span> M done!
...
</span></span></code></div></div></pre>

* Notice how **Task M never preempts Task L** while L holds the mutex — demonstrating that priority inversion has been resolved.

---

## **6. Notes**

* Uses **core 1** for predictable task execution and Serial output.
* The mutex automatically handles  **priority inheritance** , which is essential in real-time embedded systems to avoid unintentional blocking of high-priority tasks.
* Timing parameters (`cs_wait`, `med_wait`) can be adjusted to simulate different workloads and observe behavior.
