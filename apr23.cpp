#include <iostream>         // для работы с вводом-выводом
#include <vector>           // для использования вектора
#include <string>           // для работы со строками
#include <thread>           // для использования потоков
#include <mutex>            // для синхронизации с помощью мьютексов
#include <condition_variable> // для использования условных переменных
#include <chrono>           // для работы со временем
#include <windows.h>        // для установки кодировки консоли

using namespace std;

// Структура узла для связного списка
struct Node {
    string data;          // строка данных
    Node* next;           // указатель на следующий узел
    Node(const string& data) : data(data), next(nullptr) {} // конструктор узла
};

// Класс для связного списка
class LinkedList {
private:
    Node* head;           // указатель на первый узел списка
    mutex listMtx;        // мьютекс для синхронизации доступа к списку

public:
    LinkedList() : head(nullptr) {} // конструктор списка, начальная голова - nullptr

    // Вставка строки в связный список в отсортированном порядке
    void insert(const string& data) {
        Node* newNode = new Node(data); // создаем новый узел с данными
        lock_guard<mutex> lock(listMtx); // блокируем мьютекс для безопасной вставки
        if (!head || head->data > data) { // если список пуст или новый элемент меньше первого
            newNode->next = head;        // вставляем новый узел в начало списка
            head = newNode;              // обновляем голову списка
        } else {
            Node* current = head;
            // находим место для нового узла
            while (current->next && current->next->data <= data) {
                current = current->next;
            }
            newNode->next = current->next; // вставляем новый узел после текущего
            current->next = newNode;
        }
    }

    // Вывод всех элементов списка
    void print() {
        lock_guard<mutex> lock(listMtx); // блокируем мьютекс для безопасного доступа к списку
        Node* current = head; // начинаем с головы списка
        while (current) { // пока не дойдем до конца списка
            cout << current->data << endl; // выводим данные узла
            current = current->next; // переходим к следующему узлу
        }
    }
};

// Функция для потока, который сортирует часть данных и вставляет их в список
void sortingThread(vector<string>& data, LinkedList& list, int start, int end) {
    // Сортировка части массива
    sort(data.begin() + start, data.begin() + end);

    // Вставка отсортированных данных в связный список
    for (int i = start; i < end; ++i) {
        list.insert(data[i]); // вставляем строку в список
    }
}

int main() {
    // Устанавливаем кодировку консоли на UTF-8 для корректного отображения русских символов
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    setlocale(LC_ALL, "Russian"); // устанавливаем локаль для поддержки русского языка

    vector<string> data; // вектор для хранения данных
    string input;

    // Чтение данных из стандартного ввода
    while (getline(cin, input)) {
        if (input.empty()) break; // если введена пустая строка, прекращаем ввод
        data.push_back(input); // добавляем строку в вектор
    }

    // Определяем количество потоков и размер порции для сортировки
    int numThreads = 2; // количество потоков
    int portionSize = (data.size() + numThreads - 1) / numThreads; // размер порции для каждого потока

    LinkedList list; // создаем связный список
    vector<thread> sortingThreads; // вектор для хранения потоков

    // Запуск сортирующих потоков
    for (int i = 0; i < numThreads; ++i) {
        int start = i * portionSize; // начальный индекс для текущего потока
        int end = min((i + 1) * portionSize, static_cast<int>(data.size())); // конечный индекс для текущего потока
        sortingThreads.emplace_back(sortingThread, ref(data), ref(list), start, end); // создаем и запускаем поток
    }

    // Ожидание завершения всех потоков
    for (auto& thread : sortingThreads) {
        thread.join(); // ожидаем завершения потока
    }

    // Вывод содержимого связного списка
    list.print(); // выводим весь список

    return 0;
}
