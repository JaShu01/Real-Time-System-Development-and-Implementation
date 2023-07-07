# Real-Time-System-Development-and-Implementation

Welcome to the code repository for my project that involves developing and implementing a real-time system consisting of four processes.

## Project Description

The goal of this project is to develop and implement a real-time system that comprises the following four processes:

1. **Conv Process**  
   This process reads measurements from Analog-to-Digital converters (A/D) and verifies their plausibility. If necessary, it converts the measurements. Since physical A/D converters are not available, the Conv process generates random numbers.

2. **Log Process**  
   The Log process reads the measurements from the Conv process and writes them to a local file.

3. **Stat Process**  
   The Stat process reads the measurements from the Conv process and calculates statistical data such as the mean and sum.

4. **Report Process**  
   The Report process accesses the results from the Stat process and displays the statistical data in the shell.

## Synchronization Conditions

The implementation of the system should consider the following synchronization conditions:

- Conv process must write measurements before Log and Stat processes can read them.
- Stat process must write statistical data before Report process can read it.

## Data Exchange Methods

The project requires implementing the system using four different methods for data exchange between the processes:

1. **Pipes**
2. **Message Queues**
3. **Shared Memory with Semaphore**
4. **Sockets**

## Implementation and Documentation

To meet the project requirements, the following must be accomplished:

- Develop and implement the system using the specified system calls or (standard) library functions.
- Implement the data exchange between the processes using each of the four methods.
- Provide four different implementations of the program, each utilizing one of the data exchange methods.
- Develop the solution using a Bash script or a C program as open-source (freeware) software.
- Utilize a code repository platform such as GitHub to host the code.

Additionally, you should prepare a comprehensive and visually appealing documentation, approximately 10 pages long, describing your solution.

## Presentation and Demonstration

As part of the project, prepare a presentation with slides and a live demonstration. The presentation should be around 15-20 minutes long and showcase the functionality of your solution during the exercise.

## Simulator Requirements

The following requirements should be considered when developing the application:

- The application should be a command-line application.
- The source code should be well-commented for better understanding.
- The Conv, Log, Stat, and Report processes should be concurrent infinite processes.
- Create a framework to start the infinite processes using the fork system call. Monitor the parallel processes using appropriate commands such as top, ps, and pstree to determine the parent-child relationships.
- The program should gracefully terminate when the Ctrl-C key combination is pressed. Implement a signal handler for the SIGINT signal. Ensure that all resources used by the processes (pipes, message queues, shared memory, semaphores) are released upon program termination.
- Monitor the message queues, shared memory segments, and semaphores using the ipcs command. If the program fails to release these resources correctly upon incorrect termination, use ipcrm to free the message queues, shared memory segments, and semaphores.

Thanks!
