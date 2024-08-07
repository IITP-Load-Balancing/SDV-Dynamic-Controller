#include <iostream>
#include <fstream>
#include <csignal>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring> // For memset()


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

map<string, int> readServerConfig() {
    ifstream def("server_default.txt");
    map<string, int> config;
    string name;
    int status;

    // 기본 상태를 읽기
    while (def >> name >> status) {
        config[name] = status;
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
    auto server_config = readServerConfig(); // just for future

    // Sub Controller에 연결합니다.
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "10.150.5.212", &serv_addr.sin_addr) <= 0) { //TODO: change to your IP!
        cout << "Invalid address / Address not supported" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    while (true) {
        

        for (const auto& [name, props] : config) {
            if (name == "Target1") {
                // Target1에는 SIGUSR1과 SIGUSR2를 랜덤하게 보냄
                int randSignal = rand() % 2 == 0 ? SIGUSR1 : SIGUSR2;
                sendSignal(name, props.second, randSignal);
                // Target1(Server)에게 반대 신호
                string command;
                if (randSignal == SIGUSR2) command = "SIGUSR1_Target1";
                else command = "SIGUSR2_Target1";
                send(sock, command.c_str(), command.size(), 0);
                cout << "Command sent to Sub Controller: " << command << endl;

            } else if (name == "Target2") {
                // Target2에는 SIGUSR1과 SIGUSR2를 랜덤하게 보냄
                int randSignal = rand() % 2 == 0 ? SIGUSR1 : SIGUSR2;
                sendSignal(name, props.second, randSignal);
                // Target2(Server)에게 반대 신호
                string command;
                if (randSignal == SIGUSR2) command = "SIGUSR1_Target2";
                else command = "SIGUSR2_Target2";
                send(sock, command.c_str(), command.size(), 0);
                cout << "Command sent to Sub Controller: " << command << endl;
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    close(sock);
    return 0;
}
