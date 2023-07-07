// title:        main.c
// description:  Main code in C
// author:       Diego Gutiérrez und Jabrail Shurayev
// date:         25.06.2023
// compile with: gcc main.c -o main
// nodes:        This code serves as a simple user interface to execute
//               different methods of inter-process communication (IPC), based on user input.
//               The user can select between four options: sockets (S), pipes (P), message queues (M),
//               and shared memory (SM). Depending on the choice, the corresponding compiled binary
//               ('./sockets', './pipes', './mq', or './sm') is executed using the system call, and
//               if an invalid input is given, it prints an error message and exits.

// Libraries used in the program
#include <stdio.h>
#include <stdlib.h>

// Main method to select the variant with which the data exchange is to be carried out.
int main()
{
    printf("Hello, in this code, data will be exchanged using 4 different methods of your choice\n");
    printf("Press the button with which you want to exchange data\n");
    printf("P - for Pipes, S - for Sockets, M - for Message Queues, SM - for Shared Memory\n");

    char choice;
    scanf(" %c", &choice);
    int return_value;
    // Execute the desired compiled biary file
    if (choice == 'S')
    {
        return_value = system("./sockets");
        if (return_value == -1)
        {
            printf("Failed to execute ./sockets\n");
        }
    }
    else if (choice == 'P')
    {
        return_value = system("./pipes");
        if (return_value == -1)
        {
            printf("Failed to execute ./pipes\n");
        }
    }
    else if (choice == 'M')
    {
        return_value = system("./mq");
        if (return_value == -1)
        {
            printf("Failed to execute ./mq\n");
        }
    }
    else if (choice == 'SM')
    {
        return_value = system("./sm");
        if (return_value == -1)
        {
            printf("Failed to execute ./sm\n");
        }
    }
    else
    {
        printf("Invalid choice.\n");
        return 1;
    }

    return 0;
}
