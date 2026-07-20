#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

// Fixed-size structure to allow efficient, random-access positioning via lseek()
typedef struct {
    int id;
    char name[32];
    double salary;
} Employee;

int main() {
    const char *filename = "employee_records.dat";
    
    // 1. Create a file using the low-level open() system call
    // O_CREAT: Creates file if missing. O_RDWR: Read/Write mode. O_TRUNC: Clears old records for clean run.
    // 0644 provides secure read/write permissions for the owner.
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }
    printf("[SYSTEM] File '%s' created securely using open().\n", filename);

    // 2. Writes employee records
    Employee emp1 = {101, "Alice Smith", 75000.0};
    Employee emp2 = {102, "Bob Jones", 82000.0};

    if (write(fd, &emp1, sizeof(Employee)) == -1 || 
        write(fd, &emp2, sizeof(Employee)) == -1) {
        perror("Error writing records");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("[SYSTEM] Two employee records written sequentially using write().\n");

    // 3. Updates specific records without rewriting the entire file
    // Let's modify Bob's record (the 2nd record, index 1)
    off_t offset = 1 * sizeof(Employee); 
    
    // Use lseek to point the file descriptor exactly to Bob's offset
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking during update");
        close(fd);
        exit(EXIT_FAILURE);
    }

    Employee updated_emp2 = {102, "Bob Jones (Promoted)", 95000.0};
    if (write(fd, &updated_emp2, sizeof(Employee)) == -1) {
        perror("Error updating record");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("[SYSTEM] Record ID 102 updated directly in-place using lseek() + write().\n");

    // 4. Retrieves records from any location efficiently
    // Let's hop back up to retrieve the first record (Alice, index 0) randomly
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Error seeking during retrieval");
        close(fd);
        exit(EXIT_FAILURE);
    }

    Employee reader;
    if (read(fd, &reader, sizeof(Employee)) == -1) {
        perror("Error reading record");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("[FETCHED] ID: %d, Name: %s, Salary: $%.2f\n", reader.id, reader.name, reader.salary);

    // Close down the file descriptor resource cleanly
    if (close(fd) == -1) {
        perror("Error closing file descriptor");
        exit(EXIT_FAILURE);
    }
    printf("[SYSTEM] File descriptor released safely using close().\n");

    return 0;
}
