#include <stdio.h> //Adding the required libraries
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int x = 0; // Initialization of variable for computational functions
int sum = 0;

int conv()
{ // Function to calculate a random number
    int random = rand() % 100000 + 1;
    return random;
}

void writeTolog(int value)
{ // Function to open a text document and write a number there
    FILE *file = fopen("werte.txt", "a");
    fprintf(file, "%d\n", value);
    fclose(file);
}

double stat(int value)
{ // Function Calculation of the sum of numbers and the arithmetic mean
    x++;
    sum += value;
    double middleValue = (double)sum / x;
    return middleValue;
}

void report(double value)
{ // Function of the numbers output in the console
    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", value);
}

void terminate()
{ // Function of the Programm stopage
    printf("\nCtrl+C pressed.Program terminated by user.\n");
    exit(0);
}

void pipeFunc()
{ // Function for implementing data transfer using Pipes

    int conv_to_log[2]; // Creating an array to store data
    pipe(conv_to_log);  // Creating of Pipe Chanel to exchange Data from conv to log

    int log_to_stat[2]; // Creating an array to store data
    pipe(log_to_stat);  // Creating of Pipe Chanel to exchange Data from log to stat

    pid_t convID; // Generating a personal ID for each function to control processes
    pid_t logID;
    pid_t statID;

    convID = fork(); // Separation of convID into two parallel processes with own generic IDs
    if (convID == 0)
    { // To check if the code is in a child process
        // Conv Prozessing
        close(conv_to_log[0]); // Closing the conv to log chanel for reading the information
        close(log_to_stat[0]); // Closing the log_to_stat chanel for reading and writing the information
        close(log_to_stat[1]);

        while (1)
        { // Loop for writing the values from the conv function to pipe conv_to_log
            int random = conv();
            write(conv_to_log[1], &random, sizeof(int)); // Writing the random number in pipe
            sleep(1);                                    // Pausing the loop, so that it wouldnot be too fast
        }

        close(conv_to_log[1]); // Closing the Pipe after code stopage
    }
    else if (convID > 0)
    {                   // To check if the code is in a parent process
        logID = fork(); // Separation of logID after convID into two parallel processes with own generic IDs
        if (logID == 0)
        { // To check if the code is in a child process
            // Log Prozessing
            close(conv_to_log[1]); // Closing the conv_to_log chanel for writing the information
            close(log_to_stat[0]); // Closing the log_to_stat chanel for reading the information

            while (1)
            {                                               // Loop for reading the Data from conv_to_log, saving them in file and writing in pipe log_to_stat
                int value;                                  // Inizialisation the value that will read the numbers from pipe
                read(conv_to_log[0], &value, sizeof(int));  // Reading the value from conv_to_log pipe
                writeTolog(value);                          // Log function with value processing
                write(log_to_stat[1], &value, sizeof(int)); // Writing the value to log_to_stat pipe
            }

            close(conv_to_log[0]); // Closing the conv_to_log and log_to_stat chanels for reading und writing
            close(log_to_stat[1]);
        }
        else if (logID > 0)
        {                    // To check if the code is in a parent process
            statID = fork(); // Separation of statID after logID into two parallel processes with own generic IDs
            if (statID == 0)
            { // To check if the code is in a child process
                // Stat Prozessing
                close(conv_to_log[0]); // Closing the conv_to_log chanel for reading the information
                close(conv_to_log[1]); // Closing the conv_to_log chanel for writing the information
                close(log_to_stat[1]); // Closing the log_to_stat chanel for writing the information

                while (1)
                {                                              // Loop for reading the Data from log_to_stat in Value variable, prozessing the stat Function and prozessing the report function with numbers from stat
                    int value;                                 // Inizialisation the value that will read the numbers from pipe
                    read(log_to_stat[0], &value, sizeof(int)); // Reading the numbers from log_to_stat
                    double middleValue = stat(value);          // Processin the stat with value numbers
                    report(middleValue);                       // Prozessing the report after stat
                }

                close(log_to_stat[0]); // Closing the log_to_stat chanel for reading the information
            }
            else if (statID > 0)
            { // To check if the code is in a parent process
                // Elternprozess
                close(conv_to_log[0]); // Closing the conv_to_log chanel for reading the information
                close(conv_to_log[1]); // Closing the conv_to_log chanel for writing the information
                close(log_to_stat[0]); // Closing the log_to_stat chanel for reading the information
                close(log_to_stat[1]); // Closing the log_to_stat chanel for writing the information

                waitpid(convID, NULL, 0); // Waiting for the Ending every Prozess
                waitpid(logID, NULL, 0);
                waitpid(statID, NULL, 0);
            }
        }
    }
}
int main()
{
    srand(time(NULL));

    signal(SIGINT, terminate);
    pipeFunc();

    return 0;
}