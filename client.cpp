#include <iostream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <csignal>
#include "common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 55555

// global flage to check for connections
bool connected = false;

/****************************************************************************/
// handle Ctrl+C Signal to close the connection with the server if connected
void signal_handler(int signum){
    std::cout << "\nInterrupt signal recived (Ctrl+c)[" << signum << "]\n";
    if(connected){
        // todo close connection
        std::cout << "Handle Closing connection\n";
        connected = false;
    }else{
        std::cout << "Closing Not Connected yet\n";
        exit(EXIT_SUCCESS);
    }
}

/****************************************************************************/
// handle communication with the server
void communication(int client_socket){
    dataStream __buf;
    std::cout << "Waiting Server coms\n";
    while(connected){
        // blocking in recv function untill server send somthing to us 
        recv(client_socket , &__buf , sizeof(__buf) ,0);
        if(__buf.msgId != 0){
            std::cout << "Recv msgId          : "<< __buf.msgId << std::endl;
            std::cout << "Recv payload_length : "<< __buf.payload_length << std::endl;
            std::cout << "Recv payload        : "<< __buf.payload << std::endl;
        }else{
            // server request termination
            std::cout << "Server Request Disconnection\n";
            break;
        }
    }
    // send disconnect message
    memset(&__buf , 0 , sizeof(__buf));
    __buf.msgId = 0;
    strcpy(__buf.payload , "");
    __buf.payload_length = 500;
    send(client_socket , &__buf , sizeof(__buf) , 0);
    // close client socket
    close(client_socket);
}

/****************************************************************************/

int main(){
    // register signal handler
    std::signal(SIGINT,signal_handler);

    // create the socket and connect to the server 
    int client_socket = socket(AF_INET , SOCK_STREAM , 0);
    // handle if socket creation failed
    if (client_socket < 0)
    {
        std::cerr << "Socket Creation Failed\n";
        return -1;
    }

    // connect to the server
    sockaddr_in server_address;
    server_address.sin_family = AF_INET; // ipv4
    server_address.sin_port = htons(SERVER_PORT);
    // check if SERVER_IP is valid for ipv4
    if (inet_pton(AF_INET,SERVER_IP , &server_address.sin_addr) <= 0){
        std::cerr << "Invalid addess formate\n";
        return -1;
    }

    // connecting to server and handling retrying
    while (!connected)
    {
        std::cout << "Connecting...\n";
        if(connect(client_socket ,reinterpret_cast<struct sockaddr*>(&server_address) , sizeof(server_address)) >= 0){
            std::cout << "Connected..\n";
            connected = true;
            break;
        }
        // wait for 5 second and retry
        std::chrono::seconds duration(1);
        std::cout << "Can't Connect to server sleeping for 3 seconds\n";
        std::this_thread::sleep_for(duration);
        std::cout << "Can't Connect to server sleeping for 2 seconds\n";
        std::this_thread::sleep_for(duration);
        std::cout << "Can't Connect to server sleeping for 1 seconds\n";
    }
    
    communication(client_socket);
    return 0;
}
