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

// Function to decrypt a message using Playfair cipher
string messageDecryption(const string& msg) 
{
    string decryptMsg;
    for (int i = 0; i < msg.length(); i += 2) 
    {
        char c1 = toupper(msg[i]);
        char c2 = toupper(msg[i + 1]);
        if (c1 != ' ' && c2 != ' ') 
        {
            int rowNo1, colNo1, rowNo2, colNo2;
            rowNo1 = colNo1 = rowNo2 = colNo2 = -1;

            // Find the positions of both characters in the matrix based on the conditions of PlayFair
            for (int row = 0; row < 5; row++) 
            {
                for (int col = 0; col < 5; col++) 
                {
                    if (playfairMatrix[row][col] == c1) 
                    {
                        rowNo1 = row;
                        colNo1 = col;
                    }
                    if (playfairMatrix[row][col] == c2) 
                    {
                        rowNo2 = row;
                        colNo2 = col;
                    }
                }
            }
            // Decrypt the characters based on Playfair rules
            if (rowNo1 == rowNo2) 
            {
                colNo1 = (colNo1 - 1 + 5) % 5;
                colNo2 = (colNo2 - 1 + 5) % 5;
            } else if (colNo1 == colNo2)
            {
                rowNo1 = (rowNo1 - 1 + 5) % 5;
                rowNo2 = (rowNo2 - 1 + 5) % 5;
            } 
            else 
            {
                swap(colNo1, colNo2);
            }

            decryptMsg += playfairMatrix[rowNo1][colNo1];
            decryptMsg += playfairMatrix[rowNo2][colNo2];
        } 
        else 
        {
            decryptMsg += ' ';
        }
    }

    return decryptMsg;
}

int main() {
    int serverSocket;
    int clientSocket;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    struct sockaddr_in serverAddressHolder, clientAddressHolder;
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //General checks
    if (serverSocket < 0) 
    {
        cerr << "Allas!!!! We Got an Error in socket creation." << endl;
        return 1;
    }

    serverAddressHolder.sin_family = AF_INET;
    serverAddressHolder.sin_port = htons(29876);
    serverAddressHolder.sin_addr.s_addr = INADDR_ANY;


    // Listen for connections
    if (listen(serverSocket, 10) == 0)
    {
        cout << "Hello From Server, I am Listening..." << endl;
    } else 
    {
        cerr << "Allas!!!! We Got an Error in listening." << endl;
        return 1;
    }
    // Check for Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddressHolder, sizeof(serverAddressHolder)) < 0) 
    {
        cerr << "Allas!!!! We Got an Error in binding." << endl;
        return 1;
    }
    while (1) 
    {
        // Accept client connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddressHolder, &addrSize);
        if (clientSocket < 0) 
		{
            cerr << "Allas!!!!Error in accepting connection." << endl;
            return 1;
        }

        char buffer[1024];
        int find = read(clientSocket, buffer, sizeof(buffer));
        if (find < 0)
		{
            cerr << "Allas!!!!Error in reading from client." << endl;
            return 1;
        }

        // Convert the received message to string
        string receivedMsg(buffer, find);
        // Decrypt the received message using the Playfair matrix
        string decryptMsg = messageDecryption(receivedMsg);
        int countLetters = 0;
        for (char c : decryptMsg) 
		{
            if (isalpha(c)) 
			{
                countLetters++;
            }
        }
        // Send the response back to the client
        string reply = "Message received of " + to_string(countLetters) + " letters";
        write(clientSocket, reply.c_str(), reply.size());

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
