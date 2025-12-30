// Assignment 2 task 3:Параллельная сортировка с OpenMP
// Реализуйте алгоритм сортировки выбором с использованием OpenMP:
// 1. Напишите последовательную реализацию алгоритма;
// 2. Добавьте параллелизм с помощью директив OpenMP;
// 3. Проверьте производительность для массивов размером 1 000 и 10 000 элементов.

#include <iostream>      // Для работы с вводом/выводом (cout, cin, endl)
#include <vector>        // Подключение контейнера vector
#include <random>        // Для генерации случайных чисел (random_device, mt19937, uniform_int_distribution)
#include <chrono>        // Для измерения времени выполнения
#include <omp.h>         // Для OpenMP (параллельные вычисления)
using namespace std;     // Стандартное пространство имён, чтобы не писать std:: перед cout, endl и т.д.


// Функция заполнения массива случайными числами
void fillArray(vector<int>& arr) {                  // Функция принимает массив по ссылке
    random_device rd;                  // random_device используется как источник случайности
    mt19937 gen(rd());                 // mt19937 — генератор случайных чисел Mersenne Twister
                                       // Инициализируем его значением из rd(), чтобы последовательность была случайной
    
    uniform_int_distribution<int> dist(0, 10000);      // uniform_int_distribution задаёт равномерное распределение чисел от 1 до 10000 (можно изменить)
                                                       // dist(gen) будет возвращать случайное число из этого диапазона


    for (int& x : arr) {                              // Проход по всем элементам массива
        x = dist(gen);                                 // Генерация случайного числа
    }
}


// Последовательная сортировка выбором
void selectionSortSequential(vector<int>& arr) {    // Функция последовательной сортировки
    int n = arr.size();                             // Получение размера массива

    for (int i = 0; i < n - 1; ++i) {               // Внешний цикл сортировки
        int minIndex = i;                           // Индекс минимального элемента

        for (int j = i + 1; j < n; ++j) {           // Поиск минимума в неотсортированной части
            if (arr[j] < arr[minIndex]) {           // Проверка условия минимума
                minIndex = j;                       // Обновление индекса минимума
            }
        }

        swap(arr[i], arr[minIndex]);                // Обмен текущего и минимального элементов
    }
}

// Параллельная сортировка выбором (OpenMP)
void selectionSortParallel(vector<int>& arr) {      // Функция параллельной сортировки
    int n = arr.size();                             // Размер массива

    for (int i = 0; i < n - 1; ++i) {               // Внешний цикл 
        int minIndex = i;                           // Общий индекс минимума

        #pragma omp parallel                        // Начало параллельной области
        {
            int localMinIndex = minIndex;           // Локальный минимум для каждого потока

            #pragma omp for nowait                  // Параллельный цикл поиска минимума
            for (int j = i + 1; j < n; ++j) {       // Каждый поток проверяет часть массива
                if (arr[j] < arr[localMinIndex]) {  // Сравнение элементов
                    localMinIndex = j;              // Обновление локального минимума
                }
            }

            #pragma omp critical                    // Критическая секция
            {
                if (arr[localMinIndex] < arr[minIndex]) { // Сравнение локального и глобального минимума
                    minIndex = localMinIndex;             // Обновление глобального минимума
                }
            }
        }

        swap(arr[i], arr[minIndex]);                // Обмен элементов после поиска минимума
    }
}


// Функция тестирования производительности
void testPerformance(int size) {                    // Функция принимает размер массива
    vector<int> arr(size);                          // Создание массива заданного размера
    fillArray(arr);                                 // Заполнение массива случайными числами

    vector<int> arrSeq = arr;                       // Копия массива для последовательной версии
    vector<int> arrPar = arr;                       // Копия массива для параллельной версии

    auto startSeq = chrono::high_resolution_clock::now(); // Начало замера времени (последовательно)
    selectionSortSequential(arrSeq);                      // Запуск последовательной сортировки
    auto endSeq = chrono::high_resolution_clock::now();   // Конец замера времени
    chrono::duration<double> timeSeq = endSeq - startSeq; // Расчет времени выполнения

    auto startPar = chrono::high_resolution_clock::now(); // Начало замера времени (параллельно)
    selectionSortParallel(arrPar);                        // Запуск параллельной сортировки
    auto endPar = chrono::high_resolution_clock::now();   // Конец замера времени
    chrono::duration<double> timePar = endPar - startPar; // Расчет времени выполнения

    cout << "Размер массива: " << size << endl;     // Вывод размера массива
    cout << "Последовательная сортировка: " 
         << timeSeq.count() << " сек" << endl;      // Вывод времени последовательной версии
    cout << "Параллельная сортировка (OpenMP): " 
         << timePar.count() << " сек" << endl;      // Вывод времени параллельной версии

}


// Основная функция
int main() {                                        // Точка входа в программу
    testPerformance(1000);                          // Тест для массива из 1000 элементов
    testPerformance(10000);                         // Тест для массива из 10000 элементов

    return 0;                                       // Завершение программы
}
