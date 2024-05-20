#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Функция вычисления частичной суммы ряда Лейбница в определенном диапазоне
void calculatePiPartialSum(int start, int end, double& result) {
    result = 0.0;
    // Вычисление частичной суммы ряда Лейбница для заданного диапазона итераций
    for (int i = start; i < end; ++i) {
        // Формула для ряда Лейбница: π/4 = 1 - 1/3 + 1/5 - 1/7 + ...
        // В зависимости от четности номера итерации прибавляем или вычитаем соответствующий член ряда
        result += (i % 2 == 0 ? 1.0 : -1.0) / (2 * i + 1);
    }
}

int main(int argc, char* argv[]) {
    // Проверка наличия правильного количества аргументов командной строки
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <num_threads>" << endl;
        return 1;
    }

    // Получение количества потоков из аргументов командной строки
    int numThreads = atoi(argv[1]);
    // Проверка, что количество потоков является положительным целым числом
    if (numThreads <= 0) {
        cerr << "Invalid number of threads. Please provide a positive integer." << endl;
        return 1;
    }

    const int iterations = 1000000; // Количество итераций ряда (можно изменить при необходимости)

    // Вектор для хранения потоков
    vector<thread> threads(numThreads);
    // Вектор для хранения частичных сумм ряда, вычисленных каждым потоком
    vector<double> partialSums(numThreads);

    // Запуск потоков для вычисления частичных сумм ряда
    for (int i = 0; i < numThreads; ++i) {
        int start = i * iterations / numThreads; // Начальная итерация для потока
        int end = (i + 1) * iterations / numThreads; // Конечная итерация для потока
        // Создание потока, передача ему диапазона и выделение его части частичной суммы для хранения
        threads[i] = thread(calculatePiPartialSum, start, end, ref(partialSums[i]));
    }

    // Ожидание завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    // Вычисление общей суммы частичных сумм, чтобы получить приближенное значение числа Пи
    double pi = 0.0;
    for (const auto& sum : partialSums) {
        pi += sum;
    }
    pi *= 4; // Умножение на 4 для получения приближенного значения числа Пи

    // Вывод приближенного значения числа Пи с повышенной точностью
    cout << "Approximation of Pi: " << setprecision(15) << pi << endl;

    return 0;
}
