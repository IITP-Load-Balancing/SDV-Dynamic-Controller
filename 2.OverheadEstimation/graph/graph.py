import matplotlib.pyplot as plt
import numpy as np

# 파일에서 시간 데이터를 읽는 함수
def read_time_data(file_path):
    with open(file_path, 'r') as file:
        times = [int(line.split()[1]) for line in file.readlines()]
    return times

# 파일 경로 설정
controller_send_time_file = 'controllerSendTime.txt'
target_receive_time_file = 'targetReceiveTime.txt'

# 파일에서 시간 데이터 읽기
controller_send_times = read_time_data(controller_send_time_file)
target_receive_times = read_time_data(target_receive_time_file)

# 걸린 시간 계산 (nanoseconds)
durations_ns = [tr - cs for cs, tr in zip(controller_send_times, target_receive_times)]

# 걸린 시간을 milliseconds로 변환
durations_ms = [duration / 1e6 for duration in durations_ns]

# 최대 100개의 데이터 포인트로 제한
max_points = 100
durations_ns = durations_ns[:max_points]
durations_ms = durations_ms[:max_points]

# 걸린 시간을 nanoseconds로 그래프 그리기
plt.figure(figsize=(10, 4))
plt.plot(range(1, len(durations_ns) + 1), durations_ns, marker='o', linestyle='-', color='b')
plt.title('Signal Transmission Duration (nanoseconds)')
plt.xlabel('Signal Count')
plt.ylabel('Duration (ns)')
plt.xticks(range(1, len(durations_ns) + 1,10)) # X 축 눈금 설정
plt.grid(True)
plt.savefig("duration_ns.png")

# 걸린 시간을 milliseconds로 그래프 그리기
plt.figure(figsize=(10, 4))
plt.plot(range(1, len(durations_ms) + 1), durations_ms, marker='o', linestyle='-', color='r')
plt.title('Signal Transmission Duration (milliseconds)')
plt.xlabel('Signal Count')
plt.ylabel('Duration (ms)')
plt.xticks(range(1, len(durations_ms) + 1,10)) # X 축 눈금 설정
plt.grid(True)
plt.savefig("duration_ms.png")

# Calculate minimum, maximum, and average
min_duration_ns = np.min(durations_ns)
max_duration_ns = np.max(durations_ns)
avg_duration_ns = np.mean(durations_ns)

min_duration_ms = np.min(durations_ms)
max_duration_ms = np.max(durations_ms)
avg_duration_ms = np.mean(durations_ms)

# Print the table
print(f"{'Metric':<10}{'Nanoseconds':<20}{'Milliseconds':<20}")
print(f"{'-'*10}{'-'*20}{'-'*20}")
print(f"{'Minimum':<10}{min_duration_ns:<20}{min_duration_ms:<20.2f}")
print(f"{'Maximum':<10}{max_duration_ns:<20}{max_duration_ms:<20.2f}")
print(f"{'Average':<10}{avg_duration_ns:<20.2f}{avg_duration_ms:<20.2f}")


