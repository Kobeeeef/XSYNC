//
// Created by xbot on 1/26/25.
//
#include <zmq.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sys/time.h>
#include <unistd.h>

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

void *create_socket(void *context, const std::string &address, const int hwm, const int timeout) {
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_setsockopt(socket, ZMQ_SNDHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(socket, ZMQ_RCVHWM, &hwm, sizeof(hwm));
    zmq_setsockopt(socket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout)); // Send timeout
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &timeout, sizeof(timeout)); // Receive timeout

    if (zmq_connect(socket, address.c_str()) == 0) {
        printf("Connected to %s\n", address.c_str());
    } else {
        printf("Failed to connect to %s\n", address.c_str());
    }

    return socket;
}

[[noreturn]] int main(int argc, char *argv[]) {
    std::string ip = "10.4.88.2"; // Default RIO IP
    if (argc == 2) {
        ip = argv[1];
    }

    const int hwm = 1;
    const int timeout = 1000; // Timeout in milliseconds (1 second)
    const std::string address = "tcp://" + ip + ":3123";

    void *context = zmq_ctx_new();
    void *socket = create_socket(context, address, hwm, timeout);

    while (true) {
        const long t1 = get_current_time_ms(); // Record t1 (client sends request)
        if (zmq_send(socket, "REQ", 3, 0) == -1) {
            perror("Failed to send request. Reconnecting...");
            zmq_close(socket);
            socket = create_socket(context, address, hwm, timeout);
            continue;
        }

        // Wait for response
        char buffer[50];
        if (zmq_recv(socket, buffer, sizeof(buffer), 0) == -1) {
            perror("Failed to receive response. Reconnecting...");
            zmq_close(socket);
            socket = create_socket(context, address, hwm, timeout);
            continue;
        }

        const long t4 = get_current_time_ms(); // Record t4 (client receives response)
        const long t2 = atol(buffer);          // Record t2 (Server's Time)
        const long offset = t2 - ((t1 + t4) / 2);

        if (offset == 0) {
            printf("System already synchronized!\n");
            continue;
        }

        printf("Times:\n");
        printf("t1 (Request sent): %ld ms\n", t1);
        printf("t2 (Server received request): %ld ms\n", t2);
        printf("t4 (Response received): %ld ms\n", t4);
        printf("Calculated offset: %ld ms\n", offset);

        if (set_system_time(t4 + offset) == 0) {
            printf("System time updated successfully.\n");
        } else {
            printf("Failed to update system time.\n");
        }

        usleep(500);
    }
}
