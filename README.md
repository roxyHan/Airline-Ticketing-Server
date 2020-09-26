# Airline-Ticketing-Server

This program simulates an airline ticketing system using TCP/IP protocol.
The server and client communicate back and forth through sockets.

## Server
Sells the tickets and displays a map for the seats' availability

## Client
02 modes of operations:<br/>
#### Manual 
--> Input row and col number to select a seat when prompted to do so; <br/>
Sends the request for that ticket purchase to the server and waits for response from the server
<br/>
#### Automatic
--> Random seat numbers are generated and a request for ticket purchase is sent out to the server for confirmation 


## Compile
1. Open two Linux terminals
2. Compile the server in one terminal <br />
g++ -pthread -o server server.cpp
3. Compile the client in the other terminal <br/>
g++ client.cpp -o client

## Run 
Server: You can choose to specify the row * columns. If no size is provided, the default seat map is 10 * 10 <br />
##### ./server 2 3
or
##### ./server 

Client: Add the IP address followed by the port number and the mode of operation
<br /> IP address: 127.0.0.1
<br /> Port number: 5452
<br /> Mode:<br />
      &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;     1. manual or m 
      <br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;       2. automatic or a

##### ./client 127.0.0.1 5452 a



