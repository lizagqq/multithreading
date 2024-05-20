#include <iostream>
#include <pthread.h>
#include <unistd.h> // для функции sleep

using namespace std;

// Функция, выполняемая дочерним потоком
void* printText(void*) {
    // Функция очистки, которая будет вызвана при отмене потока
    auto cleanup = [](void*) {
        cout << "Дочерний поток завершает работу" << endl;
    };

    // Регистрация функции очистки
    pthread_cleanup_push(cleanup, nullptr);

    while (true) {
        cout << "Дочерний поток работает" << endl;
        sleep(1); // Ждать 1 секунду перед печатью следующей строки
    }

    // Отмена функции очистки
    pthread_cleanup_pop(1);
    return nullptr;
}

int main() {
    setlocale(LC_ALL, "Ru");

    pthread_t thread; // Идентификатор дочернего потока

    // Создание дочернего потока
    if (pthread_create(&thread, nullptr, printText, nullptr) != 0) {
        cerr << "Ошибка при создании потока" << endl;
        return 1;
    }

    // Родительский поток ждет 2 секунды
    sleep(2);

    // Прерывание дочернего потока
    if (pthread_cancel(thread) != 0) {
        cerr << "Ошибка при прерывании потока" << endl;
        return 1;
    }

    // Ожидание завершения дочернего потока
    if (pthread_join(thread, nullptr) != 0) {
        cerr << "Ошибка при ожидании завершения потока" << endl;
        return 1;
    }

    cout << "Дочерний поток был прерван" << endl;

    return 0;
}
