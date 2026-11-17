#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct Process {
    int pid;             // Process ID (1, 2, ...)
    int arrivalTime;     // Arrival Time (AT >= 0)
    int burstTime;       // CPU Burst Time (BT > 0)
    int remainingTime;   // Remaining burst time for preemptive algorithms

    // Computed metrics
    int startTime;       // First time process gets CPU (for Response Time)
    int completionTime;  // Completion Time (CT)
    int turnaroundTime;  // Turnaround Time (TAT = CT - AT)
    int waitingTime;     // Waiting Time (WT = TAT - BT)
    int responseTime;    // Response Time (RT = startTime - AT)

    Process()
        : pid(0), arrivalTime(0), burstTime(0), remainingTime(0),
          startTime(-1), completionTime(0), turnaroundTime(0), waitingTime(0), responseTime(0) {}

    Process(int id, int at, int bt)
        : pid(id), arrivalTime(at), burstTime(bt), remainingTime(bt),
          startTime(-1), completionTime(0), turnaroundTime(0), waitingTime(0), responseTime(0) {}
};

struct GanttSegment {
    int pid;        // Process ID, or -1 for CPU Idle
    int startTime;  // Start timestamp
    int endTime;    // End timestamp

    GanttSegment(int id, int start, int end)
        : pid(id), startTime(start), endTime(end) {}
};

struct ScheduleResult {
    std::string algorithmName;
    std::vector<Process> processes;
    std::vector<GanttSegment> gantt;

    double avgTurnaroundTime = 0.0;
    double avgWaitingTime = 0.0;
    double avgResponseTime = 0.0;
    double cpuUtilization = 0.0; // In percentage (0.0% - 100.0%)
    double throughput = 0.0;     // Processes completed per unit time
    int totalScheduleTime = 0;
    int totalBusyTime = 0;
    int totalIdleTime = 0;
};

// ============================================================================
// UTILITY & METRIC COMPUTATION FUNCTIONS
// ============================================================================

void consolidateGantt(std::vector<GanttSegment>& gantt) {
    if (gantt.empty()) return;

    std::vector<GanttSegment> consolidated;
    consolidated.push_back(gantt[0]);

    for (size_t i = 1; i < gantt.size(); ++i) {
        if (gantt[i].pid == consolidated.back().pid && gantt[i].startTime == consolidated.back().endTime) {
            consolidated.back().endTime = gantt[i].endTime;
        } else if (gantt[i].startTime < gantt[i].endTime) {
            consolidated.push_back(gantt[i]);
        }
    }
    gantt = consolidated;
}

void computeMetrics(ScheduleResult& result) {
    if (result.processes.empty()) return;

    size_t n = result.processes.size();
    int totalTAT = 0;
    int totalWT = 0;
    int totalRT = 0;
    int totalBT = 0;

    for (const auto& p : result.processes) {
        totalTAT += p.turnaroundTime;
        totalWT += p.waitingTime;
        totalRT += p.responseTime;
        totalBT += p.burstTime;
    }

    result.avgTurnaroundTime = static_cast<double>(totalTAT) / n;
    result.avgWaitingTime = static_cast<double>(totalWT) / n;
    result.avgResponseTime = static_cast<double>(totalRT) / n;
    result.totalBusyTime = totalBT;

    if (!result.gantt.empty()) {
        result.totalScheduleTime = result.gantt.back().endTime;
    } else {
        result.totalScheduleTime = totalBT;
    }

    int idleSum = 0;
    for (const auto& seg : result.gantt) {
        if (seg.pid == -1) {
            idleSum += (seg.endTime - seg.startTime);
        }
    }
    result.totalIdleTime = idleSum;

    if (result.totalScheduleTime > 0) {
        result.cpuUtilization = (static_cast<double>(result.totalBusyTime) / result.totalScheduleTime) * 100.0;
        result.throughput = static_cast<double>(n) / result.totalScheduleTime;
    } else {
        result.cpuUtilization = 0.0;
        result.throughput = 0.0;
    }
}

void clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getValidatedInt(const std::string& prompt, int minValue = 0, int maxValue = 1000000) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minValue && val <= maxValue) {
                return val;
            } else {
                std::cout << "[ERROR] Input must be between " << minValue << " and " << maxValue << ". Please retry.\n";
            }
        } else {
            std::cout << "[ERROR] Invalid input. Please enter a valid number.\n";
            clearInputStream();
        }
    }
}

// ============================================================================
// VISUALIZATION & OUTPUT FORMATTING
// ============================================================================

void displayGantt(const ScheduleResult& result) {
    if (result.gantt.empty()) {
        std::cout << "No Gantt chart data available.\n";
        return;
    }

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "                   Gantt Chart (" << result.algorithmName << ")\n";
    std::cout << "------------------------------------------------------------\n";

    // Compact Timeline
    std::cout << "Timeline: " << result.gantt.front().startTime;
    for (const auto& seg : result.gantt) {
        if (seg.pid == -1) {
            std::cout << " -- [IDLE] -- " << seg.endTime;
        } else {
            std::cout << " -- P" << seg.pid << " -- " << seg.endTime;
        }
    }
    std::cout << "\n\n";

    // ASCII Block Diagram
    std::string topBorder = "+";
    std::string midRow = "|";
    std::string botBorder = "+";
    std::vector<int> colWidths;

    for (const auto& seg : result.gantt) {
        std::string label = (seg.pid == -1) ? "Idle" : ("P" + std::to_string(seg.pid));
        int duration = seg.endTime - seg.startTime;
        int width = std::max(6, (int)label.length() + 2);
        if (duration > 4 && width < 8) width = 8;
        colWidths.push_back(width);
    }

    for (size_t i = 0; i < result.gantt.size(); ++i) {
        topBorder += std::string(colWidths[i], '-') + "+";
        botBorder += std::string(colWidths[i], '-') + "+";

        std::string label = (result.gantt[i].pid == -1) ? "Idle" : ("P" + std::to_string(result.gantt[i].pid));
        int padLeft = (colWidths[i] - (int)label.length()) / 2;
        int padRight = colWidths[i] - (int)label.length() - padLeft;
        midRow += std::string(padLeft, ' ') + label + std::string(padRight, ' ') + "|";
    }

    std::vector<int> tickPositions;
    tickPositions.push_back(0);
    int accum = 0;
    for (size_t i = 0; i < result.gantt.size(); ++i) {
        accum += colWidths[i] + 1;
        tickPositions.push_back(accum);
    }

    std::string timeLineStr(tickPositions.back() + 10, ' ');
    for (size_t i = 0; i < tickPositions.size(); ++i) {
        int pos = tickPositions[i];
        std::string num = (i == 0) ? std::to_string(result.gantt[0].startTime) : std::to_string(result.gantt[i - 1].endTime);
        for (size_t c = 0; c < num.length() && (pos + c) < timeLineStr.length(); ++c) {
            timeLineStr[pos + c] = num[c];
        }
    }
    while (!timeLineStr.empty() && timeLineStr.back() == ' ') timeLineStr.pop_back();

    std::cout << "Visual Gantt Diagram:\n";
    std::cout << topBorder << "\n";
    std::cout << midRow << "\n";
    std::cout << botBorder << "\n";
    std::cout << timeLineStr << "\n";
}

void displayProcessTable(const ScheduleResult& result) {
    std::cout << "\n========================================================================================================\n";
    std::cout << "                                         PROCESS SCHEDULE TABLE                                         \n";
    std::cout << "========================================================================================================\n";
    std::cout << std::left << std::setw(8) << "PID"
              << std::right << std::setw(14) << "Arrival (AT)"
              << std::setw(14) << "Burst (BT)"
              << std::setw(18) << "Completion (CT)"
              << std::setw(18) << "Turnaround (TAT)"
              << std::setw(16) << "Waiting (WT)"
              << std::setw(16) << "Response (RT)"
              << "\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";

    for (const auto& p : result.processes) {
        std::string pidStr = "P" + std::to_string(p.pid);
        std::cout << std::left << std::setw(8) << pidStr
                  << std::right << std::setw(14) << p.arrivalTime
                  << std::setw(14) << p.burstTime
                  << std::setw(18) << p.completionTime
                  << std::setw(18) << p.turnaroundTime
                  << std::setw(16) << p.waitingTime
                  << std::setw(16) << p.responseTime
                  << "\n";
    }
    std::cout << "--------------------------------------------------------------------------------------------------------\n";
}

void displayMetrics(const ScheduleResult& result) {
    std::cout << "\n========================================================================================\n";
    std::cout << "                          PERFORMANCE METRICS: " << result.algorithmName << "\n";
    std::cout << "========================================================================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  * Average Turnaround Time (TAT) : " << result.avgTurnaroundTime << " units\n";
    std::cout << "  * Average Waiting Time (WT)    : " << result.avgWaitingTime << " units\n";
    std::cout << "  * Average Response Time (RT)   : " << result.avgResponseTime << " units\n";
    std::cout << "  * CPU Utilization              : " << result.cpuUtilization << " %\n";
    std::cout << "  * Throughput                   : " << std::setprecision(4) << result.throughput << " processes/unit time\n";
    std::cout << "  * Total Execution Time         : " << result.totalScheduleTime << " units\n";
    std::cout << "  * Total CPU Busy Time          : " << result.totalBusyTime << " units\n";
    std::cout << "  * Total CPU Idle Time          : " << result.totalIdleTime << " units\n";
    std::cout << "========================================================================================\n";
}

void displayFullReport(const ScheduleResult& result) {
    std::cout << "\n****************************************************************************************\n";
    std::cout << "                   EXECUTION REPORT: " << result.algorithmName << "\n";
    std::cout << "****************************************************************************************\n";
    displayProcessTable(result);
    displayGantt(result);
    displayMetrics(result);
}

// ============================================================================
// 1. FIRST COME FIRST SERVE (FCFS)
// ============================================================================

ScheduleResult runFCFS(std::vector<Process> processes) {
    ScheduleResult result;
    result.algorithmName = "First Come First Serve (FCFS)";

    if (processes.empty()) return result;

    // Sort primarily by arrival time, break ties with PID
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime)
            return a.arrivalTime < b.arrivalTime;
        return a.pid < b.pid;
    });

    int currentTime = 0;

    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            result.gantt.push_back(GanttSegment(-1, currentTime, p.arrivalTime));
            currentTime = p.arrivalTime;
        }

        p.startTime = currentTime;
        currentTime += p.burstTime;
        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.responseTime = p.startTime - p.arrivalTime;

        result.gantt.push_back(GanttSegment(p.pid, p.startTime, p.completionTime));
    }

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    result.processes = processes;
    consolidateGantt(result.gantt);
    computeMetrics(result);

    return result;
}

// ============================================================================
// 2. SHORTEST JOB FIRST (SJF - NON-PREEMPTIVE)
// ============================================================================

ScheduleResult runSJF(std::vector<Process> processes) {
    ScheduleResult result;
    result.algorithmName = "Shortest Job First (SJF - Non-Preemptive)";

    if (processes.empty()) return result;

    int n = static_cast<int>(processes.size());
    std::vector<bool> completed(n, false);
    int completedCount = 0;
    int currentTime = 0;

    while (completedCount < n) {
        int bestIdx = -1;

        for (int i = 0; i < n; ++i) {
            if (!completed[i] && processes[i].arrivalTime <= currentTime) {
                if (bestIdx == -1) {
                    bestIdx = i;
                } else if (processes[i].burstTime < processes[bestIdx].burstTime) {
                    bestIdx = i;
                } else if (processes[i].burstTime == processes[bestIdx].burstTime) {
                    if (processes[i].arrivalTime < processes[bestIdx].arrivalTime) {
                        bestIdx = i;
                    } else if (processes[i].arrivalTime == processes[bestIdx].arrivalTime &&
                               processes[i].pid < processes[bestIdx].pid) {
                        bestIdx = i;
                    }
                }
            }
        }

        if (bestIdx != -1) {
            Process& p = processes[bestIdx];
            p.startTime = currentTime;
            int start = currentTime;
            currentTime += p.burstTime;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            p.responseTime = p.startTime - p.arrivalTime;

            completed[bestIdx] = true;
            completedCount++;

            result.gantt.push_back(GanttSegment(p.pid, start, currentTime));
        } else {
            int nextArrival = -1;
            for (int i = 0; i < n; ++i) {
                if (!completed[i]) {
                    if (nextArrival == -1 || processes[i].arrivalTime < nextArrival) {
                        nextArrival = processes[i].arrivalTime;
                    }
                }
            }
            if (nextArrival > currentTime) {
                result.gantt.push_back(GanttSegment(-1, currentTime, nextArrival));
                currentTime = nextArrival;
            } else {
                currentTime++;
            }
        }
    }

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    result.processes = processes;
    consolidateGantt(result.gantt);
    computeMetrics(result);

    return result;
}

// ============================================================================
// 3. SHORTEST REMAINING TIME FIRST (SRTF - PREEMPTIVE)
// ============================================================================

ScheduleResult runSRTF(std::vector<Process> processes) {
    ScheduleResult result;
    result.algorithmName = "Shortest Remaining Time First (SRTF - Preemptive SJF)";

    if (processes.empty()) return result;

    int n = static_cast<int>(processes.size());
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    int completedCount = 0;
    int currentTime = 0;

    int firstArrival = processes[0].arrivalTime;
    for (int i = 1; i < n; ++i) {
        if (processes[i].arrivalTime < firstArrival) {
            firstArrival = processes[i].arrivalTime;
        }
    }
    if (firstArrival > currentTime) {
        result.gantt.push_back(GanttSegment(-1, currentTime, firstArrival));
        currentTime = firstArrival;
    }

    while (completedCount < n) {
        int bestIdx = -1;

        for (int i = 0; i < n; ++i) {
            if (processes[i].remainingTime > 0 && processes[i].arrivalTime <= currentTime) {
                if (bestIdx == -1) {
                    bestIdx = i;
                } else if (processes[i].remainingTime < processes[bestIdx].remainingTime) {
                    bestIdx = i;
                } else if (processes[i].remainingTime == processes[bestIdx].remainingTime) {
                    if (processes[i].arrivalTime < processes[bestIdx].arrivalTime) {
                        bestIdx = i;
                    } else if (processes[i].arrivalTime == processes[bestIdx].arrivalTime &&
                               processes[i].pid < processes[bestIdx].pid) {
                        bestIdx = i;
                    }
                }
            }
        }

        if (bestIdx != -1) {
            Process& p = processes[bestIdx];
            if (p.startTime == -1) {
                p.startTime = currentTime;
            }

            p.remainingTime--;
            result.gantt.push_back(GanttSegment(p.pid, currentTime, currentTime + 1));
            currentTime++;

            if (p.remainingTime == 0) {
                p.completionTime = currentTime;
                p.turnaroundTime = p.completionTime - p.arrivalTime;
                p.waitingTime = p.turnaroundTime - p.burstTime;
                p.responseTime = p.startTime - p.arrivalTime;
                completedCount++;
            }
        } else {
            result.gantt.push_back(GanttSegment(-1, currentTime, currentTime + 1));
            currentTime++;
        }
    }

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    result.processes = processes;
    consolidateGantt(result.gantt);
    computeMetrics(result);

    return result;
}

// ============================================================================
// 4. ROUND ROBIN (RR)
// ============================================================================

ScheduleResult runRoundRobin(std::vector<Process> processes, int timeQuantum) {
    ScheduleResult result;
    result.algorithmName = "Round Robin (RR) [Quantum = " + std::to_string(timeQuantum) + "]";

    if (processes.empty()) return result;
    if (timeQuantum <= 0) timeQuantum = 1;

    int n = static_cast<int>(processes.size());
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.startTime = -1;
    }

    std::vector<int> sortedIndices(n);
    for (int i = 0; i < n; ++i) sortedIndices[i] = i;
    std::sort(sortedIndices.begin(), sortedIndices.end(), [&](int a, int b) {
        if (processes[a].arrivalTime != processes[b].arrivalTime)
            return processes[a].arrivalTime < processes[b].arrivalTime;
        return processes[a].pid < processes[b].pid;
    });

    std::queue<int> readyQueue;
    std::vector<bool> inQueueOrDone(n, false);

    int currentTime = 0;
    int completedCount = 0;

    int firstArrival = processes[sortedIndices[0]].arrivalTime;
    if (firstArrival > currentTime) {
        result.gantt.push_back(GanttSegment(-1, currentTime, firstArrival));
        currentTime = firstArrival;
    }

    for (int idx : sortedIndices) {
        if (processes[idx].arrivalTime <= currentTime && !inQueueOrDone[idx]) {
            readyQueue.push(idx);
            inQueueOrDone[idx] = true;
        }
    }

    while (completedCount < n) {
        if (readyQueue.empty()) {
            int nextArrival = -1;
            for (int idx : sortedIndices) {
                if (!inQueueOrDone[idx]) {
                    if (nextArrival == -1 || processes[idx].arrivalTime < nextArrival) {
                        nextArrival = processes[idx].arrivalTime;
                    }
                }
            }

            if (nextArrival > currentTime) {
                result.gantt.push_back(GanttSegment(-1, currentTime, nextArrival));
                currentTime = nextArrival;
            }

            for (int idx : sortedIndices) {
                if (processes[idx].arrivalTime <= currentTime && !inQueueOrDone[idx]) {
                    readyQueue.push(idx);
                    inQueueOrDone[idx] = true;
                }
            }

            if (readyQueue.empty()) {
                currentTime++;
                continue;
            }
        }

        int currIdx = readyQueue.front();
        readyQueue.pop();
        Process& p = processes[currIdx];

        if (p.startTime == -1) {
            p.startTime = currentTime;
        }

        int executionTime = std::min(timeQuantum, p.remainingTime);
        int start = currentTime;
        currentTime += executionTime;
        p.remainingTime -= executionTime;

        result.gantt.push_back(GanttSegment(p.pid, start, currentTime));

        // Add newly arriving processes before requeueing unfinished process
        for (int idx : sortedIndices) {
            if (processes[idx].arrivalTime <= currentTime && !inQueueOrDone[idx]) {
                readyQueue.push(idx);
                inQueueOrDone[idx] = true;
            }
        }

        if (p.remainingTime > 0) {
            readyQueue.push(currIdx);
        } else {
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            p.responseTime = p.startTime - p.arrivalTime;
            completedCount++;
        }
    }

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

    result.processes = processes;
    consolidateGantt(result.gantt);
    computeMetrics(result);

    return result;
}

// ============================================================================
// 5. COMPARISON ENGINE & OPTIMAL SELECTION
// ============================================================================

void compareAll(const std::vector<Process>& processes, int roundRobinQuantum = 2) {
    if (processes.empty()) {
        std::cout << "[ERROR] Cannot run comparison: Process list is empty.\n";
        return;
    }

    std::vector<ScheduleResult> results;
    results.push_back(runFCFS(processes));
    results.push_back(runSJF(processes));
    results.push_back(runSRTF(processes));
    results.push_back(runRoundRobin(processes, roundRobinQuantum));

    std::cout << "\n========================================================================================\n";
    std::cout << "                 ALGORITHM PERFORMANCE COMPARISON & BENCHMARK MATRIX                    \n";
    std::cout << "========================================================================================\n";

    std::cout << "\n"
              << std::left << std::setw(30) << "Algorithm"
              << std::right
              << std::setw(14) << "Avg TAT"
              << std::setw(14) << "Avg WT"
              << std::setw(14) << "Avg RT"
              << std::setw(16) << "CPU Util (%)"
              << std::setw(14) << "Throughput"
              << "\n";
    std::cout << std::string(102, '-') << "\n";

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& res : results) {
        std::string name = res.algorithmName;
        if (name.find("FCFS") != std::string::npos) name = "FCFS";
        else if (name.find("SJF - Non") != std::string::npos) name = "SJF (Non-Preemptive)";
        else if (name.find("SRTF") != std::string::npos) name = "SRTF (Preemptive)";
        else if (name.find("Round Robin") != std::string::npos) name = "Round Robin";

        std::cout << std::left << std::setw(30) << name
                  << std::right
                  << std::setw(14) << res.avgTurnaroundTime
                  << std::setw(14) << res.avgWaitingTime
                  << std::setw(14) << res.avgResponseTime
                  << std::setw(15) << res.cpuUtilization << "%"
                  << std::setw(14) << std::setprecision(4) << res.throughput
                  << std::setprecision(2)
                  << "\n";
    }
    std::cout << std::string(102, '=') << "\n";

    // Evaluation
    size_t bestTATIdx = 0, bestWTIdx = 0, bestRTIdx = 0, bestUtilIdx = 0;
    double minTAT = results[0].avgTurnaroundTime;
    double minWT = results[0].avgWaitingTime;
    double minRT = results[0].avgResponseTime;
    double maxUtil = results[0].cpuUtilization;

    for (size_t i = 1; i < results.size(); ++i) {
        if (results[i].avgTurnaroundTime < minTAT) {
            minTAT = results[i].avgTurnaroundTime;
            bestTATIdx = i;
        }
        if (results[i].avgWaitingTime < minWT) {
            minWT = results[i].avgWaitingTime;
            bestWTIdx = i;
        }
        if (results[i].avgResponseTime < minRT) {
            minRT = results[i].avgResponseTime;
            bestRTIdx = i;
        }
        if (results[i].cpuUtilization > maxUtil) {
            maxUtil = results[i].cpuUtilization;
            bestUtilIdx = i;
        }
    }

    std::cout << "\n>>> PERFORMANCE CATEGORY WINNERS <<<\n";
    std::cout << "  - Lowest Average Turnaround Time : " << results[bestTATIdx].algorithmName
              << " (" << minTAT << " units)\n";
    std::cout << "  - Lowest Average Waiting Time    : " << results[bestWTIdx].algorithmName
              << " (" << minWT << " units)\n";
    std::cout << "  - Best Response Time (Fastest)   : " << results[bestRTIdx].algorithmName
              << " (" << minRT << " units)\n";
    std::cout << "  - Peak CPU Utilization           : " << results[bestUtilIdx].algorithmName
              << " (" << maxUtil << "%)\n";

    size_t overallOptimalIdx = bestWTIdx;

    std::cout << "\n----------------------------------------------------------------------------------------\n";
    std::cout << "                         OPTIMAL SCHEDULING ALGORITHM VERDICT                           \n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "  [OPTIMAL CHOICE] : " << results[overallOptimalIdx].algorithmName << "\n\n";
    std::cout << "  Reasoning & Analysis:\n";
    std::cout << "  1. Minimizing Average Waiting Time (" << results[overallOptimalIdx].avgWaitingTime
              << " units) directly optimizes system queue delay.\n";
    std::cout << "  2. It achieved the lowest Average Turnaround Time (" << results[overallOptimalIdx].avgTurnaroundTime
              << " units), accelerating total completion.\n";

    if (results[overallOptimalIdx].algorithmName.find("SRTF") != std::string::npos) {
        std::cout << "  3. Preemptive priority for shorter remaining tasks eliminates convoy delays\n"
                  << "     caused by longer processes, maximizing responsiveness for quick tasks.\n";
    } else if (results[overallOptimalIdx].algorithmName.find("SJF") != std::string::npos) {
        std::cout << "  3. Non-preemptive shortest job selection eliminates context-switching overhead\n"
                  << "     while mathematically minimizing average waiting time for the current arrivals.\n";
    } else if (results[overallOptimalIdx].algorithmName.find("Round Robin") != std::string::npos) {
        std::cout << "  3. Time-slicing provided balanced CPU allocation without process starvation,\n"
                  << "     offering optimal fairness for this workload.\n";
    } else {
        std::cout << "  3. First Come First Serve provided straightforward execution with zero overhead\n"
                  << "     matching the arrival order effectively.\n";
    }

    if (bestRTIdx != overallOptimalIdx) {
        std::cout << "\n  Note for Interactive Systems:\n"
                  << "  * If the target environment is a responsive multi-user UI or interactive OS,\n"
                  << "    '" << results[bestRTIdx].algorithmName << "' is recommended for its lowest response time ("
                  << minRT << " units).\n";
    }
    std::cout << "----------------------------------------------------------------------------------------\n";

    std::cout << "\nDetailed Execution Breakdown:\n";
    for (const auto& res : results) {
        displayGantt(res);
    }
}

// ============================================================================
// WORKLOAD MANAGEMENT & MENU
// ============================================================================

void displayBanner() {
    std::cout << "\n========================================================================================\n";
    std::cout << "                 CPU SCHEDULING ALGORITHM SIMULATOR & COMPARATIVE SUITE                 \n";
    std::cout << "                      Implemented in C++ | Operating Systems Core                       \n";
    std::cout << "========================================================================================\n";
}

void displayMenu() {
    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "                         MAIN MENU                          \n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << "  [1] Enter Custom Process Workload\n";
    std::cout << "  [2] Load Predefined Benchmark Workloads\n";
    std::cout << "  [3] View Current Process List\n";
    std::cout << "  [4] Run First Come First Serve (FCFS)\n";
    std::cout << "  [5] Run Shortest Job First (SJF - Non-Preemptive)\n";
    std::cout << "  [6] Run Shortest Remaining Time First (SRTF - Preemptive)\n";
    std::cout << "  [7] Run Round Robin (RR)\n";
    std::cout << "  [8] Compare All 4 Algorithms & Identify Optimal Choice\n";
    std::cout << "  [9] Exit\n";
    std::cout << "------------------------------------------------------------\n";
}

void printCurrentProcesses(const std::vector<Process>& processes) {
    if (processes.empty()) {
        std::cout << "\n[!] No processes currently loaded. Please enter or load a workload first.\n";
        return;
    }
    std::cout << "\nCurrent Loaded Processes (" << processes.size() << " total):\n";
    std::cout << "------------------------------------\n";
    std::cout << std::left << std::setw(8) << "PID"
              << std::right << std::setw(14) << "Arrival Time"
              << std::setw(14) << "Burst Time" << "\n";
    std::cout << "------------------------------------\n";
    for (const auto& p : processes) {
        std::string pidStr = "P" + std::to_string(p.pid);
        std::cout << std::left << std::setw(8) << pidStr
                  << std::right << std::setw(14) << p.arrivalTime
                  << std::setw(14) << p.burstTime << "\n";
    }
    std::cout << "------------------------------------\n";
}

std::vector<Process> inputCustomProcesses() {
    std::vector<Process> processes;
    int n = getValidatedInt("Enter the number of processes (1 - 50): ", 1, 50);

    for (int i = 0; i < n; ++i) {
        int pid = i + 1;
        std::cout << "\n--- Process P" << pid << " ---\n";
        int at = getValidatedInt("  Enter Arrival Time (>= 0): ", 0, 10000);
        int bt = getValidatedInt("  Enter Burst Time (> 0): ", 1, 100000);
        processes.push_back(Process(pid, at, bt));
    }

    std::cout << "\n[SUCCESS] Successfully registered " << n << " processes.\n";
    return processes;
}

std::vector<Process> loadBenchmarkWorkload() {
    std::cout << "\nSelect a Predefined Benchmark Workload:\n";
    std::cout << "  [1] Standard Mixed Workload (4 processes, varying arrival & burst times)\n";
    std::cout << "  [2] Convoy Effect Demonstration (Long job arriving first, followed by short jobs)\n";
    std::cout << "  [3] Preemption Advantage Workload (SRTF outperforming non-preemptive SJF)\n";
    std::cout << "  [4] Workload with CPU Idle Gaps (Processes arriving with gap intervals)\n";

    int choice = getValidatedInt("Enter choice (1-4): ", 1, 4);
    std::vector<Process> processes;

    if (choice == 1) {
        processes.push_back(Process(1, 0, 8));
        processes.push_back(Process(2, 1, 4));
        processes.push_back(Process(3, 2, 9));
        processes.push_back(Process(4, 3, 5));
        std::cout << "\n[INFO] Loaded Standard Mixed Workload (P1: AT=0 BT=8, P2: AT=1 BT=4, P3: AT=2 BT=9, P4: AT=3 BT=5).\n";
    } else if (choice == 2) {
        processes.push_back(Process(1, 0, 24));
        processes.push_back(Process(2, 1, 3));
        processes.push_back(Process(3, 2, 3));
        std::cout << "\n[INFO] Loaded Convoy Effect Workload (P1: AT=0 BT=24, P2: AT=1 BT=3, P3: AT=2 BT=3).\n";
    } else if (choice == 3) {
        processes.push_back(Process(1, 0, 7));
        processes.push_back(Process(2, 2, 4));
        processes.push_back(Process(3, 4, 1));
        processes.push_back(Process(4, 5, 4));
        std::cout << "\n[INFO] Loaded Preemption Advantage Workload (P1: AT=0 BT=7, P2: AT=2 BT=4, P3: AT=4 BT=1, P4: AT=5 BT=4).\n";
    } else if (choice == 4) {
        processes.push_back(Process(1, 2, 4));
        processes.push_back(Process(2, 8, 3));
        processes.push_back(Process(3, 14, 2));
        std::cout << "\n[INFO] Loaded Idle Gap Workload (P1: AT=2 BT=4, P2: AT=8 BT=3, P3: AT=14 BT=2).\n";
    }

    return processes;
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    displayBanner();

    std::vector<Process> currentProcesses;
    currentProcesses.push_back(Process(1, 0, 8));
    currentProcesses.push_back(Process(2, 1, 4));
    currentProcesses.push_back(Process(3, 2, 9));
    currentProcesses.push_back(Process(4, 3, 5));

    std::cout << "[INFO] Initialized with default standard workload (4 processes). You can run tests directly or customize.\n";

    bool running = true;
    while (running) {
        displayMenu();
        int choice = getValidatedInt("Select an option (1 - 9): ", 1, 9);

        switch (choice) {
            case 1:
                currentProcesses = inputCustomProcesses();
                printCurrentProcesses(currentProcesses);
                break;
            case 2:
                currentProcesses = loadBenchmarkWorkload();
                printCurrentProcesses(currentProcesses);
                break;
            case 3:
                printCurrentProcesses(currentProcesses);
                break;
            case 4: {
                if (currentProcesses.empty()) {
                    std::cout << "[!] Please load or enter processes first.\n";
                    break;
                }
                ScheduleResult res = runFCFS(currentProcesses);
                displayFullReport(res);
                break;
            }
            case 5: {
                if (currentProcesses.empty()) {
                    std::cout << "[!] Please load or enter processes first.\n";
                    break;
                }
                ScheduleResult res = runSJF(currentProcesses);
                displayFullReport(res);
                break;
            }
            case 6: {
                if (currentProcesses.empty()) {
                    std::cout << "[!] Please load or enter processes first.\n";
                    break;
                }
                ScheduleResult res = runSRTF(currentProcesses);
                displayFullReport(res);
                break;
            }
            case 7: {
                if (currentProcesses.empty()) {
                    std::cout << "[!] Please load or enter processes first.\n";
                    break;
                }
                int tq = getValidatedInt("Enter Time Quantum for Round Robin (> 0): ", 1, 100000);
                ScheduleResult res = runRoundRobin(currentProcesses, tq);
                displayFullReport(res);
                break;
            }
            case 8: {
                if (currentProcesses.empty()) {
                    std::cout << "[!] Please load or enter processes first.\n";
                    break;
                }
                int tq = getValidatedInt("Enter Time Quantum for Round Robin comparison (default = 2): ", 1, 100);
                compareAll(currentProcesses, tq);
                break;
            }
            case 9:
                std::cout << "\nThank you for using the CPU Scheduling Algorithm Simulator! Exiting...\n";
                running = false;
                break;
            default:
                std::cout << "[!] Invalid option. Please select 1 - 9.\n";
                break;
        }
    }

    return 0;
}
