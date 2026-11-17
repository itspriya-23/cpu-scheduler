#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <string>

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;

    Process(int id, int at, int bt)
        : pid(id), arrivalTime(at), burstTime(bt), remainingTime(bt),
          startTime(-1), completionTime(0), turnaroundTime(0), waitingTime(0), responseTime(0) {}
};

struct GanttSegment {
    int pid; // -1 for Idle
    int startTime;
    int endTime;

    GanttSegment(int id, int s, int e) : pid(id), startTime(s), endTime(e) {}
};

void consolidateGantt(std::vector<GanttSegment>& gantt) {
    if (gantt.empty()) return;
    std::vector<GanttSegment> merged;
    merged.push_back(gantt[0]);
    for (size_t i = 1; i < gantt.size(); ++i) {
        if (gantt[i].pid == merged.back().pid && gantt[i].startTime == merged.back().endTime) {
            merged.back().endTime = gantt[i].endTime;
        } else if (gantt[i].startTime < gantt[i].endTime) {
            merged.push_back(gantt[i]);
        }
    }
    gantt = merged;
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "            Round Robin (RR) CPU Scheduling             \n";
    std::cout << "========================================================\n";

    int n;
    std::cout << "Enter the number of processes: ";
    if (!(std::cin >> n) || n <= 0) {
        std::cout << "Invalid number of processes.\n";
        return 1;
    }

    std::vector<Process> processes;
    for (int i = 0; i < n; ++i) {
        int at, bt;
        std::cout << "\nProcess P" << (i + 1) << ":\n";
        std::cout << "  Enter Arrival Time: ";
        std::cin >> at;
        std::cout << "  Enter Burst Time: ";
        std::cin >> bt;
        processes.push_back(Process(i + 1, at, bt));
    }

    int timeQuantum;
    std::cout << "\nEnter Time Quantum (> 0): ";
    std::cin >> timeQuantum;
    if (timeQuantum <= 0) timeQuantum = 1;

    std::vector<int> sortedIndices(n);
    for (int i = 0; i < n; ++i) sortedIndices[i] = i;
    std::sort(sortedIndices.begin(), sortedIndices.end(), [&](int a, int b) {
        if (processes[a].arrivalTime != processes[b].arrivalTime)
            return processes[a].arrivalTime < processes[b].arrivalTime;
        return processes[a].pid < processes[b].pid;
    });

    std::queue<int> readyQueue;
    std::vector<bool> inQueueOrDone(n, false);
    std::vector<GanttSegment> rawGantt;

    int currentTime = 0;
    int completedCount = 0;

    int firstArrival = processes[sortedIndices[0]].arrivalTime;
    if (firstArrival > currentTime) {
        rawGantt.push_back(GanttSegment(-1, currentTime, firstArrival));
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
                rawGantt.push_back(GanttSegment(-1, currentTime, nextArrival));
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

        rawGantt.push_back(GanttSegment(p.pid, start, currentTime));

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

    consolidateGantt(rawGantt);

    // Display Table
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

    int totalTAT = 0, totalWT = 0, totalBT = 0;
    for (const auto& p : processes) {
        std::string pidStr = "P" + std::to_string(p.pid);
        std::cout << std::left << std::setw(8) << pidStr
                  << std::right << std::setw(14) << p.arrivalTime
                  << std::setw(14) << p.burstTime
                  << std::setw(18) << p.completionTime
                  << std::setw(18) << p.turnaroundTime
                  << std::setw(16) << p.waitingTime
                  << std::setw(16) << p.responseTime
                  << "\n";
        totalTAT += p.turnaroundTime;
        totalWT += p.waitingTime;
        totalBT += p.burstTime;
    }
    std::cout << "--------------------------------------------------------------------------------------------------------\n";

    // Gantt Chart
    std::cout << "\nGantt Chart:\nTimeline: " << rawGantt.front().startTime;
    for (const auto& seg : rawGantt) {
        if (seg.pid == -1)
            std::cout << " -- [IDLE] -- " << seg.endTime;
        else
            std::cout << " -- P" << seg.pid << " -- " << seg.endTime;
    }
    std::cout << "\n\n";

    // Metrics
    double avgTAT = static_cast<double>(totalTAT) / n;
    double avgWT = static_cast<double>(totalWT) / n;
    int totalScheduleTime = rawGantt.back().endTime;
    double cpuUtil = (totalScheduleTime > 0) ? (static_cast<double>(totalBT) / totalScheduleTime * 100.0) : 0.0;
    double throughput = (totalScheduleTime > 0) ? (static_cast<double>(n) / totalScheduleTime) : 0.0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time : " << avgTAT << " units\n";
    std::cout << "Average Waiting Time    : " << avgWT << " units\n";
    std::cout << "CPU Utilization         : " << cpuUtil << " %\n";
    std::cout << "Throughput              : " << std::setprecision(4) << throughput << " processes/unit time\n";

    return 0;
}
