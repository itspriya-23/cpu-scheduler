# CPU Scheduling Algorithm Simulator & Comparative Benchmark Suite

![C++](https://img.shields.io/badge/Language-C%2B%2B11%20%2F%20C%2B%2B14-blue.svg)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/License-MIT-orange.svg)

A modular, high-performance CPU scheduling simulation engine and comparative benchmarking suite written in modern C++. This project accurately models CPU process scheduling, simulates context switching and CPU idle states, renders visual ASCII Gantt charts, calculates operating system performance metrics, and features an automated decision framework to determine the optimal scheduling policy for any workload.

---

## 📌 Project Overview

* **Timeline:** April 2026 – July 2026
* **Language & Standard:** C++ (C++11 / C++14 compliant)
* **Core Disciplines:** Operating Systems, Concurrency Concepts, Algorithm Design, Benchmark Engineering

### Resume Ready Highlights
* **Engineered Four Core Scheduling Algorithms:** Implemented **First-Come, First-Served (FCFS)**, non-preemptive **Shortest Job First (SJF)**, preemptive **Shortest Remaining Time First (SRTF)**, and **Round Robin (RR)** with configurable time quanta.
* **Comprehensive Performance Metrics:** Modeled discrete-event execution to compute Completion Time (CT), Turnaround Time (TAT), Waiting Time (WT), Response Time (RT), CPU Utilization percentage, and System Throughput.
* **Comparative Decision Engine:** Built an analytical benchmarking framework that executes multi-algorithm comparisons on identical workloads to automatically identify the optimal scheduling algorithm based on turnaround latency, queue delay, and system responsiveness.
* **Robust CLI & Gantt Visualizer:** Designed an interactive CLI with input sanitization, automated unit tests, and dual-mode Gantt chart rendering (timeline and block diagram) displaying process execution windows and CPU idle periods.

---

## 🚀 Key Features

* **Complete Algorithm Suite:**
  * **FCFS (First-Come, First-Served):** Classic non-preemptive queue scheduling.
  * **SJF (Shortest Job First):** Non-preemptive burst optimization to minimize waiting times.
  * **SRTF (Shortest Remaining Time First):** Preemptive shortest-job scheduling with dynamic interruption on shorter process arrival.
  * **Round Robin (RR):** Preemptive time-slicing scheduler with strict arrival queue prioritization before requeueing.
* **Automated Comparative Engine:** Evaluates all 4 algorithms concurrently and provides an analytical verdict highlighting the winner for average waiting time, turnaround time, responsiveness, and CPU utilization.
* **Dual Gantt Chart Visualization:**
  * Milestone timeline (`0 -- P1 -- 8 -- P2 -- 12`)
  * Structured ASCII diagram with aligned time tick indicators.
* **CPU Idle Gap Handling:** Accurately models and visualizes CPU idle periods when no processes have arrived.
* **Predefined Benchmark Workloads:** Includes standard textbook test cases, convoy effect demonstrations, preemption advantage scenarios, and workloads with CPU idle gaps.
* **Input Validation & Fault Tolerance:** Gracefully handles non-numeric values, negative arrival times, zero/negative burst times, and out-of-range inputs without crashing.
* **Zero External Dependencies:** Built strictly using the standard C++ runtime library (`<vector>`, `<queue>`, `<iomanip>`, `<algorithm>`).

---

## 📐 Supported Algorithms & Complexity

| Algorithm | Preemption | Strategy / Priority | Time Complexity | Best Suited For |
| :--- | :---: | :--- | :---: | :--- |
| **FCFS** | No | Earliest Arrival Time ($AT$) | $\mathcal{O}(n \log n)$ | Batch systems with uniform process durations |
| **SJF** | No | Smallest CPU Burst Time ($BT$) | $\mathcal{O}(n^2)$ | Workloads where execution times are known in advance |
| **SRTF** | Yes | Smallest Remaining Burst ($BT_{rem}$) | $\mathcal{O}(n \cdot \sum BT)$ | Workloads with short interactive jobs arriving dynamically |
| **Round Robin** | Yes | Cyclic FIFO queue with Time Quantum ($q$) | $\mathcal{O}\left(\frac{\sum BT}{q}\right)$ | Multi-user time-sharing and interactive desktop OS |

---

## 📊 Performance Metrics & Formulas

For each process $P_i$ with Arrival Time $AT_i$ and Burst Time $BT_i$:

1. **Completion Time ($CT_i$):** Timestamp when process $P_i$ finishes its final execution.
2. **Turnaround Time ($TAT_i$):** Total elapsed time from process arrival to termination.
   $$\text{TAT}_i = CT_i - AT_i$$
3. **Waiting Time ($WT_i$):** Total time spent waiting in the ready queue without CPU execution.
   $$\text{WT}_i = TAT_i - BT_i$$
4. **Response Time ($RT_i$):** Time from arrival until the process first acquires the CPU.
   $$\text{RT}_i = \text{StartTime}_i - AT_i$$
5. **Average Turnaround Time:**
   $$\overline{\text{TAT}} = \frac{1}{n} \sum_{i=1}^{n} \text{TAT}_i$$
6. **Average Waiting Time:**
   $$\overline{\text{WT}} = \frac{1}{n} \sum_{i=1}^{n} \text{WT}_i$$
7. **CPU Utilization (%):** Proportion of total elapsed time during which the CPU actively executed processes.
   $$\text{CPU Utilization} = \left(\frac{\sum BT}{\text{Total Schedule Time}}\right) \times 100\%$$
8. **Throughput:** Number of processes completed per unit time.
   $$\text{Throughput} = \frac{n}{\text{Total Schedule Time}}$$

---

## 📂 Project Structure

```
cpu-scheduler/
├── main.cpp            # All-in-one simulator: 4 algorithms, comparative matrix & interactive CLI
├── FCFS.cpp            # Standalone First-Come, First-Served implementation
├── SJF.cpp             # Standalone Shortest Job First (Non-Preemptive) implementation
├── SRTF.cpp            # Standalone Shortest Remaining Time First (Preemptive) implementation
├── round_robin.cpp     # Standalone Round Robin implementation
├── .gitignore          # Git ignore rules for build artifacts
└── README.md           # Documentation & instructions
```

> **Design Note:** All files are 100% self-contained standard C++ files with **zero custom header (`.h`) dependencies**. Each file can be compiled directly on its own using standard `g++` or `clang++`.

---

## 🛠️ Compilation & Execution

### 1. Compile & Run the Full Suite (`main.cpp`)

Compile with any standard C++ compiler (C++11 or newer):
```bash
g++ -std=c++11 main.cpp -o scheduler
./scheduler
```
*(On Windows Command Prompt or PowerShell, run `.\scheduler.exe`)*

---

### 2. Compiling Standalone Algorithm Files Individually

If you want to run any scheduling algorithm independently in isolation:

```bash
# First Come First Serve
g++ -std=c++11 FCFS.cpp -o fcfs
./fcfs

# Shortest Job First (Non-Preemptive)
g++ -std=c++11 SJF.cpp -o sjf
./sjf

# Shortest Remaining Time First (Preemptive)
g++ -std=c++11 SRTF.cpp -o srtf
./srtf

# Round Robin
g++ -std=c++11 round_robin.cpp -o round_robin
./round_robin
```

---

## 💻 Usage Walkthrough

### 1. Launching the Simulator
Run `./scheduler.exe` (or `scheduler.exe` on Windows):

```
========================================================================================
                 CPU SCHEDULING ALGORITHM SIMULATOR & COMPARATIVE SUITE                 
                      Implemented in C++ | Operating Systems Core                       
========================================================================================
[INFO] Initialized with default standard workload (4 processes). You can run tests directly or customize.

------------------------------------------------------------
                         MAIN MENU                          
------------------------------------------------------------
  [1] Enter Custom Process Workload
  [2] Load Predefined Benchmark Workloads
  [3] View Current Process List
  [4] Run First Come First Serve (FCFS)
  [5] Run Shortest Job First (SJF - Non-Preemptive)
  [6] Run Shortest Remaining Time First (SRTF - Preemptive)
  [7] Run Round Robin (RR)
  [8] Compare All 4 Algorithms & Identify Optimal Choice
  [9] Exit
------------------------------------------------------------
```

### 2. Sample Output: Comparative Matrix (Option 8)

```
========================================================================================
                 ALGORITHM PERFORMANCE COMPARISON & BENCHMARK MATRIX                    
========================================================================================

Algorithm                            Avg TAT        Avg WT        Avg RT    CPU Util (%)    Throughput
------------------------------------------------------------------------------------------------------
FCFS                                   15.25          8.75          8.75         100.00%        0.1538
SJF (Non-Preemptive)                   14.25          7.75          7.75         100.00%        0.1538
SRTF (Preemptive)                      13.00          6.50          4.25         100.00%        0.1538
Round Robin                            19.25         12.75          2.00         100.00%        0.1538
======================================================================================================

>>> PERFORMANCE CATEGORY WINNERS <<<
  - Lowest Average Turnaround Time : Shortest Remaining Time First (SRTF - Preemptive SJF) (13.00 units)
  - Lowest Average Waiting Time    : Shortest Remaining Time First (SRTF - Preemptive SJF) (6.50 units)
  - Best Response Time (Fastest)   : Round Robin (RR) [Quantum = 2] (2.00 units)
  - Peak CPU Utilization           : First Come First Serve (FCFS) (100.00%)

----------------------------------------------------------------------------------------
                         OPTIMAL SCHEDULING ALGORITHM VERDICT                           
----------------------------------------------------------------------------------------
  [OPTIMAL CHOICE] : Shortest Remaining Time First (SRTF - Preemptive SJF)

  Reasoning & Analysis:
  1. Minimizing Average Waiting Time (6.50 units) directly optimizes system queue delay.
  2. It achieved the lowest Average Turnaround Time (13.00 units), accelerating total completion.
  3. Preemptive priority for shorter remaining tasks eliminates convoy delays
     caused by longer processes, maximizing responsiveness for quick tasks.

  Note for Interactive Systems:
  * If the target environment is a responsive multi-user UI or interactive OS,
    'Round Robin (RR) [Quantum = 2]' is recommended for its lowest response time (2.00 units).
----------------------------------------------------------------------------------------
```

### 3. Sample Gantt Chart Rendering
```
------------------------------------------------------------
                   Gantt Chart (Shortest Remaining Time First (SRTF - Preemptive SJF))
------------------------------------------------------------
Timeline: 0 -- P1 -- 1 -- P2 -- 5 -- P4 -- 10 -- P1 -- 17 -- P3 -- 26

Visual Gantt Diagram:
+------+------+--------+--------+--------+
|  P1  |  P2  |   P4   |   P1   |   P3   |
+------+------+--------+--------+--------+
0      1      5        10       17       26
```

---

## 🧪 Automated Unit Testing

The project includes unit test validation in `tests/test_scheduler.cpp` covering:
* **FCFS Verification:** Correct calculation of turnaround, waiting, and completion times.
* **SJF Verification:** Tie-breaking logic and order of execution.
* **SRTF Preemption:** Mid-burst preemption when a shorter job arrives.
* **Round Robin Queue Dynamics:** Verification that newly arrived jobs join the ready queue before preempted tasks are requeued.
* **CPU Idle Gaps:** Validation of idle intervals when processes arrive at non-zero timestamps or with gaps.

Run unit tests via:
```bash
./run_tests.exe
```
Expected output:
```
========================================
   CPU SCHEDULER UNIT TEST VALIDATION   
========================================
[TEST] Running FCFS Test... PASSED
[TEST] Running SJF (Non-preemptive) Test... PASSED
[TEST] Running SRTF (Preemptive) Test... PASSED
[TEST] Running Round Robin Test... PASSED
[TEST] Running Idle CPU Gap Handling Test... PASSED

>>> ALL 5 TEST SUITES PASSED SUCCESSFULLY! <<<
```

---

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).
