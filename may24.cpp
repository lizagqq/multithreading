#include <iostream>                 // для работы с вводом-выводом
#include <vector>                   // для использования вектора
#include <thread>                   // для использования потоков
#include <mutex>                    // для синхронизации с помощью мьютексов
#include <condition_variable>       // для использования условных переменных
#include <chrono>                   // для работы со временем

using namespace std;

// Мьютексы для синхронизации доступа к деталям A, B, C и винтику
mutex mtxA;
mutex mtxB;
mutex mtxC;
mutex mtxBolt;
condition_variable cv;             // Условная переменная для синхронизации

// Счетчики для деталей A, B и C
int A_count = 0;
int B_count = 0;
int C_count = 0;

// Максимальное количество винтиков
const int MAX_PRODUCTS = 5;

// Структура для представления винтика, который состоит из деталей A, B и C
struct Bolt {
    bool isAReady;                // Флаг готовности детали A
    bool isBReady;                // Флаг готовности детали B
    bool isCReady;                // Флаг готовности детали C
    bool isProduced;              // Флаг готовности винтика
};

// Инициализация вектора винтиков
vector<Bolt> initBoltsVector() {
    vector<Bolt> bolts(MAX_PRODUCTS);
    return bolts;
}

// Функция для изготовления детали A
void makeA(vector<Bolt>& bolts) {
    for (int i = 0; i < MAX_PRODUCTS; ++i) {
        this_thread::sleep_for(chrono::seconds(1)); // Имитируем время на изготовление
        {
            lock_guard<mutex> lck(mtxA);           // Блокируем мьютекс для безопасного доступа
            A_count++;
            bolts[i].isAReady = true;              // Устанавливаем флаг готовности детали A
            cout << "Деталь A изготовлена. Всего произведено: " << A_count << endl;
        }
        cv.notify_all();                           // Уведомляем все потоки, ожидающие на условной переменной
    }
}

// Функция для изготовления детали B
void makeB(vector<Bolt>& bolts) {
    for (int i = 0; i < MAX_PRODUCTS; ++i) {
        this_thread::sleep_for(chrono::seconds(2)); // Имитируем время на изготовление
        {
            lock_guard<mutex> lck(mtxB);           // Блокируем мьютекс для безопасного доступа
            B_count++;
            bolts[i].isBReady = true;              // Устанавливаем флаг готовности детали B
            cout << "Деталь B изготовлена. Всего произведено: " << B_count << endl;
        }
        cv.notify_all();                           // Уведомляем все потоки, ожидающие на условной переменной
    }
}

// Функция для изготовления детали C
void makeC(vector<Bolt>& bolts) {
    for (int i = 0; i < MAX_PRODUCTS; ++i) {
        this_thread::sleep_for(chrono::seconds(3)); // Имитируем время на изготовление
        {
            lock_guard<mutex> lck(mtxC);           // Блокируем мьютекс для безопасного доступа
            C_count++;
            bolts[i].isCReady = true;              // Устанавливаем флаг готовности детали C
            cout << "Деталь C изготовлена. Всего произведено: " << C_count << endl;
        }
        cv.notify_all();                           // Уведомляем все потоки, ожидающие на условной переменной
    }
}

// Функция для сборки винтика из деталей A, B и C
void makeBolt(vector<Bolt>& bolts) {
    for (int i = 0; i < MAX_PRODUCTS; ++i) {
        unique_lock<mutex> lck(mtxBolt);           // Блокируем мьютекс для безопасного доступа
        cv.wait(lck, [&bolts, i] {                 // Ожидаем пока все детали будут готовы
            return (bolts[i].isAReady && bolts[i].isBReady && bolts[i].isCReady) && !bolts[i].isProduced;
            });
        bolts[i].isProduced = true;                // Устанавливаем флаг готовности винтика
        cout << "Винтик готов!" << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Ru");                       // Устанавливаем локаль для поддержки русского языка

    vector<Bolt> boltsVector = initBoltsVector();  // Инициализируем вектор винтиков

    // Создаем потоки для изготовления деталей A, B, C и сборки винтиков
    thread threadA(makeA, ref(boltsVector));
    thread threadB(makeB, ref(boltsVector));
    thread threadC(makeC, ref(boltsVector));
    thread boltThread(makeBolt, ref(boltsVector));

    // Ожидаем завершения всех потоков
    threadA.join();
    threadB.join();
    threadC.join();
    boltThread.join();

    return 0;
}
