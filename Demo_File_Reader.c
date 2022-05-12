#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

void Control()
{
    const char*filename = "Normal_Users_File";
    int fd = open(filename, O_RDONLY);
    char name[100];
    char password[100];
    int balance;
    int ID;
    read(fd, name, sizeof(name));
    read(fd, password, sizeof(password));
    read(fd, &ID, sizeof(ID));
    read(fd, &balance, sizeof(balance));
    printf("%s %s %d %d\n", name, password, ID, balance);
}

int main()
{
    Control();
    // const char* filename = "Normal_Users_File";
    // int fd = open(filename, O_RDONLY);
    // lseek(fd, 0, SEEK_SET);
    // char name[100];
    // char password[100];
    // int balance = 0;
    // int ID;
    // while(read(fd, name, sizeof(name)) != 0)
    // {
    //     printf("%s\n", name);
    //     read(fd, password, sizeof(password));
    //     printf("%s\n", password);
    //     read(fd, &balance, sizeof(int));
    //     printf("%d\n", balance);
    //     read(fd, &ID, sizeof(int));
    //     printf("%d\n", ID);
    // }
    return 0;
}