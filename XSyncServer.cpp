//
// Created by xbot on 1/26/25.
//

#include <zmq.h>
#include <cstdio>
#include <cstring>
#include <ctime>

long get_current_time_ms()
{
    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

[[noreturn]] int main()
{
    const int hwm = 1; // Allow only 1 message in each client's queue
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_REP);
    zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_bind(socket, "tcp://*:3123");

    printf("\033[34m");
    printf(
        ">>==============================================================<<\n"
        "||                                                              ||\n"
        "||  __  ________   ___   _  ____           _  _    ___   ___    ||\n"
        "||  \\ \\/ / ___\\ \\ / / \\ | |/ ___|         | || |  ( _ ) ( _ )   ||\n"
        "||   \\  /\\___ \\\\ V /|  \\| | |      _____  | || |_ / _ \\ / _ \\   ||\n"
        "||   /  \\ ___) || | | |\\  | |___  |_____| |__   _| (_) | (_) |  ||\n"
        "||  /_/\\_\\____/ |_| |_| \\_|\\____|            |_|  \\___/ \\___/   ||\n"
        "||                                                              ||\n"
        "||   _____ _   _ _____   __  __    _  _____ ____  _____  __     ||\n"
        "||  |_   _| | | | ____| |  \\/  |  / \\|_   _|  _ \\|_ _\\ \\/ /     ||\n"
        "||    | | | |_| |  _|   | |\\/| | / _ \\ | | | |_) || | \\  /      ||\n"
        "||    | | |  _  | |___  | |  | |/ ___ \\| | |  _ < | | /  \\      ||\n"
        "||    |_| |_| |_|_____| |_|  |_/_/   \\_\\_| |_| \\_\\___/_/\\_\\     ||\n"
        "||                                                              ||\n"
        ">>==============================================================<<\n"
    );
    while (true)
    {
        char buffer[10];
        zmq_recv(socket, buffer, 10, 0); // Receive request
        const long t2 = get_current_time_ms(); // Record t2 (server receives request)
        char response[50];
        snprintf(response, sizeof(response), "%ld", t2);

        zmq_send(socket, response, strlen(response), 0);
    }
}
