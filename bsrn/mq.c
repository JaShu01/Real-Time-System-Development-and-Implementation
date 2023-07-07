// The header files for the use of functions and structures
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// The first constant is used as the key for message queue creation. The second one is for the maximum size of a message.
#define msg_queue_key 123456
#define maximum_message_size 1048

// A structure to represent the message in the Message Queue.
struct message {
    long message_type;
    int value;
};

// msg_number: for counting the number of received messages. total: for calculating the total received values.
int msg_number = 0;
int total = 0;

// Function conv() generate and returns a random number between 1 and 100,000.
int conv() {
    int random_number = rand() % 100000 + 1;
    return random_number;
}

// Function log_value() is to write the passed value into a file named "values.txt".
void log_value(int value) {
    FILE* file = fopen("werte.txt", "a");
    fprintf(file, "%d\n", value);
    fclose(file);
}

// Function stat() uses the passed value to calculate the average of the received values.
double stat(int value) {
    msg_number++;
    total += value;
    double average = (double)total / msg_number;
    return average;
}

// Function report() defines the outputs of the current total and average of the received values on the console
void report() {
    printf("Total: %d\n", total);
    printf("Average: %.2f\n", (double)total / msg_number);
}

// Function terminate() is a Signal handler for SIGINT (Ctrl+C) termination and outputs a message and terminates the program.
void terminate(int signal_number) {
    printf("\nCtrl+C pressed.Program terminate by user.\n");
    exit(0);
}

/* Function consumer() is a consumer process that generates a key, creates a queue, and starts an infinite loop. During each iteration,
 it receives messages, stores values, calculates the average, and outputs a report. */
void consumer() {
    key_t key = ftok(".", msg_queue_key);
    int consumer_queue = msgget(key, 0666 | IPC_CREAT);
    if (consumer_queue == -1) {
        printf("Error creating consumer message queue.\n");
        return;
    }

    struct message msg;
    while (1) {
        if (msgrcv(consumer_queue, &msg, sizeof(struct message) - sizeof(long), 1, 0) == -1) {
            printf("Error receiving message from producer.\n");
            break;
        }

        int value = msg.value;
        log_value(value);
        stat(value);
        report();
    }

    msgctl(consumer_queue, IPC_RMID, NULL);
}

/* Function producer() is a producer process that have the same functions as consumer process but it generates a random numbers,
 puts the value into a message and sends messages instead of receiving. Then waits one second before sending the next message. */
void producer() {
    key_t key = ftok(".", msg_queue_key);
    int producer_queue = msgget(key, 0666 | IPC_CREAT);
    if (producer_queue == -1) {
        printf("Error creating producer message queue.\n");
        return;
    }

    struct message msg;
    while (1) {
        int random = conv();
        msg.value = random;
        msg.message_type = 1;

        if (msgsnd(producer_queue, &msg, sizeof(struct message) - sizeof(long), 0) == -1) {
            printf("Error sending message to consumer.\n");
            break;
        }

        sleep(1);
    }

    msgctl(producer_queue, IPC_RMID, NULL);
}

// main() initializes the random number generator with a seed from the current time.
int main() {
    srand(time(NULL));

  // The registered signal SIGINT to terminate the program properly when Ctrl+C is pressed.
    signal(SIGINT, terminate);

    int consumer_pid = fork();

    if (consumer_pid == 0) {
        // Child process: consumer functionality
        consumer();
    } else if (consumer_pid > 0) {
        // Parent process: producer functionality
        producer();
    } else {
        printf("Fork failed.\n");
        return 1;
    }

    return 0;
}