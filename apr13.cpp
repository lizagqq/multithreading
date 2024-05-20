#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool parent_turn = true; // true - очередь родительского потока, false - дочернего потока

// Функция, выполняемая дочерним потоком
void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        // Ожидание, пока не наступит очередь дочернего потока
        cv.wait(lock, [] { return !parent_turn; });
        std::cout << "Это строка из вновь созданного потока, номер: " << i + 1 << std::endl;
        parent_turn = true; // Устанавливаем очередь родительского потока
        cv.notify_all(); // Уведомляем все потоки
    }
}

int main() {
    setlocale(LC_ALL, "Ru");

    std::thread myThread(threadFunction);

    // Вывод строк родительским потоком
    for (int i = 0; i < 10; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        // Ожидание, пока не наступит очередь родительского потока
        cv.wait(lock, [] { return parent_turn; });
        std::cout << "Это строка из родительского потока, номер: " << i + 1 << std::endl;
        parent_turn = false; // Устанавливаем очередь дочернего потока
        cv.notify_all(); // Уведомляем все потоки
    }

    myThread.join();
    return 0;
}
