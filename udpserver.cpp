#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <vector>
#include "person.h"

int main(int argc, char **argv){
    int serverSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    unsigned short port;

    // Create UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure settings of the server address struct
    serverAddr.sin_family = AF_INET;
    port = (unsigned short)atoi(argv[1]);
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    // Bind the address struct to the socket
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    addr_size = sizeof serverAddr;

    while (true) {
        char buff[64] = {0};
        std::string reply;
        
        // Receive request from client
        ssize_t len = recvfrom(serverSocket, &buff, 64, 0, (struct sockaddr *)&serverAddr, &addr_size);
        
        // Process request
        if (buff[0] == 0x01) { // Add Student to database
            if (len != 26) {
                std::cout << "corrupted add packet: bad size" << std::endl;
            } else {
                Person p = from_packet(buff + 1);
                addPerson(p);
                std::cout << "added person with id:" << p.id << std::endl;
            }
        } else if (buff[0] == 0x02) { // Get student based off Student ID
            if (len != 7) {
                std::cout << "corrupted get by ID packet: bad size" << std::endl;
            } else {
                std::string targetID;
                for (int i = 1; i < 7; i++) {
                    targetID += buff[i];
                }

                Person p;
                if (getPerson(targetID, p)) {
                    reply = to_packet(p);
                }
            }
        } else if (buff[0] == 0x03) { // Display All students whose Score is > client supplied score
            if (len != 2) {
                std::cout << "corrupted get all by grade packet: bad size" << std::endl;
            } else {
                int targetScore = buff[1];
                std::vector<Person> studentsWithHigherGrades = getPeopleWithScore(targetScore);
                for (int i = 0; i < studentsWithHigherGrades.size(); i++) {
                    reply += to_packet(studentsWithHigherGrades[i]);
                }
            }
        } else if (buff[0] == 0x04) { // Display all Students
            std::vector<Person> allStudents = getPeople();
            for (int i = 0; i < allStudents.size(); i++) {
                reply += to_packet(allStudents[i]);
            }
        } else if (buff[0] == 0x05) { // Remove individual based on ID
            if (len != 7) {
                std::cout << "corrupted removal packet: bad size" << std::endl;
            } else {
                std::string targetID;
                for (int i = 1; i < 7; i++) {
                    targetID += buff[i];
                }
                Person p;
                if (getPerson(targetID, p)) {
                    removePerson(targetID);
                    reply = to_packet(p);
                }
            }
        } else if (buff[0] == 0x06) { // Quit Option
            // Break the loop to quit, i.e., close socket
            break;
        }

        // Send response to client
        sendto(serverSocket, reply.c_str(), reply.size(), 0, (struct sockaddr *)&serverAddr, addr_size);
    }

    // Close the socket
    close(serverSocket);

    return 0;
}
