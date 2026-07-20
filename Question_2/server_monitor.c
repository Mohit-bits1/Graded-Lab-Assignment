#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Signal handler to asynchronously clean up completed child processes (reaping zombies)
void handle_sigchld(int sig) {
    (void)sig; // Suppress unused parameter warning
    // waitpid with WNOHANG reaps all terminated children without blocking the main server loop
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    // Register the SIGCHLD signal handler to prevent zombie accumulation
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Error setting up SIGCHLD handler");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Master monitoring process started. PID: %d\n", getpid());
    printf("[SERVER] Creating child worker process...\n");

    // 1. Create a child process using fork()
    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (child_pid == 0) {
        // --- CHILD PROCESS WORKER BLOCK ---
        printf("  [WORKER] Child process spawned. PID: %d\n", getpid());
        printf("  [WORKER] Working... simulates becoming unresponsive (hanging).\n");
        
        // Simulates an infinite loop or unresponsive lock
        while (1) {
            sleep(1); 
        }
        exit(EXIT_SUCCESS);
    } 
    else {
        // --- PARENT PROCESS SERVER MONITOR BLOCK ---
        // 2. Monitor execution
        printf("[SERVER] Monitoring child worker (PID: %d) for 4 seconds...\n", child_pid);
        sleep(4);

        printf("[SERVER] Timeout! Child process %d is unresponsive.\n", child_pid);
        
        // 4. Terminate the unresponsive child process using SIGKILL
        printf("[SERVER] Sending SIGKILL to terminate child %d forcefully.\n", child_pid);
        if (kill(child_pid, SIGKILL) == 0) {
            printf("[SERVER] Signal delivered successfully.\n");
        } else {
            perror("Failed to send signal");
        }

        // Give the signal handler a brief moment to log/reap the process
        sleep(1);
        printf("[SERVER] Check completed. Exiting safely.\n");
    }

    return 0;
}
