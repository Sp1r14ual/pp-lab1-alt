#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <chrono>
using namespace std;

struct thread_data
{
    int index;
    time_t time;
};

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    thread_data *params = (thread_data *)arg;
    pthread_attr_t thread_attr;
    pthread_getattr_np(pthread_self(), &thread_attr);
    int detach;
    pthread_attr_getdetachstate(&thread_attr, &detach);
    size_t guard;
    pthread_attr_getguardsize(&thread_attr, &guard);
    size_t stack;
    pthread_attr_getstacksize(&thread_attr, &stack);
    string output = "Thread " + to_string(params->index) + " is running.\n";
    output += "Create time: ";
    output.append(ctime(&params->time));
    output += "ID: " + to_string(pthread_self()) + "\n";
    output += "Detach state: " + to_string(detach) + "\n";
    output += "Guard size: " + to_string(guard) + "\n";
    output += "Stack size: " + to_string(stack) + "\n\n";
    cout << output;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int N = 1; // Количество потоков по умолчанию.
    // Проверка коараметр
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
    // Определяем переменные: идентификатор потока, аттрибуты и код ошибки
    pthread_t threads[N];
    pthread_attr_t thread_attr[N];
    thread_data data[N];
    int err;
    // Создаём потоки.личества переданных параметров.
    if (argc > 2)
    {
        cout << "Too many arguments." << endl;
        exit(-1);
    }
    // Если передан п
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
        const auto now = chrono::system_clock::now();
        const time_t t_c = chrono::system_clock::to_time_t(now);
        // Устанавливаем параметры потока.
        data[i] = {
            i,
            t_c};
        // Создаём поток.
        err = pthread_create(&threads[i], &thread_attr[i], thread_job, (void *)&data[i]);
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
        // Освобождаем память, занятую под хранение атрибутов потока
        pthread_attr_destroy(&thread_attr[i]);
    }
    pthread_exit(NULL);
}
