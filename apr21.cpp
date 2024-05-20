#include <iostream>       // Подключение библиотеки для ввода-вывода
#include <windows.h>      // Подключение библиотеки для работы с консолью в Windows
#include <string>         // Подключение библиотеки для работы со строками
#include <thread>         // Подключение библиотеки для работы с потоками
#include <shared_mutex>   // Подключение библиотеки для работы с разделяемыми мьютексами
#include <chrono>         // Подключение библиотеки для работы с временем
#include <algorithm>      // Подключение библиотеки для алгоритмов
#include <vector>         // Подключение библиотеки для работы с векторами
using namespace std;

bool isSorted = true; // Глобальная переменная, указывающая, отсортирован ли список
bool xxx = false;     // Глобальная переменная для управления сортировочным потоком

// Структура для узла списка, хранящая строку и указатель на следующий узел
struct Node {
    string data;                // Строка данных
    Node* next;                 // Указатель на следующий узел
    mutable shared_mutex mtx;   // Разделяемый мьютекс для синхронизации доступа к узлу

    // Конструктор для инициализации узла
    Node(const string& data) : data(data), next(nullptr) {}
};

// Класс для связанного списка
class LinkedList {
private:
    Node* head;                   // Указатель на начало списка
    mutable shared_mutex headMtx; // Разделяемый мьютекс для синхронизации доступа к голове списка

public:
    // Конструктор для инициализации пустого списка
    LinkedList() : head(nullptr) {}

    // Функция для вставки новой строки в начало списка
    void insert(const string& data) {
        Node* newNode = new Node(data);  // Создание нового узла
        unique_lock<shared_mutex> lock(headMtx);  // Уникальная блокировка головы списка
        newNode->next = head;  // Новый узел указывает на текущую голову
        head = newNode;        // Новый узел становится головой списка
        isSorted = false;      // Список больше не отсортирован
    }

    // Функция для вывода всех строк из списка
    void print() {
        shared_lock<shared_mutex> lock(headMtx);  // Разделяемая блокировка головы списка
        Node* current = head;  // Указатель на текущий узел
        while (current != nullptr) {  // Пока есть узлы в списке
            shared_lock<shared_mutex> nodeLock(current->mtx);  // Разделяемая блокировка текущего узла
            cout << current->data << endl;  // Вывод строки
            current = current->next;  // Переход к следующему узлу
        }
    }

    // Функция для выполнения одного шага пузырьковой сортировки
    void bubbleSortStep() {
        if (!head) return;  // Если список пуст, ничего не делать
        bool swapped;       // Флаг для отслеживания обменов
        Node* ptr1;         // Указатель для прохода по списку
        Node* lptr = nullptr;  // Указатель на последний отсортированный узел

        do {
            cout << "Текущее состояние списка:" << endl;
            this->print();  // Печать текущего состояния списка
            cout << endl;
            swapped = false;  // Сброс флага обменов
            unique_lock<shared_mutex> lock(headMtx);  // Уникальная блокировка головы списка
            ptr1 = head;  // Начало прохода по списку
            Node* prev = nullptr;  // Указатель на предыдущий узел (не используется, но оставлен для ясности)

            while (ptr1->next != lptr) {  // Пока не дойдем до последнего отсортированного узла
                unique_lock<shared_mutex> lock1(ptr1->mtx);  // Уникальная блокировка текущего узла
                unique_lock<shared_mutex> lock2(ptr1->next->mtx);  // Уникальная блокировка следующего узла
                if (ptr1->data > ptr1->next->data) {  // Если данные в текущем узле больше данных в следующем узле
                    swap(ptr1->data, ptr1->next->data);  // Обмен данными
                    swapped = true;  // Установка флага обменов
                }
                prev = ptr1;  // Обновление предыдущего узла
                ptr1 = ptr1->next;  // Переход к следующему узлу
            }
            lptr = ptr1;  // Обновление указателя на последний отсортированный узел
            this_thread::sleep_for(chrono::seconds(1));  // Пауза на 1 секунду между шагами
        } while (swapped);  // Повтор до тех пор, пока происходят обмены

        isSorted = true;  // Установка флага, что список отсортирован
        xxx = false;      // Сброс глобальной переменной
    }

    // Функция для выполнения одного шага пузырьковой сортировки с паузой
    void bubbleSortStepWithDelay() {
        bubbleSortStep();  // Вызов функции сортировки
        this_thread::sleep_for(chrono::seconds(1)); // Пауза между шагами сортировки
    }
};

// Функция для сортировочного потока
void sortingThread(LinkedList& list) {
    while (true) {  // Бесконечный цикл
        if (!isSorted && xxx) {  // Если список не отсортирован и установлен флаг xxx
            list.bubbleSortStepWithDelay();  // Вызов функции сортировки с паузой
        }
    }
}

int main() {
    // Установка кодировки для поддержки кириллицы в консоли
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    LinkedList list;  // Создание объекта связанного списка
    thread sorting(sortingThread, ref(list));  // Создание и запуск сортировочного потока

    while (true) {  // Бесконечный цикл для ввода данных
        string input;  // Переменная для хранения ввода пользователя
        getline(cin, input);  // Считывание строки ввода

        if (input.empty()) {  // Если строка пустая
            if (isSorted) {  // Если список отсортирован
                cout << "Текущее состояние списка:" << endl;
                list.print();  // Вывод текущего состояния списка
            }
            else {
                xxx = true;  // Установка флага xxx для запуска сортировки
            }
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

    sorting.join();  // Ожидание завершения сортировочного потока (этот код никогда не выполнится из-за бесконечного цикла)
    return 0;
}
