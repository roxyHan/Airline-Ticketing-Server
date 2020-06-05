//
// Created by hanifa on 6/3/20.
//


// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>


using namespace std;
int PORT_NUMBER;
string IP_ADDRESS;

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        cout << "Only the IP address and the Port number need to be provided!" << endl;
        return EXIT_FAILURE;
    }
    // TODO: error handling for input
    IP_ADDRESS = argv[1];
    PORT_NUMBER = stoi(argv[2]);


    int sock = 0, valread = 0;
    struct sockaddr_in serv_addr;
    string hello = "Hello from client! Step 1";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUMBER);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, IP_ADDRESS.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed. Wrong parameters. Check IP and Port number \n");
        return -1;
    }
    /**while ((valread = read(sock, buffer, 1024))) {
        printf("%s\n", buffer);
    }*/
    valread = read(sock, buffer, 1024);
    printf("%s\n",buffer);

    send(sock, hello.c_str(), strlen(hello.c_str()), 0);
    printf("Client's message successfully sent\n");

    return 0;
}