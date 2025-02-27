#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
using namespace std;

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    cout << "Thread " << pthread_self() << " is running..." << endl;
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
    pthread_attr_t thread_attr[N];
    int err;
    // Создаём потоки
    // Создаём потоки.
    for (int i = 0; i < N; i++)
    {
        // Инициализируем переменную для хранения атрибутов потока
        err = pthread_attr_init(&thread_attr[i]);
        if (err != 0)
        {
            cout << "Cannot create thread attribute: " << strerror(err) << endl;
            exit(-1);
        }
        // Устанавливаем минимальный размер стека для потока (в байтах)
        err = pthread_attr_setstacksize(&thread_attr[i], 5 * 1024 * 1024);
        if (err != 0)
        {
            cout << "Setting stack size attribute failed: " << strerror(err)
                 << endl;
            exit(-1);
        }
        // Создаём поток.
        err = pthread_create(&threads[i], &thread_attr[i], thread_job, NULL);
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
        // Освобождаем память, занятую под хранение атрибутов потока
        pthread_attr_destroy(&thread_attr[i]);
    }

    // Ожидаем завершения созданного потока перед завершением
    // работы программы
    pthread_exit(NULL);
}
