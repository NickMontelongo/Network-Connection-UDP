#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <limits>
#include "person.h"
using namespace std;

string getStringWithSizeRange(const string& prompt, int minSize, int maxSize) {
    string input;
    do {
        cout << prompt;
        cin >> input;
        if (input.length() < minSize || input.length() > maxSize) {
            cout << "Invalid input size. Please enter a string of length between "
                 << minSize << " and " << maxSize << "." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    } while (true);
    return input;
}

int getIntInRange(const string& prompt, int minVal, int maxVal) {
    int input;
    do {
        cout << prompt;
        cin >> input;
        if (cin.fail() || input < minVal || input > maxVal) {
            cout << "Invalid input. Please enter an integer between "
                 << minVal << " and " << maxVal << "." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    } while (true);
    return input;
}

int main(int argc, char **argv){

    int clientSocket;
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    unsigned short port;
    struct hostent *hostnm;

    uint32_t num, cnum;
    int option = 0;

    do {
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

        serverAddr.sin_family = AF_INET;
        port = (unsigned short)atoi(argv[2]);
        serverAddr.sin_port = htons(port);

        hostnm = gethostbyname(argv[1]);
        serverAddr.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

        addr_size = sizeof serverAddr;

        std::cout << "\nMenu:\n";
        std::cout << "1: Add Student\n";
        std::cout << "2: Display Student by ID\n";
        std::cout << "3: Display Student by Score\n";
        std::cout << "4: Display All Students\n";
        std::cout << "5: Delete Student by ID\n";
        std::cout << "6: Exit\n";
        std::cout << "Enter your choice (1-6): ";
        std::cin >> option;

        std::string msg;

        cnum = htonl(option);

        switch (option) {
            case 1:
                {
                    string studentID, firstName, lastName, scoreStr;
                    studentID = getStringWithSizeRange("Enter student ID (6 characters): ",6,6);
                    firstName = getStringWithSizeRange("Enter first name (max 9 characters): ",1,9);
                    lastName = getStringWithSizeRange("Enter last name (max 9 characters): ",1,9);
                    scoreStr= getIntInRange("Enter student score from (0-100): ",0,100);

                    Person personToAdd;
                    personToAdd.id = studentID;
                    personToAdd.firstName = firstName;
                    personToAdd.lastName = lastName;
                    personToAdd.score = scoreStr[0];

                    msg += 0x01;
                    msg += to_packet(personToAdd);
                }
                break;
            case 2:
                {
                    string studentIDToSearch = getStringWithSizeRange("Enter a student ID to Search(6 characters): ",6,6);

                    msg += 0x02;
                    msg += studentIDToSearch;
                }
                break;
            case 3:
                {
                    string prompt = "Please enter a score between (0-100), to show students who have a score above said value: ";
                    int scoreToCompare = getIntInRange(prompt,0,100);

                    msg += 0x03;
                    msg += scoreToCompare;
                }
                break;
            case 4:
                {
                    msg += 0x04;
                }
                break;
            case 5:
                {
                    string studentIDToSearch = getStringWithSizeRange("Enter a student ID for deletion(6 characters): ",6,6);
                    msg += 0x05;
                    msg += studentIDToSearch;
                }
                break;
            case 6:
                std::cout << "Exiting Program\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }

        if (sendto(clientSocket, msg.c_str(), msg.size(), 0, (struct sockaddr *)&serverAddr, addr_size) < 0) {
            throw std::invalid_argument("failed to send");
        }

        char buff[65535] = {0};
        ssize_t len = recvfrom(clientSocket, buff, sizeof(buff), 0, NULL, NULL);

        for(int i = 0; i < len / 25; i++) {
            Person p = from_packet(buff + i * 25);
            std::cout << p.firstName << " " << p.lastName << " " << p.id << " " << p.score << endl;
        }

        close(clientSocket);

    } while (option != 6);

    return 0;
}