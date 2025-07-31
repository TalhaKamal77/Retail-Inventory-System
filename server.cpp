#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

class InventoryItem {
public:
    std::string id;
    std::string name;
    std::string description;
    int quantity;
    double price;
    std::string category;
    std::string dateAdded;

    InventoryItem() : quantity(0), price(0.0) {}
    
    InventoryItem(const std::string& n, const std::string& desc, int qty, double prc, const std::string& cat)
        : name(n), description(desc), quantity(qty), price(prc), category(cat) {
        id = generateId();
        dateAdded = getCurrentDate();
    }

private:
    std::string generateId() {
        static int counter = 1;
        return "ITEM" + std::to_string(counter++);
    }

    std::string getCurrentDate() {
        time_t now = time(0);
        struct tm* ltm = localtime(&now);
        return std::to_string(1900 + ltm->tm_year) + "-" + 
               std::to_string(1 + ltm->tm_mon) + "-" + 
               std::to_string(ltm->tm_mday);
    }
};

class InventorySystem {
private:
    std::map<std::string, InventoryItem> items;
    std::string dataFile;

public:
    InventorySystem(const std::string& filePath) : dataFile(filePath) {
        loadData();
    }

    void addItem(const std::string& name, const std::string& description, 
                 int quantity, double price, const std::string& category) {
        InventoryItem item(name, description, quantity, price, category);
        items[item.id] = item;
        saveData();
    }

    void updateItem(const std::string& id, const std::string& name, 
                   const std::string& description, int quantity, 
                   double price, const std::string& category) {
        if (items.find(id) != items.end()) {
            items[id].name = name;
            items[id].description = description;
            items[id].quantity = quantity;
            items[id].price = price;
            items[id].category = category;
            saveData();
        }
    }

    void deleteItem(const std::string& id) {
        items.erase(id);
        saveData();
    }

    std::vector<InventoryItem> getAllItems() {
        std::vector<InventoryItem> result;
        for (const auto& pair : items) {
            result.push_back(pair.second);
        }
        return result;
    }

    InventoryItem* getItem(const std::string& id) {
        auto it = items.find(id);
        return it != items.end() ? &it->second : nullptr;
    }

    std::string toJson() {
        std::stringstream ss;
        ss << "[";
        bool first = true;
        for (const auto& pair : items) {
            if (!first) ss << ",";
            ss << "{";
            ss << "\"id\":\"" << pair.second.id << "\",";
            ss << "\"name\":\"" << escapeJson(pair.second.name) << "\",";
            ss << "\"description\":\"" << escapeJson(pair.second.description) << "\",";
            ss << "\"quantity\":" << pair.second.quantity << ",";
            ss << "\"price\":" << pair.second.price << ",";
            ss << "\"category\":\"" << escapeJson(pair.second.category) << "\",";
            ss << "\"dateAdded\":\"" << pair.second.dateAdded << "\"";
            ss << "}";
            first = false;
        }
        ss << "]";
        return ss.str();
    }

private:
    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else if (c == '\r') result += "\\r";
            else if (c == '\t') result += "\\t";
            else result += c;
        }
        return result;
    }

    void saveData() {
        std::ofstream file(dataFile);
        if (file.is_open()) {
            file << toJson();
            file.close();
        }
    }

    void loadData() {
        std::ifstream file(dataFile);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            parseJson(content);
        }
    }

    void parseJson(const std::string& json) {
        // Simple JSON parser for our specific format
        size_t pos = 0;
        while ((pos = json.find("\"id\":\"", pos)) != std::string::npos) {
            pos += 6; // Skip "id":"
            size_t endPos = json.find("\"", pos);
            if (endPos == std::string::npos) break;
            std::string id = json.substr(pos, endPos - pos);
            
            InventoryItem item;
            item.id = id;
            
            // Parse other fields
            pos = json.find("\"name\":\"", endPos);
            if (pos != std::string::npos) {
                pos += 8;
                endPos = json.find("\"", pos);
                if (endPos != std::string::npos) {
                    item.name = json.substr(pos, endPos - pos);
                }
            }
            
            pos = json.find("\"description\":\"", endPos);
            if (pos != std::string::npos) {
                pos += 15;
                endPos = json.find("\"", pos);
                if (endPos != std::string::npos) {
                    item.description = json.substr(pos, endPos - pos);
                }
            }
            
            pos = json.find("\"quantity\":", endPos);
            if (pos != std::string::npos) {
                pos += 11;
                endPos = json.find(",", pos);
                if (endPos != std::string::npos) {
                    item.quantity = std::stoi(json.substr(pos, endPos - pos));
                }
            }
            
            pos = json.find("\"price\":", endPos);
            if (pos != std::string::npos) {
                pos += 8;
                endPos = json.find(",", pos);
                if (endPos != std::string::npos) {
                    item.price = std::stod(json.substr(pos, endPos - pos));
                }
            }
            
            pos = json.find("\"category\":\"", endPos);
            if (pos != std::string::npos) {
                pos += 12;
                endPos = json.find("\"", pos);
                if (endPos != std::string::npos) {
                    item.category = json.substr(pos, endPos - pos);
                }
            }
            
            pos = json.find("\"dateAdded\":\"", endPos);
            if (pos != std::string::npos) {
                pos += 13;
                endPos = json.find("\"", pos);
                if (endPos != std::string::npos) {
                    item.dateAdded = json.substr(pos, endPos - pos);
                }
            }
            
            items[id] = item;
        }
    }
};

class HttpServer {
private:
    int serverSocket;
    InventorySystem inventory;
    std::string address;
    int port;

public:
    HttpServer(const std::string& addr, int p, const std::string& dataPath) 
        : address(addr), port(p), inventory(dataPath) {
        #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
        
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            return;
        }

        if (listen(serverSocket, 10) < 0) {
            std::cerr << "Error listening on socket" << std::endl;
            return;
        }

        std::cout << "Server started on " << address << ":" << port << std::endl;
    }

    void run() {
        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            
            if (clientSocket < 0) {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }

            std::thread(&HttpServer::handleClient, this, clientSocket).detach();
        }
    }

private:
    void handleClient(int clientSocket) {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            
            std::string response = processRequest(request);
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        
        #ifdef _WIN32
        closesocket(clientSocket);
        #else
        close(clientSocket);
        #endif
    }

    std::string processRequest(const std::string& request) {
        std::string method, path;
        std::istringstream iss(request);
        iss >> method >> path;

        if (method == "GET") {
            if (path == "/" || path == "/index.html") {
                return serveFile("index.html", "text/html");
            } else if (path == "/style.css") {
                return serveFile("style.css", "text/css");
            } else if (path == "/script.js") {
                return serveFile("script.js", "text/javascript");
            } else if (path == "/api/items") {
                return createJsonResponse(inventory.toJson());
            }
        } else if (method == "POST") {
            if (path == "/api/items") {
                return handleAddItem(request);
            }
        } else if (method == "PUT") {
            if (path.find("/api/items/") == 0) {
                return handleUpdateItem(path, request);
            }
        } else if (method == "DELETE") {
            if (path.find("/api/items/") == 0) {
                return handleDeleteItem(path);
            }
        }

        return createErrorResponse(404, "Not Found");
    }

    std::string serveFile(const std::string& filename, const std::string& contentType) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return createErrorResponse(404, "File not found");
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();

        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "; charset=utf-8\r\n";
        response += "Content-Length: " + std::to_string(content.length()) + "\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE\r\n";
        response += "Access-Control-Allow-Headers: Content-Type\r\n";
        response += "\r\n";
        response += content;

        return response;
    }

    std::string createJsonResponse(const std::string& json) {
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: application/json; charset=utf-8\r\n";
        response += "Content-Length: " + std::to_string(json.length()) + "\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE\r\n";
        response += "Access-Control-Allow-Headers: Content-Type\r\n";
        response += "\r\n";
        response += json;

        return response;
    }

    std::string createErrorResponse(int code, const std::string& message) {
        std::string response = "HTTP/1.1 " + std::to_string(code) + " " + message + "\r\n";
        response += "Content-Type: text/plain; charset=utf-8\r\n";
        response += "Content-Length: " + std::to_string(message.length()) + "\r\n";
        response += "Access-Control-Allow-Origin: *\r\n";
        response += "\r\n";
        response += message;

        return response;
    }

    std::string handleAddItem(const std::string& request) {
        // Extract JSON from request body
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart == std::string::npos) {
            return createErrorResponse(400, "Bad Request");
        }

        std::string body = request.substr(bodyStart + 4);
        // Simple JSON parsing for add item
        std::string name, description, category;
        int quantity = 0;
        double price = 0.0;

        // Parse JSON fields (simplified)
        size_t pos = body.find("\"name\":\"");
        if (pos != std::string::npos) {
            pos += 8;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                name = body.substr(pos, endPos - pos);
            }
        }

        pos = body.find("\"description\":\"");
        if (pos != std::string::npos) {
            pos += 15;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                description = body.substr(pos, endPos - pos);
            }
        }

        pos = body.find("\"quantity\":");
        if (pos != std::string::npos) {
            pos += 11;
            size_t endPos = body.find(",", pos);
            if (endPos != std::string::npos) {
                quantity = std::stoi(body.substr(pos, endPos - pos));
            }
        }

        pos = body.find("\"price\":");
        if (pos != std::string::npos) {
            pos += 8;
            size_t endPos = body.find(",", pos);
            if (endPos != std::string::npos) {
                price = std::stod(body.substr(pos, endPos - pos));
            }
        }

        pos = body.find("\"category\":\"");
        if (pos != std::string::npos) {
            pos += 12;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                category = body.substr(pos, endPos - pos);
            }
        }

        inventory.addItem(name, description, quantity, price, category);
        return createJsonResponse("{\"message\":\"Item added successfully\"}");
    }

    std::string handleUpdateItem(const std::string& path, const std::string& request) {
        // Extract item ID from path
        std::string id = path.substr(11); // Remove "/api/items/"
        
        // Parse request body similar to add item
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart == std::string::npos) {
            return createErrorResponse(400, "Bad Request");
        }

        std::string body = request.substr(bodyStart + 4);
        std::string name, description, category;
        int quantity = 0;
        double price = 0.0;

        // Parse JSON fields (same as add item)
        size_t pos = body.find("\"name\":\"");
        if (pos != std::string::npos) {
            pos += 8;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                name = body.substr(pos, endPos - pos);
            }
        }

        pos = body.find("\"description\":\"");
        if (pos != std::string::npos) {
            pos += 15;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                description = body.substr(pos, endPos - pos);
            }
        }

        pos = body.find("\"quantity\":");
        if (pos != std::string::npos) {
            pos += 11;
            size_t endPos = body.find(",", pos);
            if (endPos != std::string::npos) {
                quantity = std::stoi(body.substr(pos, endPos - pos));
            }
        }

        pos = body.find("\"price\":");
        if (pos != std::string::npos) {
            pos += 8;
            size_t endPos = body.find(",", pos);
            if (endPos != std::string::npos) {
                price = std::stod(body.substr(pos, endPos - pos));
            }
        }

        pos = body.find("\"category\":\"");
        if (pos != std::string::npos) {
            pos += 12;
            size_t endPos = body.find("\"", pos);
            if (endPos != std::string::npos) {
                category = body.substr(pos, endPos - pos);
            }
        }

        inventory.updateItem(id, name, description, quantity, price, category);
        return createJsonResponse("{\"message\":\"Item updated successfully\"}");
    }

    std::string handleDeleteItem(const std::string& path) {
        std::string id = path.substr(11); // Remove "/api/items/"
        inventory.deleteItem(id);
        return createJsonResponse("{\"message\":\"Item deleted successfully\"}");
    }
};

int main() {
    std::string address;
    std::cout << "Enter server address (default: 127.0.0.1): ";
    std::getline(std::cin, address);
    if (address.empty()) {
        address = "127.0.0.1";
    }

    int port;
    std::cout << "Enter server port (default: 8080): ";
    std::string portStr;
    std::getline(std::cin, portStr);
    if (portStr.empty()) {
        port = 8080;
    } else {
        port = std::stoi(portStr);
    }

    std::string dataPath;
    std::cout << "Enter data file path (default: inventory.json): ";
    std::getline(std::cin, dataPath);
    if (dataPath.empty()) {
        dataPath = "inventory.json";
    }

    HttpServer server(address, port, dataPath);
    server.run();

    return 0;
} 