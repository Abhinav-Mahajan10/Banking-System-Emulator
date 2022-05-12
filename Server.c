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

void Normal_User_Request_Handler(int, char[]);
void Joint_Account_Request_Handler(int, char[]);
void Admin_Request_Handler(int);

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

int Admin_Login(int nsd, char username[100])
{
    char password[100];
    my_malloc('\0', username, 100);
    my_malloc('\0', password, sizeof(password));
    read(nsd, username, 100);
    read(nsd, password, sizeof(password));

    int fd = open("Admins_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    int result = 0;
    char name[100];
    char pwd[100];
    while(read(fd, name, sizeof(name)) != 0)
    {
        read(fd, pwd, sizeof(pwd));
        if(my_string_compare(username, name) == 0)
        {
            if(my_string_compare(password, pwd) == 0)
            {
                result = 1;
            }
            break;
        }
        lseek(fd, sizeof(int), SEEK_CUR);
    }
    close(fd);
    return result;
}

int Normal_User_Login(int nsd, char username[100])
{
    char password[100];
    my_malloc('\0', username, 100);
    my_malloc('\0', password, sizeof(password));
    read(nsd, username, 100);
    read(nsd, password, sizeof(password));
    //printf("Here\n");

    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    int result = 0;
    char name[100];
    char pwd[100];
    my_malloc('\0',name, sizeof(name));
    my_malloc('\0',pwd, sizeof(pwd));
    while(read(fd, name, sizeof(name)) != 0)
    {
        read(fd, pwd, sizeof(pwd));
        if(my_string_compare(username, name) == 0)
        {
            if(my_string_compare(password, pwd) == 0)
            {
                result = 1;
            }
            break;
        }
        lseek(fd, sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return result;
}

int Joint_Account_Login(int nsd, char username[100])
{
    char password[100];
    my_malloc('\0', username, 100);
    my_malloc('\0', password, sizeof(password));
    read(nsd, username, 100);
    read(nsd, password, sizeof(password));

    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    int result = 0;
    char name1[100];
    char name2[100];
    char pwd[100];
    my_malloc('\0', pwd, sizeof(pwd));
    my_malloc('\0', name1, sizeof(name1));
    my_malloc('\0', name2, sizeof(name2));
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        read(fd, pwd, sizeof(pwd));
        //printf("%s %s %s %s %s\n", username, name1, name2, pwd, password);
        if(my_string_compare(username, name1) == 0
         || my_string_compare(username, name2) == 0)
        {
            if(my_string_compare(password, pwd) == 0)
            {
                result = 1;
            }
            break;
        }
        lseek(fd, sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return result;
}

void User_Deposit(int nsd, char username[100])
{
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name[100];
    char password[100];
    int balance;
    int offset = 0;
    while(read(fd, name, sizeof(name)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);

        
            int deposit_amt;
            read(nsd, &deposit_amt, sizeof(deposit_amt));
            //struct flock fl;
            struct flock lock;
            lock.l_type   = F_WRLCK;  
            lock.l_whence = SEEK_SET;
            int start_offset = offset*(sizeof(name) + sizeof(password) + sizeof(int) + sizeof(int)) 
                                + sizeof(name)  + sizeof(password) + sizeof(int);
            lock.l_start  = start_offset;       
            lock.l_len    = sizeof(int);        
            lock.l_pid    = getpid();
            int ret = fcntl(fd, F_SETLK, &lock);
            printf("File locking done, ret = %d\n", ret);
            lseek(fd, start_offset, SEEK_SET);
            read(fd, &balance, sizeof(int));

            // int deposit_amt;
            // read(nsd, &deposit_amt, sizeof(deposit_amt));
            if(deposit_amt < 0)
            {
                write(nsd, &balance, sizeof(int));
                write(nsd, "Unsuccessful Deposit\n", sizeof("Unsuccessful Deposit\n"));
            }
            else
            {
                lseek(fd, -sizeof(int), SEEK_CUR);
                int new_balance = balance + deposit_amt;
                lseek(fd, start_offset, SEEK_SET);
                write(fd, &new_balance, sizeof(int));
                write(nsd, &new_balance, sizeof(int));
                write(nsd, "Successful Deposit\n", sizeof("Successful Deposit\n"));
            }

            lseek(fd, start_offset, SEEK_SET);
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        offset++;
    }
    Normal_User_Request_Handler(nsd, username);
    close(fd);
}

void Joint_User_Deposit(int nsd, char username[100])
{
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name1[100];
    char name2[100];
    char password[100];
    int balance;
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        if(my_string_compare(username, name1) == 0
        || my_string_compare(username, name2) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);

            int deposit_amt;
            read(nsd, &deposit_amt, sizeof(deposit_amt));
            static struct flock fl;
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked.\n");
            read(fd, &balance, sizeof(int));

            if(deposit_amt < 0)
            {
                write(nsd, &balance, sizeof(int));
                write(nsd, "Unsuccessful Deposit\n", sizeof("Unsuccessful Deposit\n"));
            }
            else
            {
                lseek(fd, -sizeof(int), SEEK_CUR);
                int new_balance = balance + deposit_amt;
                write(fd, &new_balance, sizeof(int));
                write(nsd, &new_balance, sizeof(int));
                write(nsd, "Successful Deposit\n", sizeof("Successful Deposit\n"));
            }

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked.\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Joint_Account_Request_Handler(nsd, username);
    close(fd);
}

void User_Withdraw(int nsd, char username[100])
{
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name[100];
    char password[100];
    int balance;
    while(read(fd, name, sizeof(name)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);

            int withdraw_amt;
            read(nsd, &withdraw_amt, sizeof(withdraw_amt));
            struct flock fl;
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            

            read(fd, &balance, sizeof(int));
            if(withdraw_amt < 0 || balance < withdraw_amt)
            {
                write(nsd, &balance, sizeof(int));
                write(nsd, "Unsuccessful Withdrawal\n", sizeof("Unsuccessful Withdrawal\n"));
            }
            else
            {
                lseek(fd, -sizeof(int), SEEK_CUR);
                int new_balance = balance - withdraw_amt;
                write(fd, &new_balance, sizeof(int));
                write(nsd, &new_balance, sizeof(int));
                write(nsd, "Successful Withdrawal\n", sizeof("Successful Withdrawal\n"));
            }

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Normal_User_Request_Handler(nsd, username);
    close(fd);
}

void Joint_User_Withdraw(int nsd, char username[100])
{
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name1[100];
    char name2[100];
    char password[100];
    int balance;
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        if(my_string_compare(username, name1) == 0
        || my_string_compare(username, name2) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);

            int withdraw_amt;
            read(nsd, &withdraw_amt, sizeof(withdraw_amt));
            struct flock fl;
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");

            read(fd, &balance, sizeof(int));
            if(withdraw_amt < 0 || balance < withdraw_amt)
            {
                write(nsd, &balance, sizeof(int));
                write(nsd, "Unsuccessful Withdrawal\n", sizeof("Unsuccessful Withdrawal\n"));
            }
            else
            {
                lseek(fd, -sizeof(int), SEEK_CUR);
                int new_balance = balance - withdraw_amt;
                write(fd, &new_balance, sizeof(int));
                write(nsd, &new_balance, sizeof(int));
                write(nsd, "Successful Withdrawal\n", sizeof("Successful Withdrawal\n"));
            }

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Joint_Account_Request_Handler(nsd, username);
    close(fd);
}

void Query_Balance(int nsd, char username[100])
{
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name[100];
    char password[100];
    int balance;
    while(read(fd, name, sizeof(name)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);
            
            struct flock fl;
            fl.l_type   = F_RDLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            read(fd, &balance, sizeof(int));
            write(nsd, &balance, sizeof(int));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Normal_User_Request_Handler(nsd, username);
    close(fd);
}

void Joint_Query_Balance(int nsd, char username[100])
{
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name1[100];
    char name2[100];
    char password[100];
    int balance;
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        if(my_string_compare(username, name1) == 0
        || my_string_compare(username, name2) == 0)
        {
            lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);
            
            struct flock fl;
            fl.l_type   = F_RDLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            read(fd, &balance, sizeof(int));
            write(nsd, &balance, sizeof(int));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Joint_Account_Request_Handler(nsd, username);
    close(fd);
}

void Query_Account_ID(int nsd, char username[100])
{
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name[100];
    char password[100];
    int ID;
    while(read(fd, name, sizeof(name)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            lseek(fd, sizeof(password), SEEK_CUR);

            struct flock fl;
            fl.l_type   = F_RDLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            read(fd, &ID, sizeof(int));
            write(nsd, &ID, sizeof(int));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Normal_User_Request_Handler(nsd, username);
    close(fd);
}

void Joint_Query_Account_ID(int nsd, char username[100])
{
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name1[100];
    char name2[100];
    char password[100];
    int ID;
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        if(my_string_compare(username, name1) == 0
        || my_string_compare(username, name2) == 0)
        {
            lseek(fd, sizeof(password), SEEK_CUR);

            struct flock fl;
            fl.l_type   = F_RDLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(int);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            read(fd, &ID, sizeof(int));
            write(nsd, &ID, sizeof(int));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Joint_Account_Request_Handler(nsd, username);
    close(fd);
}

void User_Change_Password(int nsd, char username[100])
{
    int fd = open("Normal_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name[100];
    char password[100];
    while(read(fd, name, sizeof(name)) != 0)
    {
        if(my_string_compare(username, name) == 0)
        {
            char new_password[100];
            my_malloc('\0', new_password, sizeof(new_password));

            read(nsd, new_password, sizeof(new_password));
            struct flock fl;
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(new_password);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");

            write(fd, new_password, sizeof(new_password));
            write(nsd, "Successful Password Update\n", sizeof("Successful Password Update\n"));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Normal_User_Request_Handler(nsd, username);
    close(fd);
}

void Joint_User_Change_Password(int nsd, char username[100])
{
    int fd = open("Joint_Users_File", O_RDWR);
    lseek(fd, 0, SEEK_SET);

    char name1[100];
    char name2[100];
    char password[100];
    while(read(fd, name1, sizeof(name1)) != 0)
    {
        read(fd, name2, sizeof(name2));
        if(my_string_compare(username, name1) == 0
        || my_string_compare(username, name2) == 0)
        {
            char new_password[100];
            my_malloc('\0', new_password, sizeof(new_password));

            read(nsd, new_password, sizeof(new_password));
            struct flock fl;
            fl.l_type   = F_WRLCK;  
            fl.l_whence = SEEK_CUR; 
            fl.l_start  = 0;        
            fl.l_len    = sizeof(new_password);        
            fl.l_pid    = getpid();
            fcntl(fd, F_SETLK, &fl);
            printf("File locked\n");
            
            write(fd, new_password, sizeof(new_password));
            write(nsd, "Successful Password Update\n", sizeof("Successful Password Update\n"));

            fl.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &fl);
            printf("File unlocked\n");
            break;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    Joint_Account_Request_Handler(nsd, username);
    close(fd);
}

void Logout(int nsd)
{
    // int ans = -1;
    // write(nsd, &ans, sizeof(int));
}

void Normal_User_Request_Handler(int nsd, char username[100])
{
    int query_choice;
    read(nsd, &query_choice, sizeof(int));
    switch(query_choice)
    {
        case 1:
            User_Deposit(nsd, username);
            break;
        case 2:
            User_Withdraw(nsd, username);
            break;
        case 3:
            Query_Balance(nsd, username);
            break;
        case 4:
            Query_Account_ID(nsd, username);
            break;
        case 5:
            User_Change_Password(nsd, username);
        case 6:
            Logout(nsd);
            break;
    }
}

void Joint_Account_Request_Handler(int nsd, char username[100])
{
    int query_choice;
    read(nsd, &query_choice, sizeof(int));
    switch(query_choice)
    {
        case 1:
            Joint_User_Deposit(nsd, username);
            break;
        case 2:
            Joint_User_Withdraw(nsd, username);
            break;
        case 3:
            Joint_Query_Balance(nsd, username);
            break;
        case 4:
            Joint_Query_Account_ID(nsd, username);
            break;
        case 5:
            Joint_User_Change_Password(nsd, username);
        case 6:
            Logout(nsd);
            break;
    }
}

int verify_name(char name[100], const char* filename)
{
    int fd = open(filename, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    char username[100];
    char password[100];
    my_malloc('\0', username, sizeof(username));
    my_malloc('\0', password, sizeof(password));
    while(read(fd, username, sizeof(username)) != 0)
    {
        //printf("%s, %s\n", username, name);
        if(my_string_compare(username, name) == 0)
        {
            return -1;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return 0;
}

int verify_Joint_Account(char name1[100], char name2[100], const char* filename)
{
    int fd = open(filename, O_RDWR);
    lseek(fd, 0, SEEK_SET);
    char username1[100];
    char username2[100];
    char password[100];
    my_malloc('\0', username1, sizeof(username1));
    my_malloc('\0', username2, sizeof(username2));
    my_malloc('\0', password, sizeof(password));
    while(read(fd, username1, sizeof(username1)) != 0)
    {
        read(fd, username2, sizeof(username2));
        //printf("%s %s %s %s\n", username1, username2, name1, name2);
        if(my_string_compare(username1, name1) == 0
         || my_string_compare(username1, name2) == 0)
        {
            return -1;
        }
        if(my_string_compare(username2, name1) == 0
         || my_string_compare(username2, name2) == 0)
        {
            return -1;
        }
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
    }
    close(fd);
    return 0;
}

int getID(const char* filename1, const char* filename2, const char* filename3)
{
    int fd = open(filename1, O_RDWR);
    char username1[100];
    char username2[100];
    char password[100];
    my_malloc('\0', username1, sizeof(username1));
    my_malloc('\0', username2, sizeof(username2));
    my_malloc('\0', password, sizeof(password));
    int number_of_accounts = 0;
    while(read(fd, username1, sizeof(username1)) != 0)
    {
        lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        number_of_accounts++;
    }
    fd = open(filename2, O_RDWR);
    while(read(fd, username1, sizeof(username1)) != 0)
    {
        lseek(fd, sizeof(username2) +sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        number_of_accounts++;
    }
    close(fd);
    fd = open(filename3, O_RDWR);
    while(read(fd, username1, sizeof(username1)) != 0)
    {
        lseek(fd, sizeof(password) + sizeof(int), SEEK_CUR);
        number_of_accounts++;
    }
    return number_of_accounts;
}

void Add_User(int nsd)
{
    //printf("Here\n");
    char name[100];
    char password[100];
    my_malloc('\0',name,sizeof(name));
    my_malloc('\0',password,sizeof(password));
    read(nsd, &name, sizeof(name));
    read(nsd, password, sizeof(password));
    int flag = verify_name(name, "Normal_Users_File");
    int Account_ID = getID("Normal_Users_File", "Joint_Users_File", "Admins_File");
    if(flag == 0)
    {
        int fd = open("Normal_Users_File", O_RDWR);
        lseek(fd, 0, SEEK_END);
        write(fd, name, sizeof(name));
        write(fd, password, sizeof(password));
        write(fd, &Account_ID, sizeof(int));
        int initial_balance = 0;
        write(fd, &initial_balance, sizeof(int));
        write(nsd, "User Successfully added\n", sizeof("User Successfully added\n"));
        close(fd);
    }
    else
    {
        write(nsd, "User already has an account!\n", sizeof("User already has an account!\n"));
    }
    Admin_Request_Handler(nsd);
}

void Add_Joint_Users(int nsd)
{
    char name1[100];
    char name2[100];
    char password[100];
    my_malloc('\0',name1,sizeof(name1));
    my_malloc('\0',name2,sizeof(name2));
    my_malloc('\0',password, sizeof(password));
    read(nsd, &name1, sizeof(name1));
    read(nsd, &name2, sizeof(name2));
    read(nsd, password, sizeof(password));
    int flag = verify_Joint_Account(name1, name2, "Joint_Users_File");
    int Account_ID = getID("Normal_Users_File", "Joint_Users_File", "Admins_File");
    if(flag == 0)
    {
        int fd = open("Joint_Users_File", O_RDWR);
        lseek(fd, 0, SEEK_END);
        write(fd, name1, sizeof(name1));
        write(fd, name2, sizeof(name2));
        write(fd, password, sizeof(password));
        write(fd, &Account_ID, sizeof(int));
        int initial_balance = 0;
        write(fd, &initial_balance, sizeof(int));
        write(nsd, "User Successfully added\n", sizeof("User Successfully added\n"));
        close(fd);
    }
    else
    {
        write(nsd, "Account already exists!\n", sizeof("Account already exists!\n"));
    }
    Admin_Request_Handler(nsd);
}

void Search_Normal_Account(int nsd)
{
    char name[100];
    read(nsd, name, sizeof(name));
    int flag = verify_name(name, "Normal_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        write(nsd, "Account exists\n", sizeof("Account exists\n"));
    }
    Admin_Request_Handler(nsd);
}

void Search_Joint_Account(int nsd)
{
    char name1[100];
    char name2[100];
    read(nsd, name1, sizeof(name1));
    read(nsd, name2, sizeof(name2));
    int flag = verify_Joint_Account(name1, name2, "Joint_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        write(nsd, "Account exists\n", sizeof("Account exists\n"));
    }
    Admin_Request_Handler(nsd);
}

void Modify_Normal_Account(int nsd)
{
    char name[100];
    my_malloc('\0',name,sizeof(name));
    read(nsd, name, sizeof(name));
    char password[100];
    int balance;
    my_malloc('\0',password,sizeof(password));
    read(nsd, password, sizeof(password));
    read(nsd, &balance, sizeof(int));
    int flag = verify_name(name, "Normal_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        const char* filename = "Normal_Users_File";
        int fd = open(filename, O_RDWR);
        char username[100];
        while(read(fd, username, sizeof(username)) != 0)
        {
            if(my_string_compare(username, name) == 0)
            {
                write(fd, password, sizeof(password));
                lseek(fd, sizeof(int), SEEK_CUR);
                write(fd, &balance, sizeof(int));
                break;
            }
            lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        }
        write(nsd, "Successfully Modified\n", sizeof("Successfully Modified\n"));
        close(fd);
    }
    Admin_Request_Handler(nsd);
}

void Modify_Joint_Account(int nsd)
{
    char name1[100];
    char name2[100];
    my_malloc('\0',name1,sizeof(name1));
    my_malloc('\0',name2,sizeof(name2));
    read(nsd, name1, sizeof(name1));
    read(nsd, name2, sizeof(name2));
    char password[100];
    int balance;
    my_malloc('\0',password, sizeof(password));
    read(nsd, password, sizeof(password));
    read(nsd, &balance, sizeof(int));
    //printf("%s %s %s %d\n", name1, name2, password, balance);
    int flag = verify_Joint_Account(name1, name2, "Joint_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        const char* filename = "Joint_Users_File";
        int fd = open(filename, O_RDWR);

        char username1[100];
        char username2[100];
        //char password[100];
        my_malloc('\0', username1, sizeof(username1));
        my_malloc('\0', username2, sizeof(username2));
        //my_malloc('\0', password, sizeof(password));
        while(read(fd, username1, sizeof(username1)) != 0)
        {
            read(fd, username2, sizeof(username2));
            if(my_string_compare(username1, name1) == 0
            || my_string_compare(username1, name2) == 0)
            {
                write(fd, password, sizeof(password));
                lseek(fd, sizeof(int), SEEK_CUR);
                write(fd, &balance, sizeof(int));
                break;
            }
            else if(my_string_compare(username2, name1) == 0
            || my_string_compare(username2, name2) == 0)
            {
                write(fd, password, sizeof(password));
                lseek(fd, sizeof(int), SEEK_CUR);
                write(fd, &balance, sizeof(int));
                break;
            }
            lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        }
        write(nsd, "Successfully Modified\n", sizeof("Successfully Modified\n"));
        close(fd);
    }
    Admin_Request_Handler(nsd);
}

void Delete_User(int nsd)
{
    char name[100];
    read(nsd, name, sizeof(name));
    int flag = verify_name(name, "Normal_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        char password[100];
        const char* filename = "Normal_Users_File";
        int fd = open(filename, O_RDWR);
        char username[100];
        while(read(fd, username, sizeof(username)) != 0)
        {
            if(my_string_compare(username, name) == 0)
            {
                lseek(fd, -sizeof(username), SEEK_CUR);
                for(int i = 0;i < sizeof(username);i++)
                {
                    char buf = '\0';
                    write(fd, &buf, sizeof(char));
                }
                break;
            }
            lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        }
        write(nsd, "Successfully Deleted\n", sizeof("Successfully Deleted\n"));
        close(fd);
    }
    Admin_Request_Handler(nsd);
}

void Delete_Joint_Account(int nsd)
{
    char name1[100];
    char name2[100];
    read(nsd, name1, sizeof(name1));
    read(nsd, name2, sizeof(name2));
    int flag = verify_Joint_Account(name1, name2, "Joint_Users_File");
    if(flag == 0)
    {
        write(nsd, "Account does not exist!\n", sizeof("Account does not exist!\n"));
    }
    else
    {
        const char* filename = "Joint_Users_File";
        int fd = open(filename, O_RDWR);

        char username1[100];
        char username2[100];
        char password[100];
        my_malloc('\0', username1, sizeof(username1));
        my_malloc('\0', username2, sizeof(username2));
        my_malloc('\0', password, sizeof(password));
        while(read(fd, username1, sizeof(username1)) != 0)
        {
            read(fd, username2, sizeof(username2));
            if(my_string_compare(username1, name1) == 0
            || my_string_compare(username1, name2) == 0)
            {
                lseek(fd, -(sizeof(username1)+sizeof(username2)), SEEK_CUR);
                for(int i = 0;i < sizeof(username1)+sizeof(username2);i++)
                {
                    char buf = '\0';
                    write(fd, &buf, sizeof(char));
                }
                break;
            }
            if(my_string_compare(username2, name1) == 0
            || my_string_compare(username2, name2) == 0)
            {
                lseek(fd, -(sizeof(username1)+sizeof(username2)), SEEK_CUR);
                for(int i = 0;i < sizeof(username1)+sizeof(username2);i++)
                {
                    char buf = '\0';
                    write(fd, &buf, sizeof(char));
                }
                break;
            }
            lseek(fd, sizeof(password) + sizeof(int) + sizeof(int), SEEK_CUR);
        }
        write(nsd, "Successfully Deleted\n", sizeof("Successfully Deleted\n"));
        close(fd);
    }
    Admin_Request_Handler(nsd);
}

void Admin_Request_Handler(int nsd)
{
    int query_choice;
    read(nsd, &query_choice, sizeof(int));
    switch(query_choice)
    {
        case 1:
            Add_User(nsd);
            break;
        case 2:
            Add_Joint_Users(nsd);
            break;
        case 3:
            Search_Normal_Account(nsd);
            break;
        case 4:
            Search_Joint_Account(nsd);
            break;
        case 5:
            Modify_Normal_Account(nsd);
            break;
        case 6:
            Modify_Joint_Account(nsd);
            break;
        case 7:
            Delete_User(nsd);
            break;
        case 8:
            Delete_Joint_Account(nsd);
            break;
        case 9:
            Logout(nsd);
            break;
    }
}

void Post_Connection_Handler(int *nsfd)
{
    //printf("First here\n");
    int nsd = *(int*)nsfd;
    int choice;
    char username[100];
    my_malloc('\0', username, sizeof(username));
    int result = 0;
    read(nsd, &choice, sizeof(choice));
    //printf("%d\n", choice);
    while(result == 0)
    {
        if(choice == 1)
        {
            result = Admin_Login(nsd, username);
        }
        else if(choice == 2)
        {
            result = Normal_User_Login(nsd, username);
        }
        else
        {
            result = Joint_Account_Login(nsd, username);
        }
        if(result == 1)
        {
            break;
        }
        write(nsd, &result, sizeof(result));
        write(nsd, "Unsuccessful Login!\n", sizeof("Unsuccessful Login!\n"));
    }
    write(nsd, &result, sizeof(result));
    write(nsd, "Successful Login!\n", sizeof("Successful Login!\n"));
    if(choice == 1)
    {
        Admin_Request_Handler(nsd);
    }
    if(choice == 2)
    {
        Normal_User_Request_Handler(nsd, username);
    }
    if(choice == 3)
    {
        Joint_Account_Request_Handler(nsd, username);
    }
    close(nsd);
    printf("Ended a client session.\n");
}

// void *First_Step_Handler(void *nsfd)
// {
//     int nsd = *(int*)nsd;
//     Post_Connection_Handler(nsd);
// }

void Control()
{
    struct sockaddr_in server, client;
	int sd, nsd, clientLen;
	pthread_t threads;
	bool result;
	sd = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(5555);

	bind(sd, (struct sockaddr *)&server, sizeof(server));
	listen(sd, 10);	//queue length :: 10 clients per connection
	
    static struct flock fl_User;

	while(1)
    {
        write(1, "Waiting for a client..\n",sizeof("Waiting for a client..\n"));
		clientLen = sizeof(client);
		int nsd = accept(sd, (struct sockaddr *)&client, &clientLen);

		write(1, "Connected to the client!\n", sizeof("Connected to the client!\n"));
		if(pthread_create(&threads, NULL, (void *)Post_Connection_Handler, (void *)&nsd) < 0)
        {
			perror("could not create thread");
			return ;
		}		
	}
	pthread_exit(NULL);
	close(sd);
}

int main()
{
    Control();
    return 0;
}