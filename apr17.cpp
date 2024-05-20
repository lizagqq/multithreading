#include <iostream>
#include <string>
#include <thread>
#include <windows.h>
#include <mutex>
#include <chrono>
#include <algorithm>

using namespace std;

// Структура для узла списка, хранящая строку и указатель на следующий узел
struct Node {
    string data;  // Строка данных
    Node* next;   // Указатель на следующий узел

    // Конструктор для инициализации узла
    Node(const string& data) : data(data), next(nullptr) {}
};

// Класс для связанного списка
class LinkedList {
private:
    Node* head;   // Указатель на начало списка
    mutex mtx;    // Мьютекс для синхронизации

public:
    // Конструктор для инициализации пустого списка
    LinkedList() : head(nullptr) {}

    // Функция для вставки новой строки в начало списка
    void insert(const string& data) {
        lock_guard<mutex> lock(mtx);  // Блокировка мьютекса на время работы с списком
        Node* newNode = new Node(data);  // Создание нового узла
        newNode->next = head;  // Установка указателя на следующий узел
        head = newNode;  // Новый узел становится головой списка
    }

    // Функция для вывода всех строк из списка
    void print() {
        lock_guard<mutex> lock(mtx);  // Блокировка мьютекса на время работы с списком
        Node* current = head;  // Указатель на текущий узел
        while (current != nullptr) {  // Пока есть узлы в списке
            cout << current->data << endl;  // Вывод строки
            current = current->next;  // Переход к следующему узлу
        }
    }

    // Функция для сортировки списка пузырьковой сортировкой
    void bubbleSort() {
        lock_guard<mutex> lock(mtx);  // Блокировка мьютекса на время работы с списком
        if (!head) return;  // Если список пуст, ничего не делать

        bool swapped;  // Флаг для отслеживания обменов
        Node* ptr1;  // Указатель для прохода по списку
        Node* lptr = nullptr;  // Указатель на последний отсортированный узел

        do {
            swapped = false;  // Сброс флага обменов
            ptr1 = head;  // Начало прохода по списку

            while (ptr1->next != lptr) {  // Пока не дойдем до последнего отсортированного узла
                if (ptr1->data > ptr1->next->data) {  // Если данные в текущем узле больше данных в следующем узле
                    swap(ptr1->data, ptr1->next->data);  // Обмен данными
                    swapped = true;  // Установка флага обменов
                }
                ptr1 = ptr1->next;  // Переход к следующему узлу
            }
            lptr = ptr1;  // Обновление указателя на последний отсортированный узел
        } while (swapped);  // Повтор до тех пор, пока происходят обмены
    }
};

// Функция для сортировочного потока
void sortingThread(LinkedList& list) {
    while (true) {  // Бесконечный цикл
        this_thread::sleep_for(chrono::seconds(5));  // Ожидание 5 секунд
        list.bubbleSort();  // Сортировка списка
    }
}

int main() {
    // Установка кодировки для поддержки кириллицы в консоли
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    LinkedList list;  // Создание объекта связанного списка
    thread sorting(sortingThread, ref(list));  // Создание и запуск потока сортировки

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

    sorting.join();  // Ожидание завершения потока сортировки (этот код никогда не выполнится из-за бесконечного цикла)
    return 0;
}
