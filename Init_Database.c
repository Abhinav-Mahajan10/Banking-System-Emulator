#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int my_string_compare(char str1[], char str2[])
{
    int i;
    int len1 = 0;
    int len2 = 0;
    //to find length of string 1
    while(1)
    {
        if(str1[len1] == '\0')
        {
            break;
        }
        len1++;
    }
    //to find length of string 2
    while(1)
    {
        if(str2[len2] == '\0')
        {
            break;
        }
        len2++;
    }
    //if lengths are unequal, strings are unequal
    if(len1 != len2)
    {
        return -1;
    }
    for(i = 0;i < len1;i++)
    {
        //if characters are unequal, strings are unequal
        if(str1[i] != str2[i])
        {
            return -1;
        }
    }
    return 0;
}

int my_string_length(char str[])
{
    int i = 0;
    while(1)
    {
        if(str[i] == '\0')
        {
            break;
        }
        i++;
    }
    return i;
}

void my_malloc(char def, char str[], int len)
{
    for(int i = 0;i < len;i++)
    {
        str[i] = def;
    }
}

void Display_Menu()
{
    write(1, "Main Menu: -\n", sizeof("Main Menu: -\n"));
    write(1, "Your Options are: -\n", sizeof("Your Options are: -\n"));
    write(1, "1.) Add Admin\n", sizeof("1.) Add Admin\n"));
    write(1, "2.) Add Normal User\n", sizeof("2.)Add Normal User\n"));
    write(1, "3.) Add Joint User Account\n", sizeof("3.) Add Joint User Account\n"));
    write(1, "4.) Close\n", sizeof("4.) Close\n"));
}

int User_Menu_Choice()
{
    int choice;
    while(1)
    {
        write(1, "Enter 1, 2, 3 or 4(to exit)\n", sizeof("Enter 1, 2, 3 or 4(to exit)\n"));
        scanf("%d", &choice);
        if(choice == 1 || choice == 2 || choice == 3 || choice == 4)
        {
            break;
        }
        write(1, "Please enter a valid input!\n", sizeof("Please enter a valid input!\n"));
    }
    return choice;
}

int verify_Admin_name(char name[100], const char* filename)
{
    int fd = open(filename, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    char username[100];
    char password[100];
    int Account_ID;
    my_malloc('\0', username, sizeof(username));
    while(read(fd, username, sizeof(username)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            return -1;
        }
        lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return 0;
}

void Add_Admin(int *Account_ID)
{
    char name[100];
    my_malloc('\0', name, sizeof(name));
    while(1)
    {
        write(1, "Enter name of Admin: -\n", sizeof("Enter name of Admin: -\n"));
        scanf(" %[^\n]", name);
        if(verify_Admin_name(name, "Admins_File") == 0)
        {
            break;
        }
        write(1, "Name Already Exists!\n", sizeof("Name Already Exists!\n"));
    }

    write(1, "Enter password: -\n", sizeof("Enter password: -\n"));
    char password[100];
    my_malloc('\0', password, 100);
    scanf(" %[^\n]", password);
    int ID = (*Account_ID)++;
    printf("Your unique Account ID is = %d\n", ID);
    int fd = open("Admins_File", O_RDWR);
    lseek(fd, 0, SEEK_END);
    write(fd, name, sizeof(name));
    write(fd, password, sizeof(password));
    write(fd, &ID, sizeof(int));
}

int verify_User_name(char name[100], const char* filename)
{
    int fd = open(filename, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    char username[100];
    char password[100];
    int Account_ID;
    my_malloc('\0', username, sizeof(username));
    while(read(fd, username, sizeof(username)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            return -1;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return 0;
}

void Add_User(int *Account_ID)
{
    char name[100];
    my_malloc('\0', name, sizeof(name));
    while(1)
    {
        write(1, "Enter name of User: -\n", sizeof("Enter name of User: -\n"));
        scanf(" %[^\n]", name);
        if(verify_User_name(name, "Normal_Users_File") == 0)
        {
            break;
        }
        write(1, "Name Already Exists!\n", sizeof("Name Already Exists!\n"));
    }

    write(1, "Enter password: -\n", sizeof("Enter password: -\n"));
    char password[100];
    my_malloc('\0', password, 100);
    scanf(" %[^\n]", password);
    int ID = (*Account_ID)++;
    int balance = 0;
    printf("Your unique Account ID is = %d\n", ID);
    printf("Your initial balance is = %d\n", balance);
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_END);
    write(fd, name, sizeof(name));
    write(fd, password, sizeof(password));
    write(fd, &ID, sizeof(int));
    write(fd, &balance, sizeof(int));
}

int verify_Joint_Account_names(char name1[100], char name2[100], const char*filename)
{
    int fd = open(filename, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    char username1[100];
    char username2[100];
    char password[100];
    int Account_ID;
    my_malloc('\0', username1, sizeof(username1));
    my_malloc('\0', username2, sizeof(username2));
    while(read(fd, username1, sizeof(username1)) != 0)
    {
        read(fd, username2, sizeof(username2));
        if(my_string_compare(username1, name1) == 0 || 
        my_string_compare(username2, name2) == 0)
        {
            return -1;
        }
        if(my_string_compare(username1, name2) == 0 ||
        my_string_compare(username2, name1) == 0)
        {
            return -1;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return 0;
}

void Add_Joint_Account(int *Account_ID)
{
    char name1[100];
    char name2[100];
    my_malloc('\0', name1, sizeof(name1));
    my_malloc('\0', name2, sizeof(name2));
    while(1)
    {
        write(1, "Enter name of User1: -\n", sizeof("Enter name of User1: -\n"));
        scanf(" %[^\n]", name1);
        write(1, "Enter name of User2: -\n", sizeof("Enter name of User2: -\n"));
        scanf(" %[^\n]", name2);
        if(verify_Joint_Account_names(name1, name2, "Joint_Users_File") == 0)
        {
            break;
        }
        write(1, "Account Already Exists!\n", sizeof("Account Already Exists!\n"));
    }

    write(1, "Enter password: -\n", sizeof("Enter password: -\n"));
    char password[100];
    my_malloc('\0', password, 100);
    scanf(" %[^\n]", password);
    int ID = (*Account_ID)++;
    int balance = 0;
    printf("Your unique Account ID is = %d\n", ID);
    printf("Your initial balance is = %d\n", balance);
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_END);
    write(fd, name1, sizeof(name1));
    write(fd, name2, sizeof(name2));
    write(fd, password, sizeof(password));
    write(fd, &ID, sizeof(int));
    write(fd, &balance, sizeof(int));
}

void Exit_Program()
{
    exit(0);
}

void Menu_Switch(int user_choice, int *Account_ID)
{
    switch(user_choice)
    {
        case 1:
            Add_Admin(Account_ID);
            break;
        case 2:
            Add_User(Account_ID);
            break;
        case 3:
            Add_Joint_Account(Account_ID);
            break;
        case 4:
            Exit_Program();
            break;
    }
}

void MakeFile(const char* filename)
{
    int fd = open(filename, O_CREAT | O_RDWR, 0744);
    if(fd == -1)
    {
        write(1, "Error in opening files!\n", sizeof("Error in opening files!\n"));
        close(fd);
        exit(0);
    }
    close(fd);
}

void Initialise_Files()
{
    MakeFile("Admins_File");
    MakeFile("Normal_Users_File");
    MakeFile("Joint_Users_File");
}

void Control()
{
    Initialise_Files();
    int Account_ID = 0;
    while(1)
    {
        Display_Menu();
        int choice = User_Menu_Choice();
        Menu_Switch(choice, &Account_ID);
    }
}

int main()
{
    Control();
    return 0;
}