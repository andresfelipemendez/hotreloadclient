#include "client.h"
#include <iostream>
#include <string>

#define PORT 8080
#define BUFFER_SIZE 1024

bool client_init(void* clientData, void* registry){
    std::cout << "hello from the dll!\n";
    return true;
}

// Function to update the client
void client_update(void* clientData, void* registry){

}

// Function to shutdown the client
void client_shutdown(void* clientData){

}
