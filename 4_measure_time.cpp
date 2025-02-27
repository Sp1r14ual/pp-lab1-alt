#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <chrono>
using namespace std;

const int asize = 1000000;
int array[asize] = {};

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    for (int i = 1; i < asize; i += 2)
    {
        array[i] += 1;
    }
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    int N = 1; // Количество потоков по умолчанию.
    // Проверка количества переданных параметров.
    if (argc > 2)
    {
        cout << "Too many arguments." << endl;
        exit(-1);
    }
    // Если передан параметр
    if (argc == 2)
    {
        // Считываем заданное количество потоков.
        N = atoi(argv[1]);
        // Проверка корректности параметра.
        if (N < 1)
        {
            cout << "Invalid threads number." << endl;
            exit(-1);
        }
    }
    // Определяем переменные: идентификатор потока и код ошибки
    pthread_t threads[N];
    int err;
    // Определяем время выполнения операций одним потоком:
    auto begin = chrono::steady_clock::now();
    for (int i = 0; i < asize; i += 1)
    {
        array[i] += 1;
    }
    auto end = chrono::steady_clock::now();
    cout << asize << " operations." << endl;
    auto time = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "One thread: " << time << endl;
    // Создаём поток, замеряем время создания и параллельного выполнения операций
    begin = chrono::steady_clock::now();
    err = pthread_create(&threads[0], NULL, thread_job, NULL);
    if (err != 0)
    {
        cout << "Cannot create a thread: " << strerror(err) << endl;
        exit(-1);
    }
    auto end_create = chrono::steady_clock::now();
    for (int i = 0; i < asize; i += 2)
    {
        array[i] += 1;
    }
    // Ожидаем завершение работы потока
    pthread_join(threads[0], NULL);
    end = chrono::steady_clock::now();
    time = chrono::duration_cast<chrono::microseconds>(end_create - begin).count();
    cout << "Thread creation: " << time << endl;
    time = chrono::duration_cast<chrono::microseconds>(end - begin).count();
    cout << "Two threads: " << time << endl;
    pthread_exit(NULL);
}
