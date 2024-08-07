#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

// 구성 정보를 읽어오는 함수
map<string, pair<int, pid_t>> readConfig() {
    ifstream def("default.txt");
    map<string, pair<int, pid_t>> config;
    string name;
    int status;

    // 기본 상태를 읽기
    while (def >> name >> status) {
        config[name].first = status;
    }

    ifstream pids("pid.txt");
    pid_t pid;
    // PID 읽기
    while (pids >> name >> pid) {
        if (config.find(name) != config.end()) {
            config[name].second = pid;
        }
    }

    return config;
}

// 각 target에 대한 시그널 보내기
void sendSignal(const string& targetName, pid_t pid, int signal, int count, int cnt) {
    if (pid > 0) {
        cout << "Sending " << (signal == SIGUSR1 ? "SIGUSR1" : "SIGUSR2") << " to " << targetName << " (PID: " << pid << ")" << endl;
        kill(pid, signal);

        // 첫 번째 target에 대한 신호만 시간 기록
        if (cnt % 10 == 1) {
            auto now = chrono::steady_clock::now();
            auto now_ns = chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count();
            
            ofstream out("controllerSendTime.txt", ios::app);
            out << count << " " << now_ns << endl;
            out.close();
        }
    }
}

int main() {
    srand(time(NULL));
    auto config = readConfig();

    int count = 0; // 시그널을 보낸 횟수를 세기 위한 카운터
    int cnt = 0;

    while (true) {
        count++; // 시그널 카운트 증가
        for (const auto& [name, props] : config) {
            cnt++;
            int randSignal = rand() % 2 == 0 ? SIGUSR1 : SIGUSR2;
            sendSignal(name, props.second, randSignal, count, cnt);
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
