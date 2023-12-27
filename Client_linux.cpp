#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cctype>
using namespace std;

char playfairMatrix[5][5] = {
    {'M', 'F', 'H', 'I', 'K'},
    {'U', 'N', 'O', 'P', 'Q'},
    {'Z', 'V', 'W', 'X', 'Y'},
    {'E', 'L', 'A', 'R', 'G'},
    {'D', 'S', 'T', 'B', 'C'}
};

// Function to encrypt a message using Playfair cipher Rules heere
string encryptMessage(const string& msg) 
{
    string encryptedMsg;
    for (char c : msg) 
    {
        // Cjeck for Ignore spaces
        if (c != ' ') 
        { 
            c = toupper(c); 
            int row1, col1, row2, col2;
            row1 = col1 = row2 = col2 = -1;
            // Find the positions of both characters in the matrix using the Playfair rules
            for (int row = 0; row < 5; row++) 
            {
                for (int col = 0; col < 5; col++) 
                {
                    if (playfairMatrix[row][col] == c) 
                    {
                        row1 = row;
                        col1 = col;
                    }
                    if (playfairMatrix[row][col] == ' ') 
                    {
                        row2 = row;
                        col2 = col;
                    }
                }
            }

            // Encrypt the characters based on Playfair rules
            if (row1 == row2) 
            {
                col1 = (col1 + 1) % 5;
                col2 = (col2 + 1) % 5;
            } 
            else if (col1 == col2) 
            {
                row1 = (row1 + 1) % 5;
                row2 = (row2 + 1) % 5;
            } 
            else 
            {
                swap(col1, col2);
            }

            encryptedMsg += playfairMatrix[row1][col1];
            encryptedMsg += playfairMatrix[row2][col2];
        } 
        else
        {
            encryptedMsg += ' ';
        }
    }

    return encryptedMsg;
}

int main() 
{
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) 
    {
        cerr << "Allas!!!Error in socket creation." << endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(29876);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        cerr << "Allas!!Error in connection." << endl;
        return 1;
    }

    string msg = "We are assigned the second homework in this course";
    string encryptedMsg = encryptMessage(msg);

    // Send the encrypted message to the server
    write(clientSocket, encryptedMsg.c_str(), encryptedMsg.size());

    char buffer[1024];
    int received = read(clientSocket, buffer, sizeof(buffer));
    if (received < 0) {
        cerr << "Allas!!Error in reading from server." << endl;
        return 1;
    }

    buffer[received] = '\0';
    cout << "Server response is: " << buffer << endl;

    close(clientSocket);
    return 0;
}
