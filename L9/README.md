# **ESP32 FreeRTOS MultiCore Demo**

This project demonstrates task execution on **multiple cores** of the ESP32 using FreeRTOS. It shows the effect of **core pinning** and **task affinity** on task scheduling and execution.

---

## **1. Overview**

ESP32 has  **dual cores** :

* **Core 0 (PRO CPU)** – typically runs system tasks.
* **Core 1 (APP CPU)** – used for application tasks.

FreeRTOS allows tasks to be pinned to a specific core or run with  **no affinity** .

This exercise explores:

1. Running multiple tasks on the  **same core** .
2. Running tasks on  **separate cores** .
3. Running tasks with **no affinity** (tasks can migrate between cores).

---

## **2. Tasks**

Two tasks are used in this demo:

1. **Task L (Low Priority)** – prints `"Task L, Core X"` and hogs CPU for a short time.
2. **Task H (High Priority)** – prints `"Task H, Core X"` and hogs CPU for a short time.

CPU hogging is implemented with a busy loop to simulate computation.

---

## **3. Experiment Steps**

### **Option 1: Both tasks on the same core**

<pre class="overflow-visible!" data-start="1157" data-end="1303"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre! language-cpp"><span><span>xTaskCreatePinnedToCore</span><span>(doTaskL, </span><span>"Task L"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>1</span><span>, </span><span>NULL</span><span>, </span><span>0</span><span>);
</span><span>xTaskCreatePinnedToCore</span><span>(doTaskH, </span><span>"Task H"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>2</span><span>, </span><span>NULL</span><span>, </span><span>0</span><span>);
</span></span></code></div></div></pre>

**Observation:**

* Both tasks run on  **Core 0** .
* Task H preempts Task L due to higher priority.
* No resets occur, but CPU hogging delays Task L.

---

### **Option 2: Tasks pinned to separate cores**

<pre class="overflow-visible!" data-start="1513" data-end="1659"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre! language-cpp"><span><span>xTaskCreatePinnedToCore</span><span>(doTaskL, </span><span>"Task L"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>1</span><span>, </span><span>NULL</span><span>, </span><span>0</span><span>);
</span><span>xTaskCreatePinnedToCore</span><span>(doTaskH, </span><span>"Task H"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>2</span><span>, </span><span>NULL</span><span>, </span><span>1</span><span>);
</span></span></code></div></div></pre>

**Observation:**

* Task L runs on  **Core 0** , Task H runs on  **Core 1** .
* Tasks execute concurrently without preempting each other.
* CPU hogging on one core does not block the other.
* Smooth execution, no resets.

---

### **Option 3: No affinity**

<pre class="overflow-visible!" data-start="1920" data-end="2092"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre! language-cpp"><span><span>xTaskCreatePinnedToCore</span><span>(doTaskL, </span><span>"Task L"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>1</span><span>, </span><span>NULL</span><span>, tskNO_AFFINITY);
</span><span>xTaskCreatePinnedToCore</span><span>(doTaskH, </span><span>"Task H"</span><span>, </span><span>2048</span><span>, </span><span>NULL</span><span>, </span><span>2</span><span>, </span><span>NULL</span><span>, tskNO_AFFINITY);
</span></span></code></div></div></pre>

**Observation:**

* Tasks can run on  **any core** .
* Serial output shows tasks switching between cores.
* Execution remains correct; context switching slightly increases.
* No resets occur.

---

## **4. Serial Output Examples**

**Pinned to separate cores:**

<pre class="overflow-visible!" data-start="2360" data-end="2431"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>Task</span><span> L, Core </span><span>0</span><span>
</span><span>Task</span><span> H, Core </span><span>1</span><span>
</span><span>Task</span><span> L, Core </span><span>0</span><span>
</span><span>Task</span><span> H, Core </span><span>1</span><span>
...
</span></span></code></div></div></pre>

**No affinity:**

<pre class="overflow-visible!" data-start="2450" data-end="2521"><div class="contain-inline-size rounded-2xl relative bg-token-sidebar-surface-primary"><div class="sticky top-9"><div class="absolute end-0 bottom-0 flex h-9 items-center pe-2"><div class="bg-token-bg-elevated-secondary text-token-text-secondary flex items-center gap-4 rounded-sm px-2 font-sans text-xs"></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="whitespace-pre!"><span><span>Task</span><span> L, Core </span><span>0</span><span>
</span><span>Task</span><span> H, Core </span><span>1</span><span>
</span><span>Task</span><span> L, Core </span><span>1</span><span>
</span><span>Task</span><span> H, Core </span><span>0</span><span>
...
</span></span></code></div></div></pre>

---

## **5. Notes**

* Use Serial Monitor at  **115200 baud** .
* Changing core pinning affects **task concurrency** and  **preemption behavior** .
* This exercise helps understand **FreeRTOS scheduling** and  **multi-core ESP32 behavior** .
