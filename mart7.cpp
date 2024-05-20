#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <fstream>
#include <filesystem>
#include <future>

using namespace std;
namespace fs = std::filesystem; // Используем псевдоним для пространства имен std::filesystem

// Функция для копирования файлов
void copyFile(const string& src, const string& dst) {
    try {
        ifstream in(src, ios::binary); // Открываем исходный файл для чтения в бинарном режиме
        if (!in) { // Проверяем, удалось ли открыть файл
            throw runtime_error("Cannot open source file: " + src); // Если нет, выбрасываем исключение
        }

        ofstream out(dst, ios::binary); // Открываем целевой файл для записи в бинарном режиме
        if (!out) { // Проверяем, удалось ли открыть файл
            throw runtime_error("Cannot open destination file: " + dst); // Если нет, выбрасываем исключение
        }

        out << in.rdbuf(); // Копируем содержимое исходного файла в целевой файл
    }
    catch (const exception& e) { // Обрабатываем исключения
        cerr << "Error copying file from " << src << " to " << dst << ": " << e.what() << endl; // Выводим сообщение об ошибке
    }
}

// Функция для копирования директорий
void copyDir(const string& src, const string& dst) {
    try {
        fs::create_directory(dst); // Создаем целевую директорию

        vector<fs::path> files, directories; // Векторы для хранения файлов и поддиректорий

        // Перебираем все элементы в исходной директории
        for (const auto& entry : fs::directory_iterator(src)) {
            if (fs::is_regular_file(entry)) { // Если элемент - файл, добавляем его в вектор files
                files.push_back(entry.path());
            }
            else if (fs::is_directory(entry)) { // Если элемент - директория, добавляем ее в вектор directories
                directories.push_back(entry.path());
            }
        }

        vector<future<void>> fileFutures, dirFutures; // Векторы для хранения асинхронных задач

        // Асинхронное копирование файлов
        for (const auto& file : files) {
            string newSrc = file.string(); // Полный путь к исходному файлу
            string newDst = dst + "\\" + file.filename().string(); // Полный путь к целевому файлу
            cout << "Copying file\nFrom: " << newSrc << "\nTo: " << newDst << endl;
            fileFutures.push_back(async(launch::async, copyFile, newSrc, newDst)); // Запускаем асинхронную задачу копирования файла
        }

        // Асинхронное копирование директорий
        for (const auto& directory : directories) {
            string newSrc = directory.string(); // Полный путь к исходной директории
            string newDst = dst + "\\" + directory.filename().string(); // Полный путь к целевой директории
            cout << "Copying directory\nFrom: " << newSrc << "\nTo: " << newDst << endl;
            dirFutures.push_back(async(launch::async, copyDir, newSrc, newDst)); // Запускаем асинхронную задачу копирования директории
        }

        // Ожидание завершения всех задач копирования файлов
        for (auto& future : fileFutures) {
            future.get(); // Ожидаем завершения асинхронной задачи
        }
        // Ожидание завершения всех задач копирования директорий
        for (auto& future : dirFutures) {
            future.get(); // Ожидаем завершения асинхронной задачи
        }
    }
    catch (const exception& e) { // Обрабатываем исключения
        cerr << "Error copying directory from " << src << " to " << dst << ": " << e.what() << endl; // Выводим сообщение об ошибке
    }
}

int main() {
    try {
        cout << "\n\nstart\n\n";

        string src = "C:\\Users\\Wind\\source\\repos\\mart6"; // Исходная директория
        string dst = "C:\\Users\\Wind\\source\\repos\\mart6-copy"; // Целевая директория

        // Проверяем, существует ли исходная директория
        if (!fs::exists(src)) {
            cerr << "Source directory does not exist: " << src << endl;
            return 1; // Завершаем программу с ошибкой
        }

        // Запускаем функцию копирования директории в отдельном потоке
        thread t(copyDir, src, dst);
        t.join(); // Ожидаем завершения потока

        cout << "\n\nend\n\n";
    }
    catch (const exception& e) { // Обрабатываем исключения
        cerr << "Error in main: " << e.what() << endl; // Выводим сообщение об ошибке
        return 1; // Завершаем программу с ошибкой
    }

    return 0; // Успешное завершение программы
}
