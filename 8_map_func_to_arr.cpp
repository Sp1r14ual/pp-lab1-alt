#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <chrono>
#include <vector>
using namespace std;

int N = 1;          // Количество потоков по умолчанию.
int asize = 100000; // Размер массива по умолчанию.
int add = 1;        // Число повторений операции по умолчанию.
vector<double> array;
vector<int64_t> thread_time;
struct thread_data
{
    int index;
};

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    auto begin = chrono::steady_clock::now();
    thread_data *params = (thread_data *)arg;
    int index = params->index;
    for (int i = index; i < asize; i += N)
    {
        for (int j = 0; j < add; j++)
            array[i] += 1;
    }
    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    thread_time[index] = time;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Проверка количества переданных параметров.
    if (argc > 4)
    {
        cout << "Too many arguments." << endl;
        exit(-1);
    }
    // Если передан параметр
    if (argc >= 2)
    {
        // Считываем заданное количество потоков.
        N = atoi(argv[1]);
        // Проверка корректности параметра.
        if (N < 1)
        {
            cout << "Invalid threads number." << endl;
            exit(-1);
        }
        if (argc >= 3)
        {
            asize = atoi(argv[2]);
            if (asize < 1)
            {
                cout << "Invalid array size." << endl;
                exit(-1);
            }
            if (argc == 4)
            {
                add = atof(argv[3]);
                if (add < 1)
                {
                    cout << "Invalid add times." << endl;
                    exit(-1);
                }
            }
        }
    }
    // Задаём рабочий массив.
    array.resize(asize, 0);
    thread_time.resize(N, 0);
    // Определяем переменные: идентификатор потока, аттрибуты и код ошибки
    pthread_t threads[N];
    thread_data data[N];
    int err;
    cout << asize << " elements, " << add << " operations, " << N << " threads." << endl;
    auto begin = chrono::steady_clock::now();
    // Создаём потоки.
    for (int i = 0; i < N; i++)
    {
        // Устанавливаем параметры потока.
        data[i] = {
            i};
        // Создаём поток.
        err = pthread_create(&threads[i], NULL, thread_job, (void *)&data[i]);
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
    }
    // Ожидаем завершения потоков.
    for (int i = 0; i < N; i++)
    {
        err = pthread_join(threads[i], NULL);
    }
    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "Work time: " << time << endl;
    double total = 0;
    for (auto n : thread_time)
    {
        total += n;
    }
    cout << "Total thread time: " << total << endl;
    cout << "Average thread time: " << total / N << endl;
    // Работа одного потока:
    begin = chrono::steady_clock::now();
    for (int i = 0; i < asize; i++)
    {
        for (int j = 0; j < add; j++)
            array[i] += 1;
    }
    end = chrono::steady_clock::now();
    double solo_time = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "One thread work time: " << solo_time << endl;
    cout << "Acceleration: " << solo_time / time << endl;
    cout << "Efficiency: " << solo_time / time / N << endl;
    pthread_exit(NULL);
}
