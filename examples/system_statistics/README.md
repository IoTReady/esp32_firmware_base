# System Statistics Example

## Overview

This example introduces how to use our [system_stats](../../lib/system_stats) to view and print the real-time system statistics like tasks running, percentage of CPU used, etc.. 

The library used in this example are:
- [system_stats](../../lib/system_stats)

## How to use example
- Build and flash the example normally as any other ESP-IDF project. No special configuration is required during this process.
- The device will run three dummy tasks and print the system task stats on the console in the following format:
````
I (27534) SYSTEM STATS: | Task | Run Time | Percentage
I (27534) SYSTEM STATS: | system_stats_ta | 904 | 0%
I (27534) SYSTEM STATS: | IDLE1 | 1000000 | 50%
I (27534) SYSTEM STATS: | IDLE0 | 999080 | 49%
I (27544) SYSTEM STATS: | task2 | 0 | 0%
I (27544) SYSTEM STATS: | task3 | 0 | 0%
I (27554) SYSTEM STATS: | task1 | 16 | 0%
I (27554) SYSTEM STATS: | Tmr Svc | 0 | 0%
I (27564) SYSTEM STATS: | ipc1 | 0 | 0%
I (27564) SYSTEM STATS: | ipc0 | 0 | 0%
I (27564) SYSTEM STATS: | esp_timer | 0 | 0%
````
- The timing interval in which the statistics are printed can be changed by the `REPORT_DELAY` definition in [tasks.c](./main/tasks.c)

### Hardware Required

To run this example, you need:
- An ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) or ESP32 core board (e.g. ESP32-DevKitC).
