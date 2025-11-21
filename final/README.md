# ESP32-S3 FreeRTOS Traffic Light System (ESP-IDF)

This project implements a full traffic light simulation on the ESP32-S3 using ESP-IDF and FreeRTOS.

It includes dual traffic lanes, pedestrian crossing logic, hardware debounced button requests, and synchronized tasks controlling lights and buzzers.

## Features

* Two independent traffic light sets (red, yellow, green)
* Two pedestrian crossing indicators (white LEDs)
* Two pedestrian request buttons with internal pull-ups
* Two buzzers for audible crossing feedback
* Fully asynchronous FreeRTOS architecture
* Task-to-task synchronization using queues and notifications
* Realistic state machine: green - yellow - red with pedestrian interrupt logic

## Hardware Overview

| Component       | Quantity | Notes                                       |
| --------------- | -------- | ------------------------------------------- |
| Traffic LEDs    | 6        | Red1/Yellow1/Green1 and Red2/Yellow2/Green2 |
| Pedestrian LEDs | 2        | White pedestrian lights                     |
| Buttons         | 2        | INPUT_PULLUP, active LOW                    |
| Buzzers         | 2        | Beep pattern during walk time               |
| ESP32-S3        | 1        | Running ESP-IDF FreeRTOS                    |

### Pin Mapping

| Function | Pin |
| -------- | --- |
| RED1     | 2   |
| YELLOW1  | 3   |
| GREEN1   | 4   |
| RED2     | 5   |
| YELLOW2  | 6   |
| GREEN2   | 7   |
| PED1     | 8   |
| PED2     | 9   |
| BUZZ1    | 10  |
| BUZZ2    | 11  |
| BTN1     | 12  |
| BTN2     | 13  |

## System Architecture

### Core Tasks

#### 1. **trafficTask**

Handles the main light cycle:

* Normal operation: green → yellow → red
* Listens for incoming pedestrian requests via a queue
* Switches to pedestrian mode when requested
* Waits for pedestrian task to finish before resuming traffic flow

#### 2. **pedestrianTask**

Manages pedestrian crossing logic:

* Waits for traffic task notification
* Flashes LEDs before walk phase
* Turns on steady walk LEDs
* Notifies buzzer task to start beeping
* Signals traffic task when the crossing phase ends

#### 3. **buzzerTask**

Plays beeping pattern during walk time:

* Activated via notification from pedestrian task
* Alternates ON/OFF every 150 ms until pedestrian phase ends

#### 4. **buttonTask**

Runs independently for each button:

* Detects button press (active LOW)
* Sends a pedestrian request to the queue
* Includes simple debounce delay

### Inter-Task Communication

| Mechanism          | Used For                                           |
| ------------------ | -------------------------------------------------- |
| Queue              | Sending pedestrian requests from buttons           |
| Task Notifications | Sync between traffic, pedestrian, and buzzer tasks |

## Timing Configuration

| Phase           | Duration |
| --------------- | -------- |
| Green           | 5000 ms  |
| Yellow          | 2000 ms  |
| Red             | 4000 ms  |
| Pedestrian Walk | 6000 ms  |

All timing is handled using `vTaskDelay()`.

## How It Works

1. System starts with normal traffic cycling.
2. User presses BTN1 or BTN2.
3. Button task posts a request to the queue.
4. Traffic task receives the request:
   * Finishes current safe transition to red
   * Notifies pedestrian task
5. Pedestrian task:
   * Flashes lights
   * Enables steady walk phase
   * Notifies buzzer task
6. After walk time:
   * Pedestrian lights turn off
   * Traffic task receives notification to resume
7. System returns to regular cycle.
