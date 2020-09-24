# Airline-Ticketing-Server

This program simulates an airline ticketing system using TCP/IP protocol.
The server and client communicate back and forth through sockets.

## Server
Sells the tickets and displays a map for the seats' availability

## Client
02 modes of operations:<br/>
Manual --> Input row and col number to select a seat when prompted to do so; <br/>
Sends the request for that ticket purchase to the server and waits for response from the server
<br/>Automatic --> Random seat numbers are generated and a request for ticket purchase is sent out to the server for confirmation 
