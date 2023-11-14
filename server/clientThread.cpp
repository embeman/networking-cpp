#include <iostream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <csignal>
#include "common.h"

extern bool server_up;

void clientThread(int client_socket){
    std::cout << "Client Thread : " << std::this_thread::get_id() << std::endl;
    
    std::chrono::seconds duration(1);
    dataStream __buf;
    int i=1;
    while (server_up){
        int recv_bytes = recv(client_socket , &__buf , sizeof(__buf), MSG_DONTWAIT );
        // if recived data handle it else send alive message and sleep
        if (recv_bytes > 0)
        {
            if (__buf.msgId == 0) // disconnect notification from client
            {
                close(client_socket);
                return;
            }
            
        }else{
            // construct __buf and send it every 1 second
            memset(&__buf , 0 , sizeof(__buf));
            __buf.msgId = i++;
            strcpy(__buf.payload , "Hello World From Server\n");
            __buf.payload_length = 500;
            send(client_socket , &__buf , sizeof(__buf) , 0);
            std::this_thread::sleep_for(duration);
        }        
    }
    
    // send termination message to client
    memset(&__buf , 0 , sizeof(__buf));
    __buf.msgId = 0;
    strcpy(__buf.payload , "");
    __buf.payload_length = 500;
    std::cout << "Sent Termination to : " << std::this_thread::get_id() << std::endl;
    send(client_socket , &__buf , sizeof(__buf) , 0);
    close(client_socket);
}


