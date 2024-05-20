#include <iostream>   
#include <windows.h>  
#include <string>     
#include <thread>     
#include <mutex>      
#include <chrono>     
#include <algorithm>  
#include <vector>     
using namespace std;

// Структура для узла списка, хранящая строку и указатель на следующий узел
struct Node {
    string data;   // Строка данных
    Node* next;    // Указатель на следующий узел
    mutex mtx;     // Мьютекс для синхронизации доступа к узлу

    // Конструктор для инициализации узла
    Node(const string& data) : data(data), next(nullptr) {}
};

// Класс для связанного списка
class LinkedList {
private:
    Node* head;     // Указатель на начало списка
    mutex headMtx;  // Мьютекс для синхронизации доступа к голове списка

public:
    // Конструктор для инициализации пустого списка
    LinkedList() : head(nullptr) {}

    // Функция для вставки новой строки в начало списка
    void insert(const string& data) {
        Node* newNode = new Node(data);   // Создание нового узла
        lock_guard<mutex> lock(headMtx);  // Блокировка мьютекса для синхронизации
        newNode->next = head;  // Новый узел указывает на текущую голову
        head = newNode;        // Новый узел становится головой списка
    }

    // Функция для вывода всех строк из списка
    void print() {
        lock_guard<mutex> lock(headMtx);  // Блокировка мьютекса для синхронизации
        Node* current = head;  // Указатель на текущий узел
        while (current != nullptr) {  // Пока есть узлы в списке
            lock_guard<mutex> nodeLock(current->mtx);  // Блокировка мьютекса узла
            cout << current->data << endl;  // Вывод строки
            current = current->next;  // Переход к следующему узлу
        }
    }

    // Функция для сортировки списка пузырьковой сортировкой
    void bubbleSort() {
        if (!head) return;  // Если список пуст, ничего не делать
        bool swapped;  // Флаг для отслеживания обменов
        Node* ptr1;    // Указатель для прохода по списку
        Node* lptr = nullptr;  // Указатель на последний отсортированный узел

        do {
            swapped = false;  // Сброс флага обменов
            lock_guard<mutex> lock(headMtx);  // Блокировка мьютекса для синхронизации
            ptr1 = head;  // Начало прохода по списку
            Node* prev = nullptr;  // Указатель на предыдущий узел (не используется)

            while (ptr1->next != lptr) {  // Пока не дойдем до последнего отсортированного узла
                lock_guard<mutex> lock1(ptr1->mtx);        // Блокировка текущего узла
                lock_guard<mutex> lock2(ptr1->next->mtx);  // Блокировка следующего узла

                if (ptr1->data > ptr1->next->data) {  // Если данные в текущем узле больше данных в следующем узле
                    swap(ptr1->data, ptr1->next->data);  // Обмен данными
                    swapped = true;  // Установка флага обменов
                }
                prev = ptr1;  // Переход к следующему узлу
                ptr1 = ptr1->next;  // Переход к следующему узлу
            }
            lptr = ptr1;  // Обновление указателя на последний отсортированный узел
        } while (swapped);  // Повтор до тех пор, пока происходят обмены
    }
};

// Функция для сортировочного потока
void sortingThread(LinkedList& list, int interval) {
    while (true) {  // Бесконечный цикл
        this_thread::sleep_for(chrono::seconds(interval));  // Ожидание указанного времени
        list.bubbleSort();  // Сортировка списка
    }
}

int main() {
    // Установка кодировки для поддержки кириллицы в консоли
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    LinkedList list;  // Создание объекта связанного списка
    vector<thread> sortingThreads;  // Вектор для хранения потоков сортировки
    int numThreads = 2;  // Количество потоков сортировки
    int intervalSeconds = 5;  // Интервал сортировки в секундах

    // Создание и запуск потоков сортировки
    for (int i = 0; i < numThreads; ++i) {
        sortingThreads.emplace_back(sortingThread, ref(list), intervalSeconds);
    }

    while (true) {  // Бесконечный цикл для ввода данных
        string input;  // Переменная для хранения ввода пользователя
        getline(cin, input);  // Считывание строки ввода

        if (input.empty()) {  // Если строка пустая
            cout << "Текущее состояние списка:" << endl;
            list.print();  // Вывод текущего состояния списка
        }
        else {
            if (input.length() > 80) {  // Если строка длиннее 80 символов
                for (size_t i = 0; i < input.length(); i += 80) {  // Разделение строки на части по 80 символов
                    string substr = input.substr(i, 80);
                    list.insert(substr);  // Вставка каждой части в список
                }
            }
            else {
                list.insert(input);  // Вставка строки в список
            }
        }
    }

    // Ожидание завершения потоков сортировки (этот код никогда не выполнится из-за бесконечного цикла)
    for (auto& thread : sortingThreads) {
        thread.join();
    }
    return 0;
}
