#include <iostream>
#include <thread>
using namespace std;

void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        cout << "Это строка из вновь созданного потока, номер: " << i + 1 << endl;
    }
}

int main() {

    setlocale(LC_ALL, "Ru");

    thread myThread(threadFunction);

    // Вывод десяти строк текста из родительского потока
    for (int i = 0; i < 10; ++i) {
        cout << "Это строка из родительского потока, номер: " << i + 1 << endl;
    }

    myThread.join();

    return 0;
}