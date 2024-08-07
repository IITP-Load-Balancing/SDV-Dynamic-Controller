#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <thread>

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
void sendSignal(const string& targetName, pid_t pid, int signal) {
    if (pid > 0) {
        cout << "Sending " << (signal == SIGUSR1 ? "SIGUSR1" : "SIGUSR2") << " to " << targetName << " (PID: " << pid << ")" << endl;
        kill(pid, signal);
    }
}

int main() {
    srand(time(NULL));
    auto config = readConfig();
    while (true) {

        for (const auto& [name, props] : config) {
            if (name == "Target1") {
                // Target1에는 SIGUSR1만 보내 계속 실행하게 함
                sendSignal(name, props.second, SIGUSR1);
            } else if (name == "Target2") {
                // Target2에는 SIGUSR1과 SIGUSR2를 랜덤하게 보냄
                int randSignal = rand() % 2 == 0 ? SIGUSR1 : SIGUSR2;
                sendSignal(name, props.second, randSignal);
            } else if (name == "Target3") {
                // Target3에는 SIGUSR2만 보내 계속 멈추게 함
                sendSignal(name, props.second, SIGUSR2);
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
