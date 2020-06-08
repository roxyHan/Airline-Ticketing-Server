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
#include <algorithm>


using namespace std;
int PORT_NUMBER;
string IP_ADDRESS;
string MODE;
int maxRow, maxCol;
int rowSize, colSize;



int* manual() {
    int* pair = new int[2];
    // Get (row,col) from the console
    string row_number, col_number;
    cout << "Please enter the row number: "<< endl;
    getline(cin, row_number);
    int row = stoi(row_number);
    cout << "Please enter the column number: "<< endl;
    getline(cin, col_number);
    int col = stoi(col_number);

    // Send it to the server

    // Back to main
    pair[0] = row;
    pair[1] = col;
    return pair;
}


int* automatic() {
    int* pair = new int[2];
    // Generate (row, col) randomly
    int row, col;
    row = rand() % rowSize;
    col = rand() % colSize;

    // Send it to the server


    // Back to main
    pair[0] = row;
    pair[1] = col;
    sleep(2);
    return pair;
}


int main(int argc, char const *argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        cout << "Please provide only the following in the given order:\n"
                "the IP address,\n"
                "the Port number,\n"
                "the mode of execution:\n"
                "\t\tuse 'manual' or 'm' for manual mode\n'"
                "\t\tuse 'automatic' or 'a' for automatic mode"
        << endl;
        return EXIT_FAILURE;
    }

    // TODO: error handling for input
    IP_ADDRESS = argv[1];
    PORT_NUMBER = stoi(argv[2]);
    MODE = argv[3];
    transform(MODE.begin(), MODE.end(), MODE.begin(), ::tolower);
    // Get the mode of execution
    /**
    if (MODE == "m" || MODE == "manual") {
        manual();
    }
    else if (tolower(MODE[0]) == 'a') {
        automatic();
    }
    else {
        cout << "A mode of execution was not selected; Default mode is automatic.." << endl;
        automatic();
    }
*/
    // --------------------------------------------------------------------------
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

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
    // ---------------------------------------------------------------------------

    bool stop = false;
    char dimensions[sizeof(int) *2];

    // Get the dimensions (maxRow, maxColumn) from the server
    read(sock, dimensions, sizeof(dimensions));
    memcpy(&maxRow, &dimensions[0], sizeof(int));
    memcpy(&maxCol, &dimensions[4], sizeof(int));
    cout << "Seats Map " << maxRow << "*" << maxCol << endl;

    while (!stop) {
        char response[255];

        cout << "\n";
        int *pair = manual();
        int i = pair[0];
        int j = pair[1];

        int x;
        // Send (row, col) to the server
        send(sock, pair, sizeof(pair), 0);

        // Send message for the seat request
        string request = "Client's request successfully received for seat: row ";
        request.append(to_string(i));
        request.append(", column ");
        request.append(to_string(j));
        request.append(".\0");
        send(sock, request.c_str(), strlen(request.c_str()), 0);

        // Read the response from the server to our seat request
        read(sock, response, 255);
        for (int a = 0; a < 255; a++) {
            if (response[a] == '!') {
                break;
            } else {
                cout << response[a];
            }
        }
        cout << endl;
        request.clear();

        // Read whether there are still tickets available for purchase
        read(sock, &x, sizeof(int));
        cout << "The value for stop is: " << stop << endl;
    }

    return 0;
}


