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
    int err;
    // Создаём потоки
    for (int i = 0; i < N; i++)
    {
        err = pthread_create(&threads[i], NULL, thread_job, NULL);
        // Если при создании потока произошла ошибка, выводим
        // сообщение об ошибке и прекращаем работу программы
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            exit(-1);
        }
    }
    // Ожидаем завершения созданного потока перед завершением
    // работы программы
    pthread_exit(NULL);
}
