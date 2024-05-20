#include <iostream>              // Подключаем библиотеку для работы с вводом-выводом
#include <vector>                // Подключаем библиотеку для работы с векторами
#include <iomanip>               // Подключаем библиотеку для работы с форматированием вывода
#include <limits>                // Подключаем библиотеку для работы с пределами типов данных
#include <string>                // Подключаем библиотеку для работы со строками
#include <thread>                // Подключаем библиотеку для работы с потоками
#include <mutex>                 // Подключаем библиотеку для работы с мьютексами (замками)
#include <fstream>               // Подключаем библиотеку для работы с файлами
#include <sys/stat.h>            // Подключаем библиотеку для работы с файловой системой
#include <pthread.h>             // Подключаем библиотеку для работы с потоками POSIX
#include <chrono>                // Подключаем библиотеку для работы с временем
#include <condition_variable>    // Подключаем библиотеку для работы с условными переменными
using namespace std;

// Глобальный мьютекс для синхронизации вывода
mutex printM;

// Функция для печати строк с синхронизацией
void print(string str) {
    printM.lock();         // Захватываем мьютекс
    cout << str << endl;   // Печатаем строку
    printM.unlock();       // Освобождаем мьютекс
}

int main() {
    setlocale(LC_ALL, "Russian"); // Устанавливаем локаль для поддержки русского языка
    const int number_of_philosophers = 5; // Константа для числа философов

    // Структура для вилок
    struct Forks {
    public:
        Forks() : is_left_available(true), is_right_available(true) {} // Конструктор, устанавливающий вилки как доступные
        std::mutex mu;             // Мьютекс для синхронизации
        std::condition_variable left_cond, right_cond; // Условные переменные для левой и правой вилки
        bool is_left_available;    // Флаг доступности левой вилки
        bool is_right_available;   // Флаг доступности правой вилки
    };

    // Лямбда-функция для процесса еды философа
    auto eat = [](Forks& left_fork, Forks& right_fork, int philosopher_number) {
        std::unique_lock<std::mutex> lock(left_fork.mu); // Захватываем мьютекс для левой вилки
        left_fork.left_cond.wait(lock, [&] { return left_fork.is_left_available; }); // Ждем, пока левая вилка станет доступной

        int right_fork_index = (number_of_philosophers + (philosopher_number - 1)) % number_of_philosophers; // Индекс правой вилки
        {
            std::unique_lock<std::mutex> right_lock(right_fork.mu); // Захватываем мьютекс для правой вилки
            right_fork.left_cond.wait(lock, [&] { return right_fork.is_right_available; }); // Ждем, пока правая вилка станет доступной

            print("Философ " + std::to_string(philosopher_number) + " ест..."); // Печатаем, что философ ест

            std::chrono::milliseconds timeout(1500); // Задаем задержку в 1.5 секунды
            std::this_thread::sleep_for(timeout); // Засыпаем на 1.5 секунды
            print("Философ " + std::to_string(philosopher_number) + " закончил есть и думает..."); // Печатаем, что философ закончил есть

            left_fork.is_left_available = true; // Освобождаем левую вилку
            left_fork.left_cond.notify_one();   // Уведомляем о доступности левой вилки

            right_fork.is_right_available = true; // Освобождаем правую вилку
            right_fork.right_cond.notify_one();   // Уведомляем о доступности правой вилки
        }
    };

    Forks forks[number_of_philosophers]; // Массив вилок для философов
    std::thread philosopher[number_of_philosophers]; // Массив потоков для философов

    // Создаем и запускаем потоки для каждого философа
    for (int i = 0; i < number_of_philosophers; ++i) {
        auto philosopher_number = i + 1; // Номер философа (начиная с 1)
        print("Философ " + std::to_string(philosopher_number) + " думает..."); // Печатаем, что философ думает

        int previous_fork_index = (number_of_philosophers + (i - 1)) % number_of_philosophers; // Индекс предыдущей вилки (левая вилка)
        philosopher[i] = std::thread(eat, std::ref(forks[i]), std::ref(forks[previous_fork_index]), philosopher_number); // Запускаем поток для философа
    }

    // Ожидаем завершения всех потоков
    for (auto& ph : philosopher) {
        ph.join(); // Ожидание завершения потока
    }

    return 0; // Завершение программы
}
