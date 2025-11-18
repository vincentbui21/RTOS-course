# FreeRTOS Binary Semaphore Demo

### Exercise L-6: Passing Stack-Based Arguments Using a Binary Semaphore

### Platform: ESP32 on Wokwi Simulator

This project demonstrates how to safely pass a **stack-based argument** to a FreeRTOS task using a **binary semaphore**.
The exercise follows the requirement from the RTOS course: create a task, pass a value from the stack, and ensure the parent function does not exit before the task finishes copying the data.

---

## 📌 Purpose of This Exercise

In FreeRTOS, if we pass a **stack variable** to a task, the variable may go out of scope before the task starts running.To prevent this, a **binary semaphore** is used:

1. The parent task (setup) creates the new task.
2. The new task immediately copies the argument.
3. The new task gives the binary semaphore.
4. The parent task waits (`xSemaphoreTake`) until the child confirms it has copied the data.
5. No invalid memory access occurs.

This is the core concept being tested.

---

## 📌 What the Program Does

1. The user enters a delay time (in milliseconds) through the Serial Monitor.
2. This value is passed as a parameter to the `blinkLED` task.
3. The `blinkLED` task:

   - Copies the argument
   - Releases the semaphore to confirm safe usage
   - Prints the received value
   - Blinks the onboard LED forever using the specified delay
4. The main task prints **“Done!”** after the semaphore is returned.

This verifies that the argument was safely passed using the binary semaphore.

---

## 📌 Why a Binary Semaphore?

A binary semaphore is used as a **synchronization mechanism** between:

- the main task (setup), and
- the newly created task (`blinkLED`)

It ensures:

- The new task has time to copy the argument off the stack
- The parent does not continue execution too early
- No race conditions occur

---

## 📌 Running the Project on Wokwi

Wokwi supports FreeRTOS and Serial input for ESP32.Follow these steps:

1. Open: https://wokwi.com/projects/new/esp32
2. Replace `sketch.ino` with the code provided
3. Click **Run ▶**
4. Open the **Serial Monitor**
5. When prompted:

Enter a number for delay (milliseconds)

css
Copy code

6. Enter a value, for example:

500

arduino
Copy code

7. The Serial output will show:

Sending: 500
Received: 500
Done!

yaml
Copy code

8. The ESP32’s built-in LED will start blinking every 500 ms.

Take a screenshot of this output (as required for the assignment).

---

## 📌 Expected Output Example

---FreeRTOS Binary Semaphore Demo---
Enter a number for delay (milliseconds)
Sending: 500
Received: 500
Done!

yaml
Copy code

LED blinks ON/OFF with the selected delay.

---

## 📌 Files Included

- `sketch.ino` – Main program using FreeRTOS and binary semaphore
- `README.md` – Explanation for the assignment

---

## ✔ Summary

This assignment demonstrates correct and safe use of a **binary semaphore** to protect **stack-based arguments** when creating FreeRTOS tasks.
The program is fully compatible with the Wokwi ESP32 simulator and meets the course requirements.
