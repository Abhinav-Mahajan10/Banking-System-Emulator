#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

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

void ShowMenu_User()
{
    printf("Your options are: -\n");
    printf("1.) Deposit\n");
    printf("2.) Withdraw\n");
    printf("3.) Query Balance\n");
    printf("4.) Query ID\n");
    printf("5.) Change password\n");
    printf("6.) Logout\n");
}

void ShowMenu_Admin()
{
    printf("Your options are: -\n");
    printf("1.) Add User\n");
    printf("2.) Add Joint Account\n");
    printf("3.) Search Normal User Account\n");
    printf("4.) Search Joint User Account\n");
    printf("5.) Modify Normal User Account\n");
    printf("6.) Modify Joint User Account\n");
    printf("7.) Delete Normal User Account\n");
    printf("8.) Delete Joint User Account\n");
    printf("9.) Logout\n");
}

void Admin_Login(int nsd)
{
    char username[100];
    char password[100];
    my_malloc('\0', username, sizeof(username));
    my_malloc('\0', password, sizeof(password));
    printf("Enter Username:\n");
    scanf("%s", username);
    printf("Enter Password:\n");
    scanf("%s", password);
    int choice = 1;
    write(nsd, &choice, sizeof(choice));
    write(nsd, username, sizeof(username));
    write(nsd, password, sizeof(password));
}

void User_Login(int nsd)
{
    char username[100];
    char password[100];
    my_malloc('\0', username, sizeof(username));
    my_malloc('\0', password, sizeof(password));
    printf("Enter Username:\n");
    scanf("%s", username);
    printf("Enter Password:\n");
    scanf("%s", password);
    int choice = 2;
    write(nsd, &choice, sizeof(choice));
    write(nsd, username, sizeof(username));
    write(nsd, password, sizeof(password));
}

void Joint_Login(int nsd)
{
    char username[100];
    char password[100];
    my_malloc('\0', username, sizeof(username));
    my_malloc('\0', password, sizeof(password));
    printf("Enter Username:\n");
    scanf("%s", username);
    printf("Enter Password:\n");
    scanf("%s", password);
    int choice = 3;
    write(nsd, &choice, sizeof(choice));
    write(nsd, username, sizeof(username));
    write(nsd, password, sizeof(password));
}

void Choose_Login_Type(int nsd)
{
    printf("Main Menu: -\n");
    printf("Options: -\n");
    printf("1.) Login in as Admin\n");
    printf("2.) Login in as Normal User\n");
    printf("3.) Login in as Joint Account Holder\n");
    int option;
    while(1)
    {
        printf("Enter an option\n");
        scanf("%d", &option);
        if(option < 1 || option > 3)
        {
            printf("Invalid input! Try Again\n");
        }
        else
        {
            break;
        }
    }
    int result = 0;
    while(result == 0)
    {
        if(option == 1)
        {
            Admin_Login(nsd);
        }
        else if(option == 2)
        {
            User_Login(nsd);
        }
        else
        {
            Joint_Login(nsd);
        }
        char feedback[100];
        read(nsd, &result, sizeof(result));
        read(nsd, feedback, sizeof(feedback));
        printf("%s\n", feedback);
        if(result == 1)
        {
            break;
        }
    }
    while(1)
    {
        if(option == 2 || option == 3)
        {
            ShowMenu_User();
            int choice;
            while(1)
            {
                printf("Enter your choice\n");
                scanf("%d", &choice);
                if(choice < 1 || choice > 6)
                {
                    printf("Invalid Input, try again\n");
                }
                else
                {
                    break;
                }
            }
            write(nsd, &choice, sizeof(choice));
            if(choice == 1)
            {
                printf("Enter Amount you want to deposit:\n");
                int amt;
                scanf("%d", &amt);
                write(nsd, &amt, sizeof(amt));
                char feedback[100];
                int new_balance;
                read(nsd, &new_balance, sizeof(new_balance));
                read(nsd, feedback, sizeof(feedback));
                printf("Balance is %d\n", new_balance);
                printf("%s\n", feedback);
            }
            else if(choice == 2)
            {
                printf("Enter Amount you want to Withdraw:\n");
                int amt;
                scanf("%d", &amt);
                write(nsd, &amt, sizeof(amt));
                char feedback[100];
                int new_balance;
                read(nsd, &new_balance, sizeof(new_balance));
                read(nsd, feedback, sizeof(feedback));
                printf("Balance is %d\n", new_balance);
                printf("%s\n", feedback);
            }
            else if(choice == 3)
            {
                int balance;
                read(nsd, &balance, sizeof(balance));
                printf("Balance is %d\n", balance);
            }
            else if(choice == 4)
            {
                int ID;
                read(nsd, &ID, sizeof(int));
                printf("Account ID is %d\n", ID);
            }
            else if(choice == 5)
            {
                printf("Enter new password\n");
                char password_new[100];
                my_malloc('\0', password_new, sizeof(password_new));
                scanf("%s", password_new);
                write(nsd, &password_new, sizeof(password_new));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 6)
            {
                break;
            }
        }
        if(option == 1)
        {
            ShowMenu_Admin();
            int choice;
            while(1)
            {
                printf("Enter your choice\n");
                scanf("%d", &choice);
                if(choice < 1 || choice > 9)
                {
                    printf("Invalid Input, try again\n");
                }
                else
                {
                    break;
                }
            }
            write(nsd, &choice, sizeof(choice));
            if(choice == 1)
            {
                char username[100];
                char password[100];
                printf("Enter User Name\n");
                scanf("%s", username);
                printf("Enter Password\n");
                scanf("%s", password);
                write(nsd, username, sizeof(username));
                write(nsd, password, sizeof(password));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 2)
            {
                char username1[100];
                char username2[100];
                char password[100];
                printf("Enter User Name 1\n");
                scanf("%s", username1);
                printf("Enter User Name 2\n");
                scanf("%s", username2);
                printf("Enter Password\n");
                scanf("%s", password);
                write(nsd, username1, sizeof(username1));
                write(nsd, username2, sizeof(username2));
                write(nsd, password, sizeof(password));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 3)
            {
                char username[100];
                printf("Enter User Name whose account you are searching for\n");
                scanf("%s", username);
                write(nsd, username, sizeof(username));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 4)
            {
                char username1[100];
                char username2[100];
                printf("For the Joint Account you are searching for,\n");
                printf("Enter User Name 1\n");
                scanf("%s", username1);
                printf("Enter User Name 2\n");
                scanf("%s", username2);
                write(nsd, username1, sizeof(username1));
                write(nsd, username2, sizeof(username2));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 5)
            {
                char username[100];
                char password[100];
                int balance;
                printf("Enter User Name whose account you are searching for\n");
                scanf("%s", username);
                write(nsd, username, sizeof(username));
                printf("Enter new password\n");
                scanf("%s", password);
                write(nsd, password, sizeof(password));
                printf("Enter new balance total\n");
                scanf("%d", &balance);
                write(nsd, &balance, sizeof(int));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 6)
            {
                char username1[100];
                char username2[100];
                char password[100];
                int balance;
                printf("For the Joint Account you are searching for,\n");
                printf("Enter User Name 1\n");
                scanf("%s", username1);
                write(nsd, username1, sizeof(username1));
                printf("Enter User Name 2\n");
                scanf("%s", username2);
                write(nsd, username2, sizeof(username2));
                printf("Enter new password\n");
                scanf("%s", password);
                write(nsd, password, sizeof(password));
                printf("Enter new balance total\n");
                scanf("%d", &balance);
                write(nsd, &balance, sizeof(int));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 7)
            {
                char username[100];
                printf("Enter User Name whose account you are deleting\n");
                scanf("%s", username);
                write(nsd, username, sizeof(username));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 8)
            {
                char username1[100];
                char username2[100];
                printf("For the Joint Account you are deleting,\n");
                printf("Enter User Name 1\n");
                scanf("%s", username1);
                printf("Enter User Name 2\n");
                scanf("%s", username2);
                write(nsd, username1, sizeof(username1));
                write(nsd, username2, sizeof(username2));
                char feedback[100];
                read(nsd, feedback, sizeof(feedback));
                printf("%s\n", feedback);
            }
            else if(choice == 9)
            {
                break;
            }
        }
    }
}

void Control()
{
    struct sockaddr_in server;
	int sd, msgLength;
	char buff[50];
	char result;

	sd = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(5555);
	connect(sd, (struct sockaddr *)&server, sizeof(server));

	Choose_Login_Type(sd);

	close(sd);
}

int main()
{
    Control();
    return 0;
}