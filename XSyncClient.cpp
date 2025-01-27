//
// Created by xbot on 1/26/25.
//
#include <zmq.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
long get_current_time_ms() {
    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int set_system_time(long server_time_ms) {
    struct timeval tv{};
    tv.tv_sec = server_time_ms / 1000;
    tv.tv_usec = (server_time_ms % 1000) * 1000;

    // Set the system time
    if (settimeofday(&tv, NULL) < 0) {
        perror("Failed to set system time");
        return -1;
    }
    return 0;
}

[[noreturn]] int main() {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://10.0.0.106:3123");

    printf("Christian Algorithm Time Client started...\n");
    while (1) {
        const long t1 = get_current_time_ms(); // Record t1 (client sends request)
        zmq_send(socket, "REQ", 3, 0);

        // Wait for response
        char buffer[50];
        zmq_recv(socket, buffer, 50, 0);
        const long t4 = get_current_time_ms(); // Record t4 (client receives response)
        const long t2 = atol(buffer); // Record t2 (Servers Time)

        printf("Times:\n");
        printf("t1 (Request sent): %ld ms\n", t1);
        printf("t2 (Server received request): %ld ms\n", t2);
        printf("t4 (Response received): %ld ms\n", t4);
        const long offset = t2 - ((t1 + t4) / 2);
        printf("Calculated offset: %ld ms\n", offset);
        if (set_system_time(t4 + offset) == 0) {
            printf("System time updated successfully.\n");
        } else {
            printf("Failed to update system time.\n");
        }
        usleep(50000);
    }
}
