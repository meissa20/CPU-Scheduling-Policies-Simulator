# Lab 6: CPU Scheduling Policies

## Overview

This project implements a simulation of various CPU scheduling algorithms using C/C++. The program reads input from `stdin` and prints either a **visual trace** of process scheduling or **statistical summaries** to `stdout`, depending on the mode selected.

## Supported Scheduling Algorithms

1. FCFS – First Come First Serve  
2. RR – Round Robin (with time quantum `q`)  
3. SPN – Shortest Process Next  
4. SRT – Shortest Remaining Time  
5. HRRN – Highest Response Ratio Next  
6. FB-1 – Feedback (quantum = 1 for all queues)  
7. FB-2i – Feedback (quantum = 2^i for i-th level queue)  
8. Aging – Priority Aging Scheduler (with aging factor `q`)

---

## Input Format

Input is passed via `stdin` and follows this structure:

1. Line 1: Mode – `"trace"` or `"stats"`  
2. Line 2: Comma-separated scheduling algorithms with optional parameters (e.g., `2-4,8-1`)  
3. Line 3: Simulation end time  
4. Line 4: Number of processes  
5. Line 5+: One line per process:
   - For algorithms 1–7: `<Name>,<Arrival Time>,<Service Time>`
   - For algorithm 8: `<Name>,<Arrival Time>,<Priority>`

Use input redirection for convenience:
```bash
./lab6 < 01a-input.txt
# or
cat 01a-input.txt | ./lab6
```

---

## Output Format

- `"trace"` mode: Visual representation of CPU usage using `*` for running and `.` for waiting.
- `"stats"` mode: A table summarizing wait time, turnaround time, and normalized turnaround for each process.

Use `diff` to verify correctness against provided output:
```bash
cat 01a-input.txt | ./lab6 | diff 01a-output.txt -
```

---

## Build Instructions

Compile using the provided Makefile:
```bash
make
```

This produces an executable named `lab6`.

---
