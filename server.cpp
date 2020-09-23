// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include "pthread.h"


using namespace std;

pthread_mutex_t lock;
const int PORT = 5452;
const int DEFAULT_ROW = 10;
const int DEFAULT_COL = 10;
int row = 0 , col = 0;
bool stop;
int server_fd, new_socket, valread;


/**
 * Checks if all seats are taken
 * @param seats
 * @param row
 * @param col
 * @return
 */
bool stoppingCriteria(int* seats, int row, int col) {
    pthread_mutex_lock(&lock);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int current = *(seats + i*col + j);
            if (current == 0) {
                pthread_mutex_unlock(&lock);
                return false;
            }
        }
    }
    pthread_mutex_unlock(&lock);
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
    pthread_mutex_lock(&lock);
    int seat = *(seats + i*col + j);
    if (seat == 0) {
        cout << "Seat available" << endl;
        *(seats + i*col + j) = 1;
        pthread_mutex_unlock(&lock);
        return true;
    }
    else {
        cout << "Sorry! Seat taken!" << endl;
        pthread_mutex_unlock(&lock);
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
    pthread_mutex_lock(&lock);

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
    pthread_mutex_unlock(&lock);

}

static void* interaction(void* s) {

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
    stop = stoppingCriteria(seats, r, c);

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
        pthread_mutex_lock(&lock);
        read(new_socket, buffer_msg, 255);
        for (int a = 0; a < 255; a++) {
            if (buffer_msg[a] == '.') {
                break;
            } else {
                cout << buffer_msg[a];
            }
        }
        cout << endl;
        pthread_mutex_unlock(&lock);

        // -----------------------------------
        //      Availability of the seat
        // -----------------------------------

        // successful and attach a message to be displayed as well

        bool availability = isAvailable(seats, c, rowReq, colReq);
        string request_reply =  "Seat on row:";
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
        pthread_mutex_lock(&lock);
        send(new_socket, request_reply.c_str(), strlen(request_reply.c_str()), 0);
        pthread_mutex_unlock(&lock);
        request_reply.clear();
        displayMap(r, c, seats);

        // Send 0 or 1 to the client depending on whether there are still tickets left
        stop = stoppingCriteria(seats, r, c);
        int x = stop;
        cout << "Are all seats taken? ";
        if (x == 0) {
            cout << "No" << endl;
        }
        else if (x == 1) {
            cout << "Yes" << endl;
        }
        pthread_mutex_lock(&lock);
        send(new_socket, &x, sizeof(int), 0);
        pthread_mutex_unlock(&lock);
    }

//    pthread_mutex_unlock(&lock);
    return (void*) 0;
}


bool inputCheck(std::string input) {
    // Check for wrong input type
    if (input.length() == 0 || input.length() > 7) {
        std::cout << "Please provide a positive integer of reasonable size." << std::endl;
        return false;
    }


    for (int idx = 0; idx < input.length(); ++idx) {
        if (!isdigit(input[idx])) {
            std::cout << "Please only provide a positive integer." << std::endl;
            return false;
        }
    }
    if (stoi(input) == 0) {
        std::cout << "The number of elements to be generated should be greater than 0."<< std::endl;
        return false;
    }
    return true;
}


int main(int argc, char const *argv[]) {
    // (row, col) from the cmd line
    if (argc == 1) {
        row = DEFAULT_ROW;
        col = DEFAULT_COL;
    }
    else if (argc == 2) {
        col = DEFAULT_COL;
    }
    else if (argc != 3) {
        cout << "Only the number of rows followed by the ones for columns"
                " need to be provided" << endl;
        return EXIT_FAILURE;
    }
    else{
        bool row_valid = inputCheck(argv[1]);
        bool col_valid = inputCheck(argv[2]);
        if (!(row_valid && col_valid)) {
            cout << "Row and/or Column are not valid." << endl;
            return EXIT_FAILURE;
        }
        row = stoi(argv[1]);
        col = stoi(argv[2]);
    }

// -----------------------------------------
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(int))) {
        perror("setsockopt");
        cout << "Trying to use a wrong port !Error" << endl;
        return EXIT_FAILURE;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

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

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed for lock 1\n");
        return 1;
    }

    //interaction(0);

    pthread_t threadClient;
    int g = 0;

    while (!stop) {
        pthread_create(&threadClient, NULL, interaction, (void *) (long) g);
        pthread_join(threadClient, NULL);
    }

    pthread_mutex_destroy(&lock);

    return 0;
}

