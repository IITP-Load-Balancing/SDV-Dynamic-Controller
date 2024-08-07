# SDV Dynamic Computation Offloading Controller

## About
Implementation of Dynamic Computation Offloading Control Logic in a Software-Defined Vehicle (SDV) System

## How to Adapt to Your System
In the examples' Target.cpp code, ensure to utilize the following functions:
1. readDefaultStatus
2. updatePid
3. signalHandler


Additionally, replace "Target" with "YourApp". In default.txt, the user can decide whether to execute these functions at the initial start.

## LocalOnlyController
![그림1](https://github.com/user-attachments/assets/13f64857-9560-4e74-b9c8-2b03eafbaca1)

### Feature: Low Latency using Signal Processing
![그림2](https://github.com/user-attachments/assets/fb7c5a9e-02ec-4cbf-9643-d5dcb9e4cb6d)

## LocalServerDynamicController
![그림3](https://github.com/user-attachments/assets/9a2bf790-307d-464b-a711-62de95700c9c)
