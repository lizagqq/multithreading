#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

// Мьютекс для синхронизации вывода в консоль
mutex mPrint;

// Функция для потокобезопасного вывода строк в консоль
void print(string str) {
    mPrint.lock(); // Блокируем мьютекс
    cout << str << endl; // Выводим строку
    mPrint.unlock(); // Разблокируем мьютекс
}

// Класс для реализации потокобезопасной очереди сообщений
class MessageQueue {
private:
    queue<string> queue_; // Очередь для хранения сообщений
    mutex mutex_; // Мьютекс для синхронизации доступа к очереди
    condition_variable messageAvailable_; // Условная переменная для сигнализации, что в очереди есть сообщение
    const size_t maxSize_ = 10; // Максимальный размер очереди

public:
    // Метод для добавления сообщения в очередь
    void put(const string& msg) {
        unique_lock<mutex> lock(mutex_); // Захватываем мьютекс с уникальной блокировкой
        // Ждем, пока в очереди будет место для нового сообщения
        messageAvailable_.wait(lock, [this] { return queue_.size() < maxSize_; });
        queue_.push(msg.substr(0, 80)); // Добавляем сообщение (обрезаем до 80 символов)
        lock.unlock(); // Освобождаем мьютекс
        messageAvailable_.notify_one(); // Уведомляем один из потоков, что в очереди есть новое сообщение
    }

    // Метод для получения сообщения из очереди
    string get() {
        unique_lock<mutex> lock(mutex_); // Захватываем мьютекс с уникальной блокировкой
        // Ждем, пока в очереди не появится сообщение
        messageAvailable_.wait(lock, [this] { return !queue_.empty(); });
        string msg = queue_.front(); // Получаем первое сообщение в очереди
        queue_.pop(); // Удаляем сообщение из очереди
        lock.unlock(); // Освобождаем мьютекс
        return msg; // Возвращаем сообщение
    }

    // Метод для очистки очереди и пробуждения всех ожидающих потоков
    void drop() {
        lock_guard<mutex> lock(mutex_); // Захватываем мьютекс с блокировкой на всё время выполнения метода
        messageAvailable_.notify_all(); // Уведомляем все потоки, что в очереди есть изменения
    }
};

// Функция-производитель для добавления сообщений в очередь
void producer(MessageQueue& queue, int id) {
    for (int i = 0; i < 5; ++i) {
        // Создаем сообщение с номером производителя и номером сообщения
        string msg = "Сообщение от производителя " + to_string(id) + ", номер " + to_string(i);
        queue.put(msg); // Добавляем сообщение в очередь
        this_thread::sleep_for(chrono::milliseconds(100)); // Имитируем работу, приостанавливаем поток на 100 мс
    }
}

// Функция-потребитель для получения сообщений из очереди
void consumer(MessageQueue& queue, int id) {
    for (int i = 0; i < 5; ++i) {
        string msg = queue.get(); // Получаем сообщение из очереди
        // Печатаем сообщение в консоль
        print("Потребитель " + to_string(id) + " получил: " + msg);
        this_thread::sleep_for(chrono::milliseconds(500)); // Имитируем работу, приостанавливаем поток на 500 мс
    }
}

int main() {
    setlocale(LC_ALL, "Ru"); // Устанавливаем локаль для поддержки русского языка

    MessageQueue queue; // Создаем очередь сообщений

    // Создаем и запускаем потоки-производители и потоки-потребители
    thread producer1(producer, ref(queue), 1);
    thread producer2(producer, ref(queue), 2);
    thread consumer1(consumer, ref(queue), 1);
    thread consumer2(consumer, ref(queue), 2);

    // Ожидаем завершения всех потоков
    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();

    return 0; // Завершаем программу
}
