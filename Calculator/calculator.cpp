#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>

// Function prototypes
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
std::string calculate(double num1, char op, double num2);
std::string handleRequest(const std::string& request);
void handleClient(int clientSocket);
bool parseJSON(const std::string& json, double& num1, double& num2, char& op);

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // Allow reuse of address
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Calculator HTTP Server running on http://localhost:8080" << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        // Handle client in a separate thread for concurrent requests
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    return a / b;
}

std::string calculate(double num1, char op, double num2) {
    std::ostringstream response;
    
    switch (op) {
        case '+':
            response << "{\"result\":" << std::fixed << std::setprecision(10) << add(num1, num2) << ",\"success\":true}";
            break;
        case '-':
            response << "{\"result\":" << std::fixed << std::setprecision(10) << subtract(num1, num2) << ",\"success\":true}";
            break;
        case '*':
            response << "{\"result\":" << std::fixed << std::setprecision(10) << multiply(num1, num2) << ",\"success\":true}";
            break;
        case '/':
            if (num2 == 0) {
                response << "{\"error\":\"Division by zero!\",\"success\":false}";
            } else {
                response << "{\"result\":" << std::fixed << std::setprecision(10) << divide(num1, num2) << ",\"success\":true}";
            }
            break;
        default:
            response << "{\"error\":\"Invalid operator!\",\"success\":false}";
    }
    
    return response.str();
}

void handleClient(int clientSocket) {
    char buffer[4096] = {0};
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }
    
    std::string request(buffer);
    std::string response = handleRequest(request);
    
    send(clientSocket, response.c_str(), response.length(), 0);
    close(clientSocket);
}

std::string handleRequest(const std::string& request) {
    std::istringstream iss(request);
    std::string line;
    std::getline(iss, line);
    
    // CORS headers for browser compatibility
    std::string headers = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json\r\n"
                         "Access-Control-Allow-Origin: *\r\n"
                         "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                         "Access-Control-Allow-Headers: Content-Type\r\n"
                         "\r\n";
    
    // Handle OPTIONS request (CORS preflight)
    if (line.find("OPTIONS") != std::string::npos) {
        return headers;
    }
    
    // Serve the HTML file for GET requests
    if (line.find("GET / ") != std::string::npos || line.find("GET /index.html") != std::string::npos) {
        std::string htmlHeaders = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Access-Control-Allow-Origin: *\r\n"
                                 "\r\n";
        
        // Read and serve the HTML file
        std::ifstream file("index.html");
        if (file.is_open()) {
            std::string html((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
            return htmlHeaders + html;
        } else {
            return "HTTP/1.1 404 Not Found\r\n\r\nHTML file not found";
        }
    }
    
    // Handle POST requests for calculations
    if (line.find("POST") != std::string::npos) {
        // Find the JSON body
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart != std::string::npos) {
            std::string body = request.substr(bodyStart + 4);
            
            try {
                double num1, num2;
                char op;
                
                if (parseJSON(body, num1, num2, op)) {
                    std::string result = calculate(num1, op, num2);
                    return headers + result;
                } else {
                    return headers + "{\"error\":\"Invalid JSON format\",\"success\":false}";
                }
            } catch (const std::exception& e) {
                return headers + "{\"error\":\"Invalid JSON format\",\"success\":false}";
            }
        }
    }
    
    // Default response
    return headers + "{\"error\":\"Invalid request\",\"success\":false}";
}

bool parseJSON(const std::string& json, double& num1, double& num2, char& op) {
    try {
        // Simple JSON parsing for our specific format
        // Expected format: {"num1":123,"operator":"+","num2":456}
        
        size_t num1Pos = json.find("\"num1\":");
        size_t opPos = json.find("\"operator\":\"");
        size_t num2Pos = json.find("\"num2\":");
        
        if (num1Pos == std::string::npos || opPos == std::string::npos || num2Pos == std::string::npos) {
            return false;
        }
        
        // Extract num1
        num1Pos += 7; // Skip "num1":
        size_t num1End = json.find(",", num1Pos);
        if (num1End == std::string::npos) return false;
        std::string num1Str = json.substr(num1Pos, num1End - num1Pos);
        num1 = std::stod(num1Str);
        
        // Extract operator
        opPos += 12; // Skip "operator":"
        if (opPos >= json.length()) return false;
        op = json[opPos];
        
        // Extract num2
        num2Pos += 7; // Skip "num2":
        size_t num2End = json.find("}", num2Pos);
        if (num2End == std::string::npos) num2End = json.length();
        std::string num2Str = json.substr(num2Pos, num2End - num2Pos);
        num2 = std::stod(num2Str);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
