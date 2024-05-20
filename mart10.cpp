﻿#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/stat.h>

#include <pthread.h>
#include <filesystem>
#include <chrono>
#include <future>
#include <math.h>
#include <conio.h>
#include <signal.h>

using namespace std;
mutex printM;

void print(int philosopher_number, int n) {
    //const std::lock_guard<std::mutex> lock(printM);
    printM.lock();
    if (n == 1) {
        cout << "Филосов " << philosopher_number << " думает.." << endl;
    }
    else if (n == 2)
    {
        cout << "Филосов " << philosopher_number << " закончил есть и думает.." << endl;
    }
    else {
        cout << "Филосов " << philosopher_number << " начал есть.." << endl;
    }
    printM.unlock();
}



int main()
{
    setlocale(LC_ALL, "Russian");



    const int number_of_philosophers = 5;


    struct Forks
    {
    public:
        Forks() { ; }
        mutex mu;
    };




    auto eat = [](Forks& left_fork, Forks& right_fork, int philosopher_number) {

        unique_lock< mutex> llock(left_fork.mu);
        unique_lock< mutex> rlock(right_fork.mu);

        print(philosopher_number, 3);

        chrono::milliseconds timeout(300);
        this_thread::sleep_for(timeout);

        print(philosopher_number, 2);


    };


    Forks forks[number_of_philosophers];
    thread philosopher[number_of_philosophers];

    for (int i = 0; i < number_of_philosophers; ++i) {
        auto philosopher_number = i + 1;
        print(philosopher_number, 1);
        auto previous_fork_index = (number_of_philosophers + (i - 1)) % number_of_philosophers;
        philosopher[i] = thread(eat, ref(forks[i]), ref(forks[previous_fork_index]), philosopher_number);

    }
    cout << endl;
    for (auto& ph : philosopher) {
        ph.join();
    }


    return 0;
}
