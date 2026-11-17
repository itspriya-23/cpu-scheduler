#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <string>

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;

    Process(int id, int at, int bt)
        : pid(id), arrivalTime(at), burstTime(bt),
          startTime(-1), completionTime(0), turnaroundTime(0), waitingTime(0), responseTime(0) {}
};

struct GanttSegment {
    int pid; // -1 for Idle
    int startTime;
    int endTime;

    GanttSegment(int id, int s, int e) : pid(id), startTime(s), endTime(e) {}
};

int main() {
    std::cout << "========================================================\n";
    std::cout << "          First Come First Serve (FCFS) Scheduling      \n";
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

    // Sort by arrival time primarily, break ties by PID
    std::vector<Process> execList = processes;
    std::sort(execList.begin(), execList.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime)
            return a.arrivalTime < b.arrivalTime;
        return a.pid < b.pid;
    });

    std::vector<GanttSegment> gantt;
    int currentTime = 0;

    for (auto& p : execList) {
        if (currentTime < p.arrivalTime) {
            gantt.push_back(GanttSegment(-1, currentTime, p.arrivalTime));
            currentTime = p.arrivalTime;
        }

        p.startTime = currentTime;
        currentTime += p.burstTime;
        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.responseTime = p.startTime - p.arrivalTime;

        gantt.push_back(GanttSegment(p.pid, p.startTime, p.completionTime));
    }

    // Sort back by PID for display
    std::sort(execList.begin(), execList.end(), [](const Process& a, const Process& b) {
        return a.pid < b.pid;
    });

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
    for (const auto& p : execList) {
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
    std::cout << "\nGantt Chart:\nTimeline: " << gantt.front().startTime;
    for (const auto& seg : gantt) {
        if (seg.pid == -1)
            std::cout << " -- [IDLE] -- " << seg.endTime;
        else
            std::cout << " -- P" << seg.pid << " -- " << seg.endTime;
    }
    std::cout << "\n\n";

    // Metrics
    double avgTAT = static_cast<double>(totalTAT) / n;
    double avgWT = static_cast<double>(totalWT) / n;
    int totalScheduleTime = gantt.back().endTime;
    double cpuUtil = (totalScheduleTime > 0) ? (static_cast<double>(totalBT) / totalScheduleTime * 100.0) : 0.0;
    double throughput = (totalScheduleTime > 0) ? (static_cast<double>(n) / totalScheduleTime) : 0.0;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time : " << avgTAT << " units\n";
    std::cout << "Average Waiting Time    : " << avgWT << " units\n";
    std::cout << "CPU Utilization         : " << cpuUtil << " %\n";
    std::cout << "Throughput              : " << std::setprecision(4) << throughput << " processes/unit time\n";

    return 0;
}
