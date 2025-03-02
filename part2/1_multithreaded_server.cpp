#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <array>

using namespace std;

char response[] = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
                  "<style>body { background-color: #111 }"
                  "h1 { font-size:4cm; text-align: center; color: black;"
                  " text-shadow: 0 0 2mm red}</style></head>"
                  "<body><h1>Goodbye, world!</h1></body></html>\r\n";

struct thread_data
{
    int client_fd;
};

// void *thread_job(void *arg)
// {
//     thread_data *par = (struct thread_data *)arg;
//     int client_fd = par->client_fd;
//     write(client_fd, response, strlen(response) * sizeof(char));
//     close(client_fd);
//     return NULL;
// }

// void *thread_job(void *arg)
// {
//     thread_data *par = (thread_data *)arg;
//     int client_fd = par->client_fd;

//     write(client_fd, response, strlen(response) * sizeof(char));

//     shutdown(client_fd, SHUT_WR);
//     close(client_fd);

//     delete par;
//     return NULL;
// }

void *thread_job(void *arg)
{
    thread_data *par = (struct thread_data *)arg;
    int client_fd = par->client_fd;

    std::array<char, 1024> request_buffer{};

    ssize_t bytes_received = recv(par->client_fd, request_buffer.data(),
                                  request_buffer.size() - 1, 0);
    if (bytes_received < 0)
    {
        throw std::runtime_error("Failed to receive client request: " +
                                 std::string(strerror(errno)));
    }

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Connection: close\r\n"
        "Content-Length: ";

    std::string body =
        "<!DOCTYPE html>"
        "<html><head><title>Bye-bye baby bye-bye</title></head>"
        "<body><h1>Goodbye, world!</p>"
        "</body></html>\r\n";

    response += std::to_string(body.length()) + "\r\n\r\n" + body;

    ssize_t bytes_sent = send(par->client_fd, response.c_str(),
                              response.length(), 0);
    if (bytes_sent < 0)
    {
        throw std::runtime_error("Failed to send response: " +
                                 std::string(strerror(errno)));
    }

    shutdown(par->client_fd, SHUT_RDWR);

    close(client_fd);
    return NULL;
}

int main()
{
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    int port = 8080;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
    {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    int err;
    pthread_attr_t thread_attr;
    err = pthread_attr_init(&thread_attr);
    if (err != 0)
    {
        perror("Cannot init attr for thread");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (err != 0)
    {
        perror("Cannot create deteched thread");
        exit(-1);
    }

    err = pthread_attr_setstacksize(&thread_attr, 0.5 * 1024 * 1024);
    if (err != 0)
    {
        perror("Cannot create stack for thread");
        exit(-1);
    }

    // while (1)
    // {
    //     client_fd = accept(sock, (struct sockaddr *)&cli_addr, &sin_len);
    //     printf("got connection\n");

    //     if (client_fd == -1)
    //     {
    //         perror("Can't accept");
    //         continue;
    //     }

    //     // Устанавливаем параметры потока.
    //     pthread_t thread;
    //     thread_data *data = new thread_data{client_fd};
    //     // Создаём поток.
    //     int err = pthread_create(&thread, NULL, thread_job, data);
    //     if (err != 0)
    //     {
    //         cout << "Cannot create a thread: " << strerror(err) << endl;
    //         close(client_fd);
    //         delete data;
    //         continue;
    //     }

    //     pthread_detach(thread);
    //     pthread_attr_destroy(&thread_attr);
    // }

    while (1)
    {
        client_fd = accept(sock, (sockaddr *)&cli_addr, &sin_len);
        if (client_fd == -1)
        {
            perror("Can't accept");
            continue;
        }

        thread_data *data = new thread_data{client_fd};
        pthread_t thread;
        int err = pthread_create(&thread, &thread_attr, thread_job, data);

        if (err != 0)
        {
            cerr << "Thread creation error: " << strerror(err) << endl;
            shutdown(client_fd, SHUT_WR);
            close(client_fd);
            delete data;
        }
    }

    // pthread_exit(NULL);
    return 0;
}
