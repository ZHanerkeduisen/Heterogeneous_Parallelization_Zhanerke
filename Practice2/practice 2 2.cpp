// Practice 2 part 2
// Используйте директивы OpenMP для распараллеливания внешних циклов. Протестируйте производительность каждой сортировки на
// массивах разного размера (например, 1000, 10,000 и 100,000 элементов).

#include <iostream>      // Для работы с вводом/выводом (cout, cin, endl)
#include <vector>        // Для использования динамического массива vector
#include <algorithm>     // Для функции swap
#include <omp.h>         // Для параллельных вычислений OpenMP
#include <chrono>        // Для измерения времени выполнения
#include <random>        // Для генерации случайных чисел (random_device, mt19937, uniform_int_distribution) 

using namespace std;     // Стандартное пространство имён, чтобы не писать std:: перед cout, endl и т.д.   

// Пузырьком с OpenMP (BUBBLE SORT)
void bubbleSortParallel(vector<int>& arr) {    // Функция принимает массив по ссылке, чтобы менять его прямо
    int n = arr.size();                        // Определяем размер массива
    for (int i = 0; i < n - 1; i++) {          // Внешний цикл: количество проходов по массиву
                                               // Параллельное выполнение внутреннего цикла
        #pragma omp parallel for               // Директива OpenMP: распределяем цикл по потокам
        for (int j = 0; j < n - i - 1; j++) {  // Внутренний цикл сравнения соседних элементов
            if (arr[j] > arr[j + 1]) {         // Если текущий элемент больше следующего
                swap(arr[j], arr[j + 1]);      // Меняем их местами с помощью встроенной функции swap
            }
        }
    }
}

// Сортировка выбором с OpenMP (SELECTION SORT)
void selectionSortParallel(vector<int>& arr) {  // Функция принимает массив по ссылке, чтобы менять его прямо
    int n = arr.size();                         // Определяем размер массива
    for (int i = 0; i < n - 1; i++) {           // Внешний цикл: выбираем позицию для минимального элемента
        int minIndex = i;                       // Изначально считаем минимальным текущий элемент

        #pragma omp parallel                    // Создаём параллельный регион
        {
            int localMin = minIndex;            // Каждый поток имеет свою копию локального минимума

            #pragma omp for nowait              // Распределяем цикл поиска минимума по потокам без ожидания
            for (int j = i + 1; j < n; j++) {   // Ищем минимальный элемент в оставшейся части массива
                if (arr[j] < arr[localMin]) {   // Если нашли элемент меньше локального минимума
                    localMin = j;               // Обновляем локальный минимум
                }
            }

            #pragma omp critical                // Критическая секция: только один поток обновляет глобальный минимум
            {
                if (arr[localMin] < arr[minIndex]) {
                    minIndex = localMin;        // Обновляем глобальный индекс минимального элемента
                }
            }
        }
        swap(arr[i], arr[minIndex]);            // Меняем текущий элемент с найденным минимальным
    }
}


// Сортировка вставкой (последовательная, так как трудно распараллелить) INSERTION SORT
void insertionSort(vector<int>& arr) {          // Функция принимает массив по ссылке, чтобы менять его прямо
    int n = arr.size();                         // Определяем размер массива
    for (int i = 1; i < n; i++) {               // Проходим по массиву начиная со второго элемента
        int key = arr[i];                       // Сохраняем текущий элемент
        int j = i - 1;                          // Начинаем проверку отсортированной части слева

        while (j >= 0 && arr[j] > key) {        // Сдвигаем элементы, большие key, вправо
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;                       // Вставляем key на правильное место
    }
}

// Настройка генератора случайных чисел
vector<int> generateRandomArray(int size) {
    vector<int> arr(size);

    random_device rd;                             // random_device используется как источник случайности
    mt19937 gen(rd());                            // mt19937 — генератор случайных чисел Mersenne Twister
                                                  // Инициализируем его значением из rd(), чтобы последовательность была случайной
    uniform_int_distribution<int> dist(0, 99999); // uniform_int_distribution задаёт равномерное распределение чисел от 0 до 999999
                                                  // dist(gen) будет возвращать случайное число из этого диапазона

    // Заполнение массива случайными числами
    for (int i = 0; i < size; i++) {
        arr[i] = dist(gen);                       // Для каждого индекса i от 0 до SIZE-1 генерируем случайное число dist(gen) и записываем его в массив
    }
    return arr;
}



int main() {                          // Основная функция
    vector<int> sizes = {1000, 10000, 100000};               // Размеры массивов для тестирования

    for (int size : sizes) {                                 // Для каждого размера массива
        cout << "Массив размера: " << size << endl;

        vector<int> data = generateRandomArray(size);        // Генерируем случайный массив

        // BUBBLE SORT
        vector<int> bubbleArr = data;                        // Создаём копию массива для пузырька
        auto start = chrono::high_resolution_clock::now();   // Начала замера времени
        bubbleSortParallel(bubbleArr);                       // Вызываем параллельную сортировку пузырьком
        auto end = chrono::high_resolution_clock::now();     // Конец замера времени
        chrono::duration<double> duration = end - start;     // Вычисляем длительность
        cout << "Bubble Sort Parallel: " << duration.count() << " s" << endl; // Выводим время

        // SELECTION SORT
        vector<int> selectionArr = data;                     // Копия массива для сортировки выбором
        start = chrono::high_resolution_clock::now();        // Начала замера времени
        selectionSortParallel(selectionArr);                 // Параллельная сортировка выбором
        end = chrono::high_resolution_clock::now();          // Конец замера времени
        duration = end - start;                              // Вычисляем длительность
        cout << "Selection Sort Parallel: " << duration.count() << " s" << endl;

        // INSERTION SORT (последовательная) 
        vector<int> insertionArr = data;                     // Копия массива для сортировки вставкой
        start = chrono::high_resolution_clock::now();        // Начала замера времени
        insertionSort(insertionArr);                         // Последовательная сортировка вставкой
        end = chrono::high_resolution_clock::now();          // Конец замера времени
        duration = end - start;                              // Вычисляем длительность
        cout << "Insertion Sort: " << duration.count() << " s" << endl;
    }

    return 0;                                                // Завершаем программу
}
