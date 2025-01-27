//
// Created by xbot on 1/26/25.
//
#include <zmq.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sys/time.h>
#include <thread>
#include <atomic>
#include <unistd.h>

// Global atomic variable for the current offset
std::atomic<long> current_offset_ms(0);

long get_current_time_ms() {
    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void smooth_time_adjuster() {
    const long step_size_ms = 10;       // Adjust in 10ms increments
    const long adjustment_interval_ms = 50; // Interval between adjustments (50ms)

    while (true) {
        long offset = current_offset_ms.load(); // Read the current offset atomically
        if (offset != 0) {
            long adjustment = (offset > 0) ? step_size_ms : -step_size_ms;

            // If the offset is smaller than the step size, apply the remaining offset
            if (std::abs(offset) < step_size_ms) {
                adjustment = offset;
            }

            // Print the adjustment
            printf("Adjusting system clock by %ld ms. Remaining offset: %ld ms\n", adjustment, offset);

            // Apply the adjustment
            struct timeval tv{};
            gettimeofday(&tv, NULL);
            long adjusted_time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000 + adjustment;
            tv.tv_sec = adjusted_time_ms / 1000;
            tv.tv_usec = (adjusted_time_ms % 1000) * 1000;
            settimeofday(&tv, NULL);

            // Update the remaining offset atomically
        }

        // Sleep for the adjustment interval
        usleep(adjustment_interval_ms * 1000);
    }
}


[[noreturn]] int main(int argc, char *argv[]) {
    std::string ip = "10.4.88.2"; // Default RIO IP
    if (argc == 2) {
        ip = argv[1];
    }

    // Start the smooth time adjuster in a background thread
    std::thread adjuster_thread(smooth_time_adjuster);
    adjuster_thread.detach(); // Detach the thread to let it run in the background

    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);

    const std::string address = "tcp://" + ip + ":3123";
    if (zmq_connect(socket, address.c_str()) == 0) {
        printf("Connected to %s\n", address.c_str());
    } else {
        printf("Failed to connect to %s\n", address.c_str());
        return 1;
    }

    while (true) {
        const long t1 = get_current_time_ms(); // Record t1 (client sends request)
        zmq_send(socket, "REQ", 3, 0);

        // Wait for response
        char buffer[50];
        zmq_recv(socket, buffer, 50, 0);
        const long t4 = get_current_time_ms(); // Record t4 (client receives response)
        const long t2 = atol(buffer);          // Record t2 (Server's Time)

        printf("Times:\n");
        printf("t1 (Request sent): %ld ms\n", t1);
        printf("t2 (Server received request): %ld ms\n", t2);
        printf("t4 (Response received): %ld ms\n", t4);

        const long offset = t2 - ((t1 + t4) / 2);
        printf("Calculated offset: %ld ms\n", offset);

        // Update the global offset
        current_offset_ms.store(offset);

        usleep(50000); // 50ms delay
    }
}
