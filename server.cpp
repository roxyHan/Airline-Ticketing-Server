// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>


using namespace std;

const int PORT = 5452;
const int DEFAULT_ROW = 10;
const int DEFAULT_COL = 10;

/**
 * Checks if all seats are taken
 * @param seats
 * @param row
 * @param col
 * @return
 */
bool stoppingCriteria(int* seats, int row, int col) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int current = *(seats + i*col + j);
            if (current == 0) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Checks whether a seat is available and marks it accordingly
 * @param seats
 * @param col
 * @param i
 * @param j
 * @return
 */
bool isAvailable(int* seats, int col, int i, int j) {
    int seat = *(seats + i*col + j);
    if (seat == 0) {
        *(seats + i*col + j) = 1;
        return true;
    }
    else {
        cout << "Nope! Seat taken!" << endl;
        return false;
    }
}

/**
 * Displays the map to show the seat availability
 * @param row
 * @param col
 * @param seats
 */
void displayMap(const int row, const int col, int* seats) {
    cout << "\n***  ------------------- ***\n"
            "      Current Seat Map\n"
            "***  ------------------- ***\n"
    << endl;
    cout << "     ";
    for (int idx = 0; idx < col; ++idx) {
        cout << idx << " ";
    }
    cout << "   " << endl;

    for (int i = 0; i < row; ++i) {
        cout << "|" << i << "|  ";
        for (int j = 0; j < col; ++j) {
            int current = *(seats + i*col + j);
            if (current == 1) { // not available
                cout << "X ";
            }
            else if (current == 0) { // available
                cout << "- ";
            }
            else {
                cout << "Issue!" << endl;
            }
        }
        cout << "\n" << endl;
    }
}


int main(int argc, char const *argv[]) {
    int row = 0 , col = 0;
    // (row, col) from the cmd line
    if (argc == 1) {
        row = DEFAULT_ROW;
        col = DEFAULT_COL;
    }
    else if (argc != 3) {
        cout << "Only the number of rows followed by the ones for columns"
                " need to be provided" << endl;
        return EXIT_FAILURE;
    }
    else{
        row = stoi(argv[1]);
        col = stoi(argv[2]);
    }

// -----------------------------------------
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    string hello = "Server says Hiii!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(int))) {
        perror("setsockopt");
        cout << "Trying to use port 8080!Error" << endl;
        return EXIT_FAILURE;
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    // Bind
    if (bind(server_fd, (struct sockaddr *) &address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Accept
    if ((new_socket = accept(server_fd, (struct sockaddr *) &address,
                             (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
//-----------------------------------------------------------------------
    const int r = row;
    const int c = col;


    // Creation of the 2D array for the seats
    int* seats = new int[r * c];
    cout << "Welcome to the ticket booking system!" << endl;
    displayMap(r, c, seats);

    // Send the (row, col) from the server to the client
    int *size = new int[r * c];
    size[0] = r;
    size[1] = c;
    send(new_socket, size, sizeof(size), 0);
    bool stop = stoppingCriteria(seats, r, c);

    while (!stop) {
        // Initialize buffers
        char buffer_msg[255];
        char buffer_request[sizeof(int) * 2];

        valread = read(new_socket, buffer_request, sizeof(int) * 2);

        // Extract the row and col number received from the client
        int rowReq, colReq;
        memcpy(&rowReq, &buffer_request[0], sizeof(int));
        memcpy(&colReq, &buffer_request[4], sizeof(int));

        // Read the message for the seat request obtained from the client
        read(new_socket, buffer_msg, 255);
        //cout << buffer_msg << endl;
        for (int a = 0; a < 255; a++) {
            if (buffer_msg[a] == '.') {
                break;
            } else {
                cout << buffer_msg[a];
            }
        }
        cout << endl;

        // -----------------------------------
        //      Availability of the seat
        // -----------------------------------

        // successful and attach a message to be displayed as well

        bool availability = isAvailable(seats, c, rowReq, colReq);
        string request_reply =  "Seat request on row:";
        request_reply.append(to_string(rowReq));
        request_reply.append(", column:");
        request_reply.append(to_string(colReq));
        if (availability) {
            request_reply.append(" is available. Ticket purchase was successful.!");
        } else {
            request_reply.append(" is not available.\nTicket purchase was denied."
                                 "Try another seat.!");
        }
        // Send response to the seat request to the client
        send(new_socket, request_reply.c_str(), strlen(request_reply.c_str()), 0);
        request_reply.clear();
        displayMap(r, c, seats);

        // Send 0 or 1 to the client depending on whether there are still tickets left
        stop = stoppingCriteria(seats, r, c);
        int x = stop;
        cout << "Are all seats taken? " << x << endl;
        send(new_socket, &x, sizeof(int), 0);
    }

    return 0;
}

