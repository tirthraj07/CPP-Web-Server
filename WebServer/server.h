#ifndef SERVER_H
#define SERVER_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "avltree.h"
#include "response.h"
#include "middleware.h"

/**
 * @brief A simple HTTP web server implemented in C++ using Winsock2 by Tirthraj Mahajan.
 * 
 * The WebServer class provides functionality to create, configure, and run
 * a basic HTTP web server. It listens for incoming connections, accepts client
 * requests, and serves static files and dynamic content based on the requested routes.
 * 
 * The server supports GET requests for serving static files such as CSS, JavaScript, and
 * other resources stored in predefined directories. Additionally, it allows users to register
 * custom response functions for specific routes, enabling dynamic content generation.
 * 
 * @note This class requires Windows operating system and Winsock2 library for networking.
 */
class WebServer{
private:
    WORD wVersionRequested; ///< Winsock version requested
    WSADATA wsaData;        ///< Winsock data structure
    int iResult;            ///< Winsock operation result

    SOCKET serverSocket;    ///< Server socket for listening to incoming connections

    struct addrinfo* result = NULL; ///< Address information
    struct addrinfo hints;          ///< Address hints for socket configuration

    const char* IPAddr;         ///< IP address for the server
    const char* PORT;           ///< Port number for the server

    SOCKET clientSocket;    ///< Socket for communicating with client

    AVLTree GetRouteTree;   ///< AVL tree to store GET routes and associated response functions
    AVLTree PostRouteTree;   ///< AVL tree to store POST routes and associated response functions
    AVLTree PutRouteTree;   ///< AVL tree to store PUT routes and associated response functions
    AVLTree PatchRouteTree;    ///< AVL tree to store PATCH routes and associated response functions
    AVLTree DeleteRouteTree;   ///< AVL tree to store DELETE routes and associated response functions

    std::string cssDirectory = "/static/css/";      ///< Directory for serving CSS files
    std::string jsDirectory = "/static/js/";        ///< Directory for serving JavaScript files
    std::string publicDirectory = "/public/";       ///< Directory for serving other public files

    int initializeWinsock(); 
    int createUnboundedSocket();
    int setupAddressInfo();
    int bindSocketToAddress();
    int listenForConnections();
    int acceptConnectionRequest();
    int handleClientRequest();
    std::string searchGETTree(Request& requestObject);
    std::string searchPOSTTree(Request& requestObject);
    std::string searchPUTTree(Request& requestObject);
    std::string searchPATCHTree(Request& requestObject);
    std::string searchDELETETree(Request& requestObject);

    bool startsWith(const std::string& str, const std::string& prefix);
    std::string getRemainingPath(const std::string& str, const std::string& prefix);
    std::string serveCSSFile(std::string cssFilePath);
    std::string serveJSFile(std::string jsFilePath);
    std::string servePublicFile(std::string publicFilePath);

public:
    WebServer(const char* PORT,const char* IPAddr);
    ~WebServer();

    int run();
    void get(std::string route, Response (*responseFunction)(Request&));
    void get(std::string route, Response (*responseFunction)(Request &), Middleware &middleware);
    void post(std::string route, Response (*responseFunction)(Request&));
    void post(std::string route, Response (*responseFunction)(Request &), Middleware &middleware);
    void put(std::string route, Response (*responseFunction)(Request&));
    void put(std::string route, Response (*responseFunction)(Request &), Middleware &middleware);
    void patch(std::string route, Response (*responseFunction)(Request&));
    void patch(std::string route, Response (*responseFunction)(Request &), Middleware &middleware);
    void del(std::string route, Response (*responseFunction)(Request&));
    void del(std::string route, Response (*responseFunction)(Request &), Middleware &middleware);

};

#endif