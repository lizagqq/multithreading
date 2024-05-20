#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>

using namespace std;

// Объявление семафоров
std::counting_semaphore<1> parent_semaphore(1);  // Изначально открыт для родительского потока
std::counting_semaphore<1> child_semaphore(0);   // Изначально закрыт для дочернего потока

// Функция, выполняемая дочерним потоком
void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        child_semaphore.acquire(); // Ожидание сигнала от родительского потока
        cout << "Это строка из вновь созданного потока, номер: " << i + 1 << endl;
        parent_semaphore.release(); // Сигнал родительскому потоку
    }
}

int main() {
    setlocale(LC_ALL, "Ru");

    std::thread myThread(threadFunction);

    // Вывод строк родительским потоком
    for (int i = 0; i < 10; ++i) {
        parent_semaphore.acquire(); // Ожидание сигнала от дочернего потока
        cout << "Это строка из родительского потока, номер: " << i + 1 << endl;
        child_semaphore.release(); // Сигнал дочернему потоку
    }

    myThread.join();
    return 0;
}
