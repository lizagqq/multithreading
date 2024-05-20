#include <iostream>
#include <pthread.h>

using namespace std;

void* threadFunction(void*) {
    for (int i = 0; i < 10; ++i) {
        cout << "Это строка из вновь созданного потока, номер: " << i + 1 << endl;
    }
    return nullptr;
}

int main() {
    setlocale(LC_ALL, "Ru");

    pthread_t thread;
    pthread_create(&thread, nullptr, threadFunction, nullptr);

    // Ожидание завершения работы вновь созданного потока
    pthread_join(thread, nullptr);

    // Вывод десяти строк текста из родительского потока
    for (int i = 0; i < 10; ++i) {
        cout << "Это строка из родительского потока, номер: " << i + 1 << endl;
    }

    return 0;
}
