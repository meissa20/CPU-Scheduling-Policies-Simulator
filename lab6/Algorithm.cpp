#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>

using namespace std;

int absoluteValue(int num)
{
    return (num < 0) ? -num : num;
}

struct Process
{
    char name;
    int arrivalTime;
    int serviceTime;
    int beginTime;
    int endTime;
    int waitingTime;
    int remainingTime;
    int level;
    int priority;
    int arrivalToQ;
    bool completed;
    bool initialized;
    string traceStr;
};

struct CompareProcess
{
    bool operator()(const Process *p1, const Process *p2) const
    {
        if (p1->priority == p2->priority)
        {
            return p1->arrivalToQ > p2->arrivalToQ; // Earlier arrivalTime has higher priority
        }
        return p1->priority < p2->priority; // Higher priority (lower numerical value) is preferred
    }
};

bool isProcessInNormalQueue(queue<Process *> q, const Process &process);
void fcfs(int lastInst, int N, vector<Process> &processes, char str[10]);
void rr(int lastInst, int N, vector<Process> &processes, char str[10], int q);
void spn(int lastInst, int N, vector<Process> &processes, char str[10]);
void srt(int lastInst, int N, vector<Process> &processes, char str[10]);
void HRRN(int lastInst, int N, vector<Process> &processes, char str[10]);
void FB_1(int lastInst, int N, vector<Process> &processes, char str[10]);
void FB_2i(int lastInst, int N, vector<Process> &processes, char str[10]);
void print_stats(int N, vector<Process> &processes);
void print_trace(int lastInst, vector<Process> &processes, int N, vector<vector<int>> interrupts, vector<int> indicies);
bool isProcessInQueue(std::priority_queue<Process *, std::vector<Process *>, CompareProcess> pq, const Process &process);
void Aging(int lastInst, int N, vector<Process> &processes, char str[10], int q);
void check(vector<Process> &processes, Process p, vector<vector<int>> &interrupts, int currenttime, int execTime, vector<int> &indicies);

void initialization(vector<Process> &processes, int N)
{
    for (int i = 0; i < N; i++)
    {
        processes[i].completed = false;
        processes[i].initialized = false;
        processes[i].priority = processes[i].serviceTime;
        processes[i].remainingTime = processes[i].serviceTime;
        processes[i].level = 0;
        processes[i].beginTime = -1;
        processes[i].endTime = -1;
    }
}
int main()
{
    char str[10];
    string policystr;
    int policyint, lastInst, q, N;

    cin >> str;
    cin >> policystr >> lastInst >> N;

    vector<Process> processes(N);

    for (int i = 0; i < N; i++)
    {
        cin >> processes[i].name;
        cin.ignore();
        cin >> processes[i].arrivalTime;
        cin.ignore();
        cin >> processes[i].serviceTime;
    }
    string segment;
    stringstream ss(policystr);
    while (getline(ss, segment, ','))
    {
        stringstream segmentStream(segment);
        int policyint = 0;
        int q = 0; // Default value for q if it's not present

        // Parse the segment by ','
        stringstream segmentStreamComma(segment);
        vector<int> values;
        string temp;

        while (getline(segmentStreamComma, temp, '-'))
        {
            values.push_back(stoi(temp));
        }

        
        if (values.size() == 1)
        {
            policyint = values[0];
            q = 0;
        }
        else if (values.size() == 2)
        {
            policyint = values[0];
            q = absoluteValue(values[1]);
        }

        
        if (policyint == 1)
        {
            initialization(processes, N);
            fcfs(lastInst, N, processes, str);
        }
        else if (policyint == 2)
        {
            initialization(processes, N);
            rr(lastInst, N, processes, str, q);
        }
        else if (policyint == 3)
        {
            initialization(processes, N);
            spn(lastInst, N, processes, str);
        }
        else if (policyint == 4)
        {
            initialization(processes, N);
            srt(lastInst, N, processes, str);
        }
        else if (policyint == 5)
        {
            initialization(processes, N);
            HRRN(lastInst, N, processes, str);
        }
        else if (policyint == 6)
        {
            initialization(processes, N);
            FB_1(lastInst, N, processes, str);
        }
        else if (policyint == 7)
        {
            initialization(processes, N);
            FB_2i(lastInst, N, processes, str);
        }
        else if (policyint == 8)
        {
            initialization(processes, N);
            Aging(lastInst, N, processes, str, q);
        }
        else
        {
            cout << "Invalid policy: " << policyint << "\n";
        }
    }

    return 0;
}

void fcfs(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }
    int currentTime = 0;
    while (currentTime <= lastInst)
    {
        int fc = -1;
        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                if (fc == -1 || processes[i].arrivalTime < processes[fc].arrivalTime)
                {
                    fc = i;
                }
            }
        }

        if (fc != -1)
        {
            processes[fc].beginTime = currentTime;
            currentTime += processes[fc].serviceTime;
            processes[fc].endTime = currentTime;
            processes[fc].completed = true;
        }
        else
        {
            currentTime++;
        }
    }
    if (strcmp("trace", str) == 0)
    {
        cout << "FCFS  ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "FCFS";
        print_stats(N, processes);
    }
}

void checkingRr(queue<Process *> &checking, int N, vector<Process> &processes, int currentTime, int order)
{
    for (int i = 0; i < N; i++)
    {

        if (!processes[i].completed && processes[i].arrivalTime <= currentTime && !isProcessInNormalQueue(checking, processes[i]))
        {
            processes[i].initialized = true;
            checking.push(&processes[i]);
        }
    }
}

void rr(int lastInst, int N, vector<Process> &processes, char str[10], int q)
{
    int order = 0;
    queue<Process *> normalQ;
    queue<Process *> checking;
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }

    int currentTime = 0;
    bool first = true;
    while (currentTime < lastInst)
    {
        for (int i = 0; i < N; i++)
        {
            
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime && !isProcessInNormalQueue(normalQ, processes[i]))
            {
                processes[i].initialized = true;
                processes[i].arrivalToQ = order++;
                normalQ.push(&processes[i]);
            }
        }

        bool processExecuted = false;
        if (!normalQ.empty())
        {
            Process *runningProcess = normalQ.front();
            if (runningProcess->remainingTime == runningProcess->serviceTime)
            {
                runningProcess->beginTime = currentTime;
            }

            int executionTime = min(q, runningProcess->remainingTime);
    
            check(processes, *runningProcess, interrupts, currentTime, executionTime, indicies);
            currentTime += executionTime;
            
            runningProcess->remainingTime -= executionTime;
            checking = normalQ;
            if (runningProcess->remainingTime == 0)
            {
                runningProcess->endTime = currentTime;
                runningProcess->completed = true;
            }
            checkingRr(checking, N, processes, currentTime, order);
            checking.pop();
            if(!runningProcess->completed)
                checking.push(normalQ.front());
            normalQ = checking;
        }

        else
        {
            currentTime++;
        }
    }

    if (strcmp("trace", str) == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            indicies[i] = 0;
        }
        printf("RR-%-3d", q);
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        printf("RR-%d", q);
        print_stats(N, processes);
    }
}

void spn(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }
    int currentTime = 0;
    while (currentTime <= lastInst)
    {
        int shortestIdx = -1;
        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                if (shortestIdx == -1 || processes[i].serviceTime < processes[shortestIdx].serviceTime)
                {
                    shortestIdx = i;
                }
            }
        }

        if (shortestIdx != -1)
        {
            processes[shortestIdx].beginTime = currentTime;
            currentTime += processes[shortestIdx].serviceTime;
            processes[shortestIdx].endTime = currentTime;
            processes[shortestIdx].completed = true;
        }
        else
        {
            currentTime++;
        }
    }
    if (strcmp("trace", str) == 0)
    {
        cout << "SPN   ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "SPN";
        print_stats(N, processes);
    }
}

void srt(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }
    int currentTime = 0;
    while (currentTime <= lastInst)
    {
        int shortestIdx = -1;
        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                if (shortestIdx == -1 || processes[i].remainingTime < processes[shortestIdx].remainingTime)
                {
                    processes[shortestIdx].initialized = true;
                    shortestIdx = i;
                }
            }
        }

        if (shortestIdx != -1)
        {
            if (processes[shortestIdx].remainingTime == processes[shortestIdx].serviceTime)
                processes[shortestIdx].beginTime = currentTime;
            check(processes, processes[shortestIdx], interrupts, currentTime, 1, indicies);
            processes[shortestIdx].remainingTime--;
            currentTime++;
            if (processes[shortestIdx].remainingTime == 0)
            {
                processes[shortestIdx].endTime = currentTime;
                processes[shortestIdx].completed = true;
            }
        }
        else
        {
            currentTime++;
        }
    }
    if (strcmp("trace", str) == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            indicies[i] = 0;
        }
        cout << "SRT   ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "SRT";
        print_stats(N, processes);
    }
}

void HRRN(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }
    int currentTime = 0;
    while (currentTime <= lastInst)
    {
        int longestRatioidx = -1;
        float currlongestRatio;
        float idxlongestRatio;
        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                if (longestRatioidx != -1)
                {
                    processes[i].waitingTime = currentTime - processes[i].arrivalTime;
                    processes[longestRatioidx].waitingTime = currentTime - processes[longestRatioidx].arrivalTime;
                    currlongestRatio = (processes[i].waitingTime + processes[i].serviceTime) / processes[i].serviceTime;
                    idxlongestRatio = (processes[longestRatioidx].waitingTime + processes[longestRatioidx].serviceTime) / processes[longestRatioidx].serviceTime;
                }
                if (longestRatioidx == -1 || currlongestRatio >= idxlongestRatio)
                {
                    longestRatioidx = i;
                }
            }
        }

        if (longestRatioidx != -1)
        {
            processes[longestRatioidx].beginTime = currentTime;
            currentTime += processes[longestRatioidx].serviceTime;
            processes[longestRatioidx].endTime = currentTime;
            processes[longestRatioidx].completed = true;
        }
        else
        {
            currentTime++;
        }
    }
    if (strcmp("trace", str) == 0)
    {
        cout << "HRRN  ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "HRRN";
        print_stats(N, processes);
    }
}

//________________________________________________________________
//________________________________________________________________
//________________________________________________________________
//________________________________________________________________
//________________________________________________________________
//________________________________________________________________

void check(vector<Process> &processes, Process p, vector<vector<int>> &interrupts, int currenttime, int execTime, vector<int> &indicies)
{
    int m = currenttime;
    for (int i = 0; i < processes.size(); ++i)
    {
        currenttime = m;
        if (processes[i].name != p.name && processes[i].initialized && !processes[i].completed)
        {
            for (int j = 0; j < execTime; ++j)
            {
                interrupts[i][indicies[i]] = currenttime;
                indicies[i]++;
                currenttime++;
            }
        }
    }
    return;
}

bool onlyOneProcess(int N, const vector<Process> &processes, int currentTime)
{
    int c = 0;
    for (int i = 0; i < N; i++)
    {
        if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
        {
            c++;
        }
    }
    return c <= 1;
}

void FB_1(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<queue<Process *>> queues(lastInst);
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    vector<int> time_quantum;
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
        time_quantum.push_back(pow(2, i));
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }

    int currentTime = 0;

    while (currentTime <= lastInst)
    {
        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                processes[i].initialized = true;
                queues[processes[i].level].push(&processes[i]);
            }
        }
        bool processExecuted = false;
        for (int i = 0; i < lastInst; i++)
        {
            if (!queues[i].empty())
            {
                Process *runningProcess = queues[i].front();
                if (runningProcess->remainingTime == runningProcess->serviceTime)
                {
                    runningProcess->beginTime = currentTime; // Record first execution time
                }
                check(processes, *runningProcess, interrupts, currentTime, 1, indicies);
                if (!onlyOneProcess(N, processes, currentTime + 1))
                {
                    runningProcess->level++;
                }

                currentTime++;
                runningProcess->remainingTime--;
                if (runningProcess->remainingTime == 0)
                {
                    runningProcess->endTime = currentTime;
                    runningProcess->completed = true;
                }
                while (!queues[i].empty())
                {
                    queues[i].pop();
                }

                processExecuted = true;
                break;
            }
        }

        if (!processExecuted)
        {
            currentTime++;
        }
    }

    // Calculate and output results
    if (strcmp("trace", str) == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            indicies[i] = 0;
        }
        cout << "FB-1  ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "FB-1";
        print_stats(N, processes);
    }
}

void FB_2i(int lastInst, int N, vector<Process> &processes, char str[10])
{
    vector<queue<Process *>> queues(N);
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    vector<int> time_quantum;
    for (int i = 0; i < N; ++i)
    {
        indicies[i] = 0;
        time_quantum.push_back(pow(2, i));
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }

    int currentTime = 0;

    while (currentTime <= lastInst)
    {

        for (int i = 0; i < N; i++)
        {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime)
            {
                processes[i].initialized = true;
                queues[processes[i].level].push(&processes[i]);
            }
        }
        bool processExecuted = false;
        for (int i = 0; i < N; i++)
        {
            if (!queues[i].empty())
            {
                Process *runningProcess = queues[i].front();
                if (runningProcess->remainingTime == runningProcess->serviceTime)
                {
                    runningProcess->beginTime = currentTime; // Record first execution time
                }

                int executionTime = min(time_quantum[i], runningProcess->remainingTime);
                check(processes, *runningProcess, interrupts, currentTime, executionTime, indicies);
                if (!onlyOneProcess(N, processes, currentTime + 1))
                {
                    runningProcess->level++;
                }

                currentTime += executionTime;
                runningProcess->remainingTime -= executionTime;
                if (runningProcess->remainingTime == 0)
                {
                    runningProcess->endTime = currentTime;
                    runningProcess->completed = true;
                }
                while (!queues[i].empty())
                {
                    queues[i].pop();
                }

                processExecuted = true;
                break;
            }
        }

        if (!processExecuted)
        {
            currentTime++;
        }
    }

    if (strcmp("trace", str) == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            indicies[i] = 0;
        }
        cout << "FB-2i ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "FB-2i";
        print_stats(N, processes);
    }
}

bool isProcessInQueue(priority_queue<Process *, vector<Process *>, CompareProcess> pq, const Process &process)
{
    while (!pq.empty())
    {
        Process *current = pq.top();
        if (current->name == process.name)
        {
            return true;
        }
        pq.pop();
    }
    return false;
}

bool isProcessInNormalQueue(queue<Process *> q, const Process &process)
{
    while (!q.empty())
    {
        Process *current = q.front();
        if (current->name == process.name)
        {
            return true;
        }
        q.pop();
    }
    return false;
}

//_________________________________________________________
//_________________________________________________________
//_________________________________________________________
//_________________________________________________________
//_________________________________________________________
//_________________________________________________________
//_________________________________________________________

void Aging(int lastInst, int N, vector<Process> &processes, char str[10], int q)
{
    int order = 0;
    priority_queue<Process *, vector<Process *>, CompareProcess> pq;
    vector<vector<int>> interrupts(N, vector<int>(lastInst));
    vector<int> indicies(N);
    vector<int> time_quantum;
    for (int i = 0; i < N; ++i)
    {
        processes[i].endTime = lastInst;
        indicies[i] = 0;
    }
    for (int i = 0; i < interrupts.size(); ++i)
    {
        for (int j = 0; j < interrupts[i].size(); ++j)
        {
            interrupts[i][j] = -1;
        }
    }

    int currentTime = 0;

    while (currentTime < lastInst)
    {
        for (int i = 0; i < N; i++)
        {

            if (processes[i].arrivalTime <= currentTime && !isProcessInQueue(pq, processes[i]))
            {
                if (!processes[i].initialized)
                    processes[i].priority++;
                processes[i].initialized = true;
                processes[i].arrivalToQ = order++;
                pq.push(&processes[i]);
            }
        }

        bool processExecuted = false;
        if (!pq.empty())
        {
            Process *runningProcess = pq.top();
            if (runningProcess->remainingTime == runningProcess->serviceTime)
            {
                runningProcess->beginTime = currentTime;
            }

            int executionTime = min(q, runningProcess->remainingTime);
            check(processes, *runningProcess, interrupts, currentTime, executionTime, indicies);

            currentTime += executionTime;
            runningProcess->remainingTime = runningProcess->serviceTime * runningProcess->serviceTime;
            runningProcess->priority = runningProcess->serviceTime;
            pq.pop();
            priority_queue<Process *, vector<Process *>, CompareProcess> temp = pq;
            while (!temp.empty())
            {
                temp.top()->priority++;
                temp.pop();
            }
        }

        else
        {
            currentTime++;
        }
    }

    if (strcmp("trace", str) == 0)
    {
        
        for (int i = 0; i < N; ++i)
        {
            indicies[i] = 0;
        }
        cout << "Aging ";
        print_trace(lastInst, processes, N, interrupts, indicies);
    }
    else
    {
        cout << "Aging";
        print_stats(N, processes);
    }
}

//___________________________________________________________________
//___________________________________________________________________
//___________________________________________________________________
//___________________________________________________________________
//___________________________________________________________________
//___________________________________________________________________

void print_trace(int lastInst, vector<Process> &processes, int N, vector<vector<int>> interrupts, vector<int> indicies)
{
    for (int i = 0; i <= lastInst; i++)
    {
        cout << i % 10 << " ";
    }
    cout << endl
         << "------";
    for (int i = 0; i <= lastInst; i++)
    {
        cout << "--";
    }
    cout << endl;
    for (int j = 0; j < N; j++)
    {
        printf("%-6c", processes[j].name);
        for (int i = 0; i <= lastInst; i++)
        {
            // printf("inte %d i %d ",interrupts[j][indicies[j]],i);
            // if (i >= processes[j].beginTime && i < processes[j].endTime && i != interrupts[j][indicies[j]])
            // {

            //     cout << "";
            // }
            if (i >= processes[j].beginTime && i < processes[j].endTime && i != interrupts[j][indicies[j]])
            {

                cout << "|*";
            }
            else if (i == interrupts[j][indicies[j]])
            {
                indicies[j]++;
                cout << "|.";
            }
            else if (i < processes[j].beginTime && i >= processes[j].arrivalTime)
            {
                cout << "|.";
            }
            else
            {
                cout << "| ";
            }
        }
        cout << endl;
    }
    cout << "------------------------------------------------\n"
         << endl;
}
void print_stats(int N, vector<Process> &processes)
{
    cout << endl;
    cout << "Process    ";
    for (const auto &process : processes)
    {
        printf("|  %c  ", process.name);
    }
    cout << "|" << endl
         << "Arrival    ";
    for (const auto &process : processes)
    {
        printf("|%3d  ", process.arrivalTime);
    }
    cout << "|" << endl
         << "Service    ";
    for (const auto &process : processes)
    {
        printf("|%3d  ", process.serviceTime);
    }
    cout << "| Mean|" << endl
         << "Finish     ";

    for (const auto &process : processes)
    {
        printf("|%3d  ", process.endTime);
    }
    cout << "|-----|" << endl
         << "Turnaround ";
    float sum = 0;
    int Turnaround;
    float Normturn;
    for (const auto &process : processes)
    {
        Turnaround = process.endTime - process.arrivalTime;
        printf("|%3d  ", Turnaround);
        sum += Turnaround;
    }
    float mean = sum / N;
    printf("|%5.2f|", mean);
    cout << endl
         << "NormTurn   ";
    sum = 0;
    for (const auto &process : processes)
    {
        Turnaround = process.endTime - process.arrivalTime;
        Normturn = float(Turnaround) / process.serviceTime;
        printf("|%5.2f", Normturn);
        sum += Normturn;
    }
    mean = sum / N;
    printf("|%5.2f|", mean);
    cout << "\n"
         << endl;
}