#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE 1024

int x = 0;
int sum = 0;

int conv() {
    int random = rand() % 100000 + 1;
    return random;
}

void writeToLog(int value) {
    FILE* file = fopen("werte.txt", "a");
    fprintf(file, "%d\n", value);
    fclose(file);
}

double stat(int value) {
    x++;
    sum += value;
    double middleValue = (double)sum / x;
    return middleValue;
}

void report(double value) {
    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", value);
}

void terminate(int signum) {
    printf("\nCtrl+C pressed. Program terminated by user.\n");
    exit(0);
}

void child_process(int* shm_ptr, int sem_id) {
    while (1) {
        int random = conv();

        struct sembuf sem_wait = {0, -1, 0}; // Wait operation on semaphore
        struct sembuf sem_signal = {0, 1, 0}; // Signal operation on semaphore

        // Wait for access to shared memory
        semop(sem_id, &sem_wait, 1);

	// Write random number to shared memory
        *shm_ptr = random;

	// Write random number to log file
        writeToLog(random);
	
	// Calculate and report middle value
        double middleValue = stat(random);
        report(middleValue);

        // Signal release of shared memory
        semop(sem_id, &sem_signal, 1);

        sleep(1);
    }
}

void parent_process(int* shm_ptr, int sem_id) {
    while (1) {
        double middleValue;

        struct sembuf sem_wait = {0, -1, 0}; // Wait operation on semaphore
        struct sembuf sem_signal = {0, 1, 0}; // Signal operation on semaphore

        // Wait for access to shared memory
        semop(sem_id, &sem_wait, 1);

	// Read value from shared memory and calculate middle value
        middleValue = stat(*shm_ptr);

	// Report middle value
        report(middleValue);

        // Signal release of shared memory
        semop(sem_id, &sem_signal, 1);

        sleep(1);
    }
}

int main() {
    srand(time(NULL));

    signal(SIGINT, terminate);

    // Create shared memory segment
    key_t shm_key = ftok(".", 'x');

    int shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        printf("Error creating shared memory segment.\n");
        return 1;
    }

    // Attach shared memory segment
    int* shm_ptr = (int*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (int*)-1) {
        printf("Error attaching shared memory segment.\n");
        return 1;
    }

    // Create semaphore
    key_t sem_key = ftok(".", 's');
    int sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        printf("Error creating semaphore.\n");
        return 1;
    }

    // Initialize semaphore value to 1
    semctl(sem_id, 0, SETVAL, 1);

    int pid = fork();

    if (pid < 0) {
        printf("Fork failed.\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        child_process(shm_ptr, sem_id);
    } else {
        // Parent process
        parent_process(shm_ptr, sem_id);
    }

    // Detach shared memory segment
    shmdt(shm_ptr);

    // Remove shared memory segment
    shmctl(shm_id, IPC_RMID, NULL);

    // Remove semaphore
    semctl(sem_id, 0, IPC_RMID);

    return 0;
}