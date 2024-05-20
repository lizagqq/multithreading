#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

const char* PARENT_SEM_NAME = "/parent_semaphore";
const char* CHILD_SEM_NAME = "/child_semaphore";

void parentProcess() {
    // Открыть существующие семафоры
    sem_t* parentSem = sem_open(PARENT_SEM_NAME, 0);
    sem_t* childSem = sem_open(CHILD_SEM_NAME, 0);

    if (parentSem == SEM_FAILED || childSem == SEM_FAILED) {
        cerr << "Failed to open semaphores in parent process" << endl;
        return;
    }

    for (int i = 0; i < 10; ++i) {
        sem_wait(parentSem); // Ждать сигнала от дочернего процесса
        cout << "Это строка из родительского процесса, номер: " << i + 1 << endl;
        sem_post(childSem); // Сигнал дочернему процессу
    }

    sem_close(parentSem);
    sem_close(childSem);
}

void childProcess() {
    // Открыть существующие семафоры
    sem_t* parentSem = sem_open(PARENT_SEM_NAME, 0);
    sem_t* childSem = sem_open(CHILD_SEM_NAME, 0);

    if (parentSem == SEM_FAILED || childSem == SEM_FAILED) {
        cerr << "Failed to open semaphores in child process" << endl;
        return;
    }

    for (int i = 0; i < 10; ++i) {
        sem_wait(childSem); // Ждать сигнала от родительского процесса
        cout << "Это строка из дочернего процесса, номер: " << i + 1 << endl;
        sem_post(parentSem); // Сигнал родительскому процессу
    }

    sem_close(parentSem);
    sem_close(childSem);
}

int main() {
    // Создать именованные семафоры
    sem_t* parentSem = sem_open(PARENT_SEM_NAME, O_CREAT, 0644, 1); // Изначально открыт для родительского процесса
    sem_t* childSem = sem_open(CHILD_SEM_NAME, O_CREAT, 0644, 0); // Изначально закрыт для дочернего процесса

    if (parentSem == SEM_FAILED || childSem == SEM_FAILED) {
        cerr << "Failed to create semaphores" << endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        cerr << "Fork failed" << endl;
        return 1;
    }
    else if (pid == 0) {
        // Дочерний процесс
        childProcess();
    }
    else {
        // Родительский процесс
        parentProcess();
        wait(nullptr); // Ожидание завершения дочернего процесса

        // Удаление именованных семафоров
        sem_unlink(PARENT_SEM_NAME);
        sem_unlink(CHILD_SEM_NAME);
    }

    return 0;
}
