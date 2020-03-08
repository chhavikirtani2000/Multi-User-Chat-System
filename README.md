# Multi-User-Chat-System
This is a multi user chat system which connects multiple clients to a server which facilitates message passing amongst clients.
This particular code works on a single system. Will upload a chat system which would run on different systems connecting them via IP address
Compilation and Testing: 
Use the cd command to go to the directory Makefile, serverftry.c and clientftry.c files are located. 
Use the make command to compile both the files.  
Run the serverftry.c file by typing ./serverftry. 
Run the clientftry.c file by typing ./clientftry. 
Run make clean to remove all executable files There exists a single server and multiple clients. 
 
EXPECTED OUTPUT: 
Each client represents a process. On running a client following message will pop up: “Enter username: “ Username entered by the user and its client number are used as an identity for that user across the chat system.  
User gets notified after it is connected to the server by the following message: "Connection established to port chhavik_chat_system” Server is constantly listening to each and every client and the message passed by them. 
 
INPUT THE USER NEEDS TO GIVE: 
If the user wants to send a message to client number 3, input from the user side should be: 3 ‘message user wants to send’ (there is no need of single quotes while entering the message) 
If the user wants to send a message to every client, input from the user side should be: A ‘message user wants to send’ (there is no need of single quotes while entering the message) 
 
EXPECTED OUTPUT AND TESTING: 
A message meant for a single user will be sent to that particular user itself and the receiving user will be notified about the sending user’s name and client number as well. 
A message meant for all the user will be sent to all users (except the sending user) and the receiving users will be notified about the sending user’s name and client number as well. 
 
A message received by a user will be of the form: ‘Message’ -message from client: <client number>: <username> 
 
ERRORS HANDLED: 
1. Error in socket creation: Error message- “Cannot create socket” Return value: -1 
2. Error in creating lock file: Error message- “Cannot create socket” Return value: -1 
3. Socket already in use error: Error message- “Socket already in use by another process....exiting" Return value: -1 
4. Error in binding server to the specified port: Error message- “Problem with binding....exiting" Return value: -1 
5. Error in listening: Error message- “Problem in listening....exiting" Return value: -1 
6. Error in connecting to an incoming client: Error message- “Connection failed". 
7. Error in receiving message from a particular client: Error message- “"Error in receiving message”. 
8. INPUT ERROR: if the user enters its own client ID before a message, an error “cannot send message to yourself is thrown”. 
9. INPUT ERROR: if the user enters any alphabet other than a digit or A, an error “wrong input is thrown”. 
  
 
Description of code and logical implementation: 
 
Unix domain sockets are used as the end points of client server client system. 
Unix domain sockets are created using the socket system call using : int cs = socket(AF_UNIX, SOCK_STREAM, 0); 
Path and name of the socket is set using : clname.sun_family=AF_UNIX; strcpy(clname.sun_path,SOCK_PATH); int len =strlen(clname.sun_path)+sizeof(clname.sun_family); 
Each client is alloted a thread in server file and each thread has a function running in the background called clientTask.  
This function is constantly waiting for a message from client and as soon as it gets the message (using recv system call), it sends it to the destined receiver using string parsing (using send system call). 
The first letter is used to judge if the message is meant for a particular user or the whole set of users. 
int recv(int ​socket ​ , char *​buffer ​ , int ​length ​ , int ​flags ​ ); ssize_t send(int​ ​sockfd ​ , const void *​buf ​ , size_t​ ​len ​ , int​ ​flags ​ ); (source: ​https://linux.die.net/man/2/send​ ) 
Rest part of the string is parsed and the actual message is sent to the destined user. 
 
Parsing is done as: First the message is split into space separated words. 
The first word is used to determine the destination using strcmp(first string to compare, second string to compare) method.  
The rest actual message is copied into a string called ‘finalmsg’ using strncpy() inbuilt function. The function takes - 1. 
First argument- the new string to be obtained by parsing the old string. 2. Second argument- ‘old string’ + the index from which onwards the string needs to be copied into the new string. 
Example: msg+2 for copying the after 2nd index msg into finalmsg. 3. Last index of the old string. 

Each client has a thread. Each thread has a function associated with itself. 
This function (clientserverTask) continuously listens(waits) for a message from the server. 
As soon as it gets the message, it prints it on terminal using printf function. 
