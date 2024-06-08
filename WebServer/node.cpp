#include "response.h"
#include "node.h"

/**
 * @brief Constructs a new Node object with the specified route and response function.
 * 
 * @param route The route associated with the node.
 * @param responseFunction A function pointer to the response function associated with the route.
 * 
 * This constructor initializes a new Node object with the given route and response function.
 * The route represents the URL path associated with the node, and the response function is
 * a function pointer to the function that generates the HTTP response for that route.
 * 
 * The left and right child pointers are initialized to nullptr, and the height of the node
 * is initially set to 1, indicating that it is a leaf node in the AVL tree.
 */
Node::Node(const std::string &route, Response (*responseFunction)(Request&)): route(route), responseFunction(responseFunction), left(nullptr), right(nullptr), height(1) {
    middleware = Middleware();
    containsResponseObject = false;
}

Node::Node(const std::string &route, Response (*responseFunction)(Request &), Middleware &middleware): route(route), responseFunction(responseFunction), left(nullptr), right(nullptr) {
    this->middleware = middleware;
    containsResponseObject = false;
}

Node::Node(Response response){
    this->response = response;
    route = "--INVALID--";
    responseFunction = NULL;
    containsResponseObject = true;
}
