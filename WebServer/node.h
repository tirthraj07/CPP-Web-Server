#ifndef NODE_H
#define NODE_H
#include "response.h"
#include "request.h"
#include "middleware.h"
/**
 * @brief Represents a node in the AVL tree used for routing in the web server.
 * 
 * The Node class represents a node in the AVL tree structure used for routing in the web server.
 * Each node stores information about a specific URL route and the corresponding response function.
 * Nodes are organized in the AVL tree based on the route string to enable efficient lookup and routing
 * of HTTP requests to the appropriate response function.
 * 
 * The class provides methods to construct nodes, access and modify their properties, and to manage
 * the AVL tree structure. It is used internally by the WebServer class for routing HTTP requests.
 * 
 * @see WebServer
 */
class Node{
private:
    std::string route;
    Response (*responseFunction)(Request&);
    Middleware middleware;
    Node* left;
    Node* right;
    int height;
    Response response;
    bool containsResponseObject;
    
    Node(Response response);

public:
    Node(const std::string& route, Response (*responseFunction)(Request&));
    Node(const std::string& route, Response (*responseFunction)(Request&), Middleware &middleware);
    
    friend class AVLTree;
    friend class WebServer;
};

#endif