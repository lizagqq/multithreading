#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

// Функция для сортировки и вывода строки
void sleepSortAndPrint(const string& str) {
    // Рассчитываем время сна в миллисекундах, пропорциональное длине строки
    int sleepTime = str.length() * 10; // Множитель 10 выбран рандомно
    
    // Спим на рассчитанное время
    this_thread::sleep_for(chrono::milliseconds(sleepTime));
    
    // Выводим строку
    cout << str << endl;
}

int main() {
    // Чтение строк из стандартного ввода
    vector<string> strings;
    string input;
    while (getline(cin, input)) {
        strings.push_back(input);
    }

    // Создание и запуск потоков для каждой строки
    vector<thread> threads;
    for (const auto& str : strings) {
        threads.emplace_back(sleepSortAndPrint, str);
    }

    // Ожидание завершения всех потоков
    for (auto& th : threads) {
        th.join();
    }

    return 0;
}
