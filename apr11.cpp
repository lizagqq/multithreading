#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable cv;
bool parent_turn = true;

void threadFunction() {
    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return !parent_turn; });
        cout << "Это строка из вновь созданного потока, номер: " << i + 1 << endl;
        parent_turn = true;
        cv.notify_one();
    }
}

int main() {
    setlocale(LC_ALL, "Ru");

    thread myThread(threadFunction);

    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return parent_turn; });
        cout << "Это строка из родительского потока, номер: " << i + 1 << endl;
        parent_turn = false;
        cv.notify_one();
    }

    myThread.join();

    return 0;
}