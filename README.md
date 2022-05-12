# Banking-System-Emulator
Abhinav Mahajan, IMT2020553.  
EG 301P Operating Systems Lab    
## Objective
Design and Development of online banking management system written in anci C in system programming style.  
## Functionalities
- One can either login as an Admin, a normal User or as a Joint Account Holder.
- An Admin can Add, delete, modify, query and check Accounts. (Both normal and joint accounts)
- A normal user or joint account holder has the ability to deposit, withdraw, check balance, check Account ID and changing password.
## Executing
Make the executables ready by: -
- gcc Init_Database.c -o InitDB
- gcc -pthread Server.c -o serv
- gcc Client.c -o cli
- gcc Demo_File_Reader.c -o FR
Now, first run ./InitDB, add an Admin(Must), and normal users or joint users(optional).  
Then run ./serv  
Then on other terminals run ./cli  
To check the datafields in any file, you can make use of the Demo_File_Reader.c file.
## Other feautures and assumptions
- Same User can be logged in on many clients, as file locking is done appropriately.
- Locking not done on Admin queries. It is assumed that no user is logged in when the Admin is logged in, as the admin can delete and add users and change user details.  
