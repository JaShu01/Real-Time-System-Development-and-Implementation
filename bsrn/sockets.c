// title:        sockets.c
// description:  Server-client application in C
// author:       Diego Gutiérrez
// date:         25.06.2023
// compile with: gcc sockets.c -o sockets
// nodes:        In this program a child process handles the server-client interaction
//               and the parent process writes random numbers to a file.  It calculates
//               an average value from random numbers and communicates this value between
//               the server and client while displaying the sum and average value on the screen.

// Libraries used in the program
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Global variables for the following 4 functions
int x = 0;
int sum = 0;

// Random number generator
int conv()
{
    int random = rand() % 100000 + 1;
    return random;
}

// Open the text file and write the values in it
void writeToLog(int value)
{
    FILE *file = fopen("werte.txt", "a");
    fprintf(file, "%d\n", value);
    fclose(file);
}

// Calculate sum and average of the generated values
double stat(int value)
{
    x++;
    sum += value;
    double average = (double)sum / x;
    return average;
}

// Prints sum and average in the console
void report(double value)
{
    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", value);
}

// Terminate the program when the user clicks Ctrl+C
void terminate()
{
    printf("\nCtrl+C pressed. Program terminated by user.\n");
    exit(0);
}

void server_client_func()
{
    int server_socket, client_socket;                  // Define server and client descriptors
    struct sockaddr_in server_address, client_address; // Define server and client structures
    int port_number = 12345;                           // Define the port number for the communication
    socklen_t client_address_len;

    // Create server socket and check for error
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Error creating server socket.\n");
        return;
    }
    // Define server address parameters
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to the defined adress and check for error
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Error binding server socket.\n");
        return;
    }
    // Listen on server socket and check for error
    if (listen(server_socket, 5) < 0)
    {
        printf("Error listening on server socket.\n");
        return;
    }
    // Accept client connection
    client_address_len = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket < 0)
    {
        printf("Error accepting client connection.\n");
        return;
    }
    // Close connection
    close(server_socket);

    while (1)
    {
        // Read from client socket
        int value;
        if (read(client_socket, &value, sizeof(int)) < 0)
        {
            printf("Error reading from client socket.\n");
            break;
        }

        writeToLog(value);            // Write recieved value to log
        double average = stat(value); // Calculate the average value

        // Write the average value back to the client socket
        if (write(client_socket, &average, sizeof(double)) < 0)
        {
            printf("Error writing to client socket.\n");
            break;
        }

        printf("Received average value: %.2f\n", average); // Print the value
    }

    close(client_socket); // Close connection after communication is done
}

int main()
{
    srand(time(NULL)); // Seed the random number generator

    signal(SIGINT, terminate); // Set singnal handler for Ctrl+C

    int server_client_pid = fork(); // Create child process

    if (server_client_pid == 0)
    {
        // Child process: server and client functionality
        server_client_func();
    }
    else if (server_client_pid > 0)
    {
        // Parent process: main program functionality
        FILE *file = fopen("werte.txt", "w");
        if (file == NULL)
        {
            printf("Error opening file.\n");
            return 1;
        }

        while (1)
        {
            int random = conv();
            fprintf(file, "%d\n", random);
            fflush(file); // Ensure the file is updated inmediatly

            sleep(1); // Pause for a second

            double average = stat(random);
            report(average);
        }

        fclose(file); // Close the file
    }
}
