#include <iostream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <csignal>
#include <vector>
#include <iterator>
#include <mutex>
#include <netinet/in.h>
#include <fcntl.h>

#include "common.h"


using namespace std;

#define SERVER_PORT 55555


std::vector<std::thread> client_threads;


// server state Protection
/****************************************************************************/
static std::mutex server_up_mutex;
bool server_up = false;
int client_count =0;
void set_server_up(){
    std::lock_guard<std::mutex> lock(server_up_mutex);
    server_up = true;
    // mutex unlocked whene goes out of scope
}

void set_server_shutdown(){
    std::lock_guard<std::mutex> lock(server_up_mutex);
    server_up = false;
    // mutex unlocked whene goes out of scope
}

int get_server_state(){
    std::lock_guard<std::mutex> lock(server_up_mutex);
    if(server_up == true){
        return 1;
    }else{
        return 0;
    }
}

/****************************************************************************/
// handle Ctrl+C Signal to close the connection with the server if connected
void signal_handler(int signum){
    cout << "\nInterrupt signal recived (Ctrl+c)[" << signum << "]\n";
    cout << "Handle Closing connection\n";
    set_server_shutdown();
    std::cout << "Server Status : " << get_server_state() << std::endl;
    if(client_count ==0){
        exit(EXIT_SUCCESS);
    }
}
/****************************************************************************/

// handle communication with the client
extern void clientThread(int client_socket);

int main(){
    // register signal handler
    std::signal(SIGINT,signal_handler);

    // create the socket 
    int server_socket = socket(AF_INET , SOCK_STREAM , 0);

    // handle if socket creation failed
    if (server_socket < 0)
    {
        std::cerr << "Socket Creation Failed\n";
        return -1;
    }

    // bind socket to address and listen to socket
    sockaddr_in server_address;
    server_address.sin_family = AF_INET; // ipv4
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // binding
    if(bind(server_socket , (struct sockaddr*)&server_address,sizeof(server_address)) < 0){
        std::cerr << "Error binding\n";
        return -1;
    }


    // Set the socket to non-blocking
    if (fcntl(server_socket, F_SETFL, O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    // listing to the socket
    if(listen(server_socket , 5) < 0){
        std::cerr << "Can't listen to the socket\n";
        close(server_socket);
        return -1;
    }

    // making accept function non-blocking

    // server is up because we are listening on the port
    set_server_up();

    while(get_server_state()){
        int client_socket =-1;
        
        cout << "Waiting for Client Connection..\n";
        while ((client_socket <= 0) && get_server_state() == 1 ){
            client_socket = accept(server_socket,NULL , NULL);
        }
        
        if(client_socket < 0){
            cout << "Can't Accept New Client Connection..\n";
            break;
        }

        cout << "Connected..\n";
        // Start a separate thread for communication
        client_threads.emplace_back(clientThread, client_socket);
        client_threads.back().detach();
        client_count++;
    }

    std::cout << "Waiting threads to finish\n" << "client_count :" <<client_count << std::endl;
    for(auto& thread : client_threads){
        if(thread.joinable())
            thread.join();
    }

    close(server_socket);

    return 0;
}
