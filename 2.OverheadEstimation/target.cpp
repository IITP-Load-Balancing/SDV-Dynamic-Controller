#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <pthread.h>
#include <atomic>
#include <cstring>
#include <chrono>

using namespace std;

atomic<bool> run(true);
atomic<bool> continueRunning(false);

void writeTimeToFile(const string& nodeName, int cnt) {
    // nodeName이 Target10일 때만 파일에 시간 기록
    if (nodeName == "Target9") {
        auto now = chrono::steady_clock::now();
        auto now_ns = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
        
        ofstream out("Target10ReceiveTime.txt", ios::app);
        out << cnt << " " << now_ns << endl;
        out.close();
    }
}

void updatePid(const string& nodeName) {
    ofstream out("pid.txt", ios::app);
    out << nodeName << " " << getpid() << endl;
    out.close();
}

void readDefaultStatus(const string& nodeName) {
    ifstream in("default.txt");
    string line;
    while (getline(in, line)) {
        if (line.find(nodeName) == 0) {
            continueRunning.store(line.back() == '1');
            break;
        }
    }
    in.close();
}

void* signalHandler(void* arg) {
    sigset_t set;
    int sig, s;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    int count = 0; // 시그널을 받은 횟수를 세기 위한 카운터

    while (run.load()) {
        s = sigwait(&set, &sig);
        if (s == 0) {
            count++; // 시그널 카운트 증가
            if (sig == SIGUSR1) {
                cout << "SIGUSR1 received. Continue running." << endl;
                continueRunning.store(true);
            } else if (sig == SIGUSR2) {
                cout << "SIGUSR2 received. Pause running." << endl;
                continueRunning.store(false);
            }
            // 시간 기록
            writeTimeToFile(static_cast<string*>(arg)->data(), count);
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " nodeName" << endl;
        return 1;
    }
    string nodeName = argv[1];

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_t tid;
    pthread_create(&tid, NULL, signalHandler, new string(nodeName)); // nodeName을 스레드에 전달

    readDefaultStatus(nodeName);
    updatePid(nodeName);
    int cnt = 0;
    while (run.load()) {
        if (continueRunning) {
            cnt++;
            cout << nodeName << " is running...[" << cnt << "]" << endl;
        } else {
            cout << nodeName << " is paused.[" << cnt << "]" << endl;
        }
        sleep(1);
    }

    pthread_join(tid, NULL);
    return 0;
}
