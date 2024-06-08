#define _WIN32_WINNT 0x501
#include "server.h"
#include "response.h"
#include "request.h"
#include "middleware.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")


/**
 * @brief Constructs a WebServer object with the specified port and IP address.
 * 
 * This constructor initializes the WebServer object with the given port and IP address.
 * It also initializes the Winsock library, creates a server socket, sets up address information,
 * and binds the socket to the address.
 * 
 * @param PORT The port number for the server.
 * @param IPAddr The IP address for the server.
 * @throw std::runtime_error if initialization fails.
 */
WebServer::WebServer(const char* PORT,const char* IPAddr):PORT(PORT),IPAddr(IPAddr){
    this->wVersionRequested = MAKEWORD(2,2);

    if( initializeWinsock() == 1){
        throw std::runtime_error("Failed to initialize Winsock");
    } 

    if( createUnboundedSocket() == 1 ){
        throw std::runtime_error("Failed to create socket");
    }

    if( setupAddressInfo() == 1 ){
        throw std::runtime_error("Failed to get address info");
    }

    if ( bindSocketToAddress() == 1 ){
        throw std::runtime_error("Failed to bind socket");
    }

};

WebServer::~WebServer(){
    closesocket(serverSocket);
    WSACleanup();
    std::cout<<"--- Application stopped ---"<<std::endl;
}

/**
 * Initialize Winsock library.
 * 
 * This function initializes the Winsock library by calling WSAStartup
 * and performs necessary setup for socket communication. If the initialization
 * fails, an error message is printed to standard error stream.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::initializeWinsock(){
    iResult = WSAStartup(wVersionRequested, &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }
    return 0;
}

/**
 * Create an unbounded socket.
 * 
 * This function creates a socket for communication using the TCP/IP protocol (AF_INET)
 * with stream-oriented communication (SOCK_STREAM) and a default protocol (0).
 * If the socket creation fails, an error message is printed to standard error stream
 * and the Winsock library is cleaned up.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::createUnboundedSocket()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET){
        std::cerr << "Socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    return 0;
}

/**
 * Setup address information for socket binding.
 * 
 * This function initializes a `addrinfo` structure with the desired socket parameters,
 * such as address family (IPv4), socket type (stream-oriented), protocol (TCP), and flags
 * (AI_PASSIVE for binding to any available address). It then retrieves address information
 * for the specified IP address and port using the `getaddrinfo` function. If the operation
 * fails, an error message is printed to standard error stream, the socket is closed, and
 * the Winsock library is cleaned up.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::setupAddressInfo()
{
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(IPAddr, PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    return 0;
}

/**
 * Bind the socket to the address information.
 * 
 * This function binds the server socket to the address information obtained
 * earlier using the `setupAddressInfo` function. If the bind operation fails,
 * an error message is printed to standard error stream, the address information
 * is freed, the socket is closed, and the Winsock library is cleaned up.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::bindSocketToAddress()
{
    iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);   
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    return 0;
}

/**
 * Run the web server.
 * 
 * This function starts the web server by listening for incoming connections, accepting
 * connection requests, and handling client requests indefinitely. It continuously listens
 * for and processes client requests until an error occurs or the server is terminated manually.
 * 
 * @return 0 on success, terminates the program with an error message on failure.
 */
int WebServer::run()
{
    if( listenForConnections() == 1 ){
        throw std::runtime_error("Failed to listen to connections");
    }

    while(true){
        if( acceptConnectionRequest() == 1 ){
            throw std::runtime_error("Failed to accept connection request");
        }
        if( handleClientRequest() == 1 ){
            throw std::runtime_error("Failed to receive data from connection");
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    std::cout<<"--- Ending application -- "<<std::endl;
    return 0;
}

/**
 * Listen for incoming connections.
 * 
 * This function starts listening for incoming connections on the server socket.
 * If the listen operation fails, an error message is printed to standard error stream,
 * the server socket is closed, and the Winsock library is cleaned up.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::listenForConnections()
{
    iResult = listen(serverSocket, SOMAXCONN);

    if(iResult == SOCKET_ERROR){
        std::cerr<<"Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    } 

    std::cout<<"Server listening on http://"<<IPAddr<<":"<<PORT<<std::endl;

    return 0;
}

/**
 * Accept incoming connection requests.
 * 
 * This function accepts incoming connection requests on the server socket.
 * If the accept operation fails, an error message is printed to standard error stream,
 * the server socket is closed, and the Winsock library is cleaned up.
 * 
 * @return 0 on success, 1 on failure
 */
int WebServer::acceptConnectionRequest(){
    clientSocket = accept(serverSocket, NULL, NULL);

    if(clientSocket ==  INVALID_SOCKET){
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    return 0;
}

/**
 * Check if a string starts with a given prefix.
 * 
 * This function checks if the given string `str` starts with the specified prefix.
 * 
 * @param str The string to check.
 * @param prefix The prefix to check for.
 * @return True if `str` starts with `prefix`, otherwise false.
 */
bool WebServer::startsWith(const std::string &str, const std::string &prefix){
    return str.compare(0, prefix.size(), prefix) == 0;
}


/**
 * Get the remaining path after removing a prefix.
 * 
 * This function removes the specified prefix from the given string `str` and returns
 * the remaining path.
 * 
 * @param str The string from which to remove the prefix.
 * @param prefix The prefix to remove.
 * @return The remaining path after removing the prefix, or an empty string if the prefix
 *         is not found in `str`.
 */
std::string WebServer::getRemainingPath(const std::string &str, const std::string &prefix) {
    if (str.compare(0, prefix.size(), prefix) == 0) {
        return str.substr(prefix.size());
    }
    return "";
}

/**
 * Handle a client request.
 * 
 * This function handles a client request by receiving the request data, parsing it to extract
 * the method and route, processing the request based on the method and route, generating an
 * appropriate HTTP response, and sending the response back to the client.
 * 
 * @return 0 on success, 1 on failure.
 */

int WebServer::handleClientRequest(){
    const int recvbuflen = 8192;
    char recvbuf[recvbuflen];

    int iResult = recv(clientSocket, recvbuf, recvbuflen, 0);

    if( iResult == SOCKET_ERROR ){
        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return 1;
    }
    else if ( iResult == 0 ){
        closesocket(clientSocket);
        return 0;
    }

    recvbuf[iResult] = '\0';
    std::string request(recvbuf);
    // std::string method;
    // std::string route;
    // std::istringstream requestStream(request);
    Request requestObject(request);
    std::string route = requestObject.getRequestRoute();
    std::string method = requestObject.getRequestType();
    // requestStream >> method >> route;
    std::string response;
    if(method == "GET"){
        if(startsWith(route, cssDirectory)){
            std::string cssFilePath = getRemainingPath(route, cssDirectory);
            response = serveCSSFile(cssFilePath);
        }
        else if(startsWith(route, jsDirectory)){
            std::string jsFilePath = getRemainingPath(route, jsDirectory);
            response = serveJSFile(jsFilePath);
        }
        else if(startsWith(route, publicDirectory)){
            std::string publicFilePath = getRemainingPath(route, publicDirectory);
            response = servePublicFile(publicFilePath);
        }
        else{
            response = searchGETTree(requestObject);
        }
    }
    else if(method == "POST"){
        response = searchPOSTTree(requestObject);
    }
    else if(method == "PUT"){
        response = searchPUTTree(requestObject);
    }
    else if(method == "PATCH"){
        response = searchPATCHTree(requestObject);
    }
    else if(method == "DELETE"){
        response = searchDELETETree(requestObject);
    }
    else {
        std::string rawResponse = R"({"error": "Method Not Allowed"})";
        response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }

    iResult = send(clientSocket, response.c_str(), (int)response.length(), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    }   

    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Shutdown failed: " << WSAGetLastError() << std::endl;
    }

    closesocket(clientSocket);

    return 0;
}

/**
 * Add a GET route to the server.
 * 
 * This function associates a GET route with a response function and adds it to the server's route tree.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 */
void WebServer::get(std::string route,  Response (*responseFunction)(Request&)){
    GetRouteTree.insert(route, responseFunction);
}

/**
 * Add a GET route to the server with middleware.
 * 
 * This function associates a GET route with a response function and a middleware chain, then adds it to the server's route tree.
 * The middleware chain allows for additional processing of the request before the response function is called.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 * @param middleware A reference to the middleware chain to be executed before the response function.
 */
void WebServer::get(std::string route,  Response (*responseFunction)(Request&), Middleware &middleware){
    GetRouteTree.insert(route, responseFunction, middleware);
}

/**
 * Add a POST route to the server.
 * 
 * This function associates a POST route with a response function and adds it to the server's route tree.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 */
void WebServer::post(std::string route, Response (*responseFunction)(Request &)){
    PostRouteTree.insert(route, responseFunction);
}

/**
 * Add a POST route to the server with middleware.
 * 
 * This function associates a POST route with a response function and a middleware chain, then adds it to the server's route tree.
 * The middleware chain allows for additional processing of the request before the response function is called.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 * @param middleware A reference to the middleware chain to be executed before the response function.
 */
void WebServer::post(std::string route, Response (*responseFunction)(Request &), Middleware &middleware){
    PostRouteTree.insert(route, responseFunction, middleware);
}

/**
 * Add a PUT route to the server.
 * 
 * This function associates a PUT route with a response function and adds it to the server's route tree.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 */
void WebServer::put(std::string route, Response (*responseFunction)(Request &)){
    PutRouteTree.insert(route, responseFunction);
}

/**
 * Add a PUT route to the server with middleware.
 * 
 * This function associates a PUT route with a response function and a middleware chain, then adds it to the server's route tree.
 * The middleware chain allows for additional processing of the request before the response function is called.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 * @param middleware A reference to the middleware chain to be executed before the response function.
 */
void WebServer::put(std::string route, Response (*responseFunction)(Request &), Middleware &middleware){
    PutRouteTree.insert(route, responseFunction, middleware);
}

/**
 * Add a PATCH route to the server.
 * 
 * This function associates a PATCH route with a response function and adds it to the server's route tree.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 */
void WebServer::patch(std::string route, Response (*responseFunction)(Request &)){
    PatchRouteTree.insert(route, responseFunction);
}

/**
 * Add a PATCH route to the server with middleware.
 * 
 * This function associates a PATCH route with a response function and a middleware chain, then adds it to the server's route tree.
 * The middleware chain allows for additional processing of the request before the response function is called.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 * @param middleware A reference to the middleware chain to be executed before the response function.
 */
void WebServer::patch(std::string route, Response (*responseFunction)(Request &), Middleware &middleware){
    PatchRouteTree.insert(route, responseFunction, middleware);
}

/**
 * Add a DELETE route to the server.
 * 
 * This function associates a DELETE route with a response function and adds it to the server's route tree.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 */
void WebServer::del(std::string route, Response (*responseFunction)(Request &)){
    DeleteRouteTree.insert(route, responseFunction);
}


/**
 * Add a DELETE route to the server with middleware.
 * 
 * This function associates a DELETE route with a response function and a middleware chain, then adds it to the server's route tree.
 * The middleware chain allows for additional processing of the request before the response function is called.
 * 
 * @param route The route path.
 * @param responseFunction A pointer to the response function to be called when the route is accessed.
 * @param middleware A reference to the middleware chain to be executed before the response function.
 */
void WebServer::del(std::string route, Response (*responseFunction)(Request &), Middleware &middleware){
    DeleteRouteTree.insert(route, responseFunction, middleware);
}

/**
 * Search for a GET route in the route tree and return the corresponding response.
 * 
 * This function searches for the specified GET route in the route tree and returns the
 * corresponding response. If the route is not found, a 404 Not Found response is returned.
 * 
 * @param route The GET route to search for.
 * @return The HTTP response corresponding to the route, or a 404 Not Found response if the
 *         route is not found.
 */
std::string WebServer::searchGETTree(Request &requestObject) {
    std::string route = requestObject.getRequestRoute();
    Node* searchedRoute = GetRouteTree.search(requestObject);
    std::string response;
    if(searchedRoute == NULL){
        std::cerr<<"GET "<<route<<": Not Found"<<std::endl;
        std::string rawResponse = R"({"error": "Not Found"})";
        return response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }
    if(searchedRoute->containsResponseObject){
        // Interrupted by a Middleware
        return searchedRoute->response.getHttpResponse();
    }
    Response responseObject = (searchedRoute->responseFunction)(requestObject);
    response = responseObject.getHttpResponse();
    std::cout<<"GET "<<route<<std::endl;
    return response;
}

/**
 * Search for a POST route in the route tree and return the corresponding response.
 * 
 * This function searches for the specified POST route in the route tree and returns the
 * corresponding response. If the route is not found, a 404 Not Found response is returned.
 * 
 * @param route The POST route to search for.
 * @return The HTTP response corresponding to the route, or a 404 Not Found response if the
 *         route is not found.
 */
std::string WebServer::searchPOSTTree(Request &requestObject){
    std::string route = requestObject.getRequestRoute();
    Node* searchedRoute = PostRouteTree.search(requestObject);
    std::string response;
    if(searchedRoute == NULL){
        std::cerr<<"POST "<<route<<": Not Found"<<std::endl;
        std::string rawResponse = R"({"error": "Not Found"})";
        return response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }
    if(searchedRoute->containsResponseObject){
        // Interrupted by a Middleware
        return searchedRoute->response.getHttpResponse();
    }
    Response responseObject = (searchedRoute->responseFunction)(requestObject);
    response = responseObject.getHttpResponse();
    std::cout<<"POST "<<route<<std::endl;
    return response;
}

/**
 * Search for a PUT route in the route tree and return the corresponding response.
 * 
 * This function searches for the specified PUT route in the route tree and returns the
 * corresponding response. If the route is not found, a 404 Not Found response is returned.
 * 
 * @param route The PUT route to search for.
 * @return The HTTP response corresponding to the route, or a 404 Not Found response if the
 *         route is not found.
 */
std::string WebServer::searchPUTTree(Request &requestObject){
    std::string route = requestObject.getRequestRoute();
    Node* searchedRoute = PutRouteTree.search(requestObject);
    std::string response;
    if(searchedRoute == NULL){
        std::cerr<<"PUT "<<route<<": Not Found"<<std::endl;
        std::string rawResponse = R"({"error": "Page Not Found"})";
        return response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }
    if(searchedRoute->containsResponseObject){
        // Interrupted by a Middleware
        return searchedRoute->response.getHttpResponse();
    }
    Response responseObject = (searchedRoute->responseFunction)(requestObject);
    response = responseObject.getHttpResponse();
    std::cout<<"PUT "<<route<<std::endl;
    return response;
}

/**
 * Search for a PATCH route in the route tree and return the corresponding response.
 * 
 * This function searches for the specified PATCH route in the route tree and returns the
 * corresponding response. If the route is not found, a 404 Not Found response is returned.
 * 
 * @param route The PATCH route to search for.
 * @return The HTTP response corresponding to the route, or a 404 Not Found response if the
 *         route is not found.
 */
std::string WebServer::searchPATCHTree(Request &requestObject){
    std::string route = requestObject.getRequestRoute();
    Node* searchedRoute = PatchRouteTree.search(requestObject);
    std::string response;
    if(searchedRoute == NULL){
        std::cerr<<"PATCH "<<route<<": Not Found"<<std::endl;
        std::string rawResponse = R"({"error": "Page Not Found"})";
        return response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }
    if(searchedRoute->containsResponseObject){
        // Interrupted by a Middleware
        return searchedRoute->response.getHttpResponse();
    }
    Response responseObject = (searchedRoute->responseFunction)(requestObject);
    response = responseObject.getHttpResponse();
    std::cout<<"PATCH "<<route<<std::endl;
    return response;
}

/**
 * Search for a DELETE route in the route tree and return the corresponding response.
 * 
 * This function searches for the specified DELETE route in the route tree and returns the
 * corresponding response. If the route is not found, a 404 Not Found response is returned.
 * 
 * @param route The DELETE route to search for.
 * @return The HTTP response corresponding to the route, or a 404 Not Found response if the
 *         route is not found.
 */
std::string WebServer::searchDELETETree(Request &requestObject){
    std::string route = requestObject.getRequestRoute();
    Node* searchedRoute = DeleteRouteTree.search(requestObject);
    std::string response;
    if(searchedRoute == NULL){
        std::cerr<<"DELETE "<<route<<": Not Found"<<std::endl;
        std::string rawResponse = R"({"error": "Page Not Found"})";
        return response = "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\nContent-Length: " + std::to_string(rawResponse.size()) + "\r\n\r\n" + rawResponse;
    }
    if(searchedRoute->containsResponseObject){
        // Interrupted by a Middleware
        return searchedRoute->response.getHttpResponse();
    }
    Response responseObject = (searchedRoute->responseFunction)(requestObject);
    response = responseObject.getHttpResponse();
    std::cout<<"DELETE "<<route<<std::endl;
    return response;
}

/**
 * Serve a CSS file to the client.
 * 
 * This function serves the specified CSS file to the client by reading its contents and
 * generating an appropriate HTTP response.
 * 
 * @param cssFilePath The path to the CSS file.
 * @return The HTTP response containing the CSS file content.
 */
std::string WebServer::serveCSSFile(std::string cssFilePath){
    Response responseObject;
    responseObject.serveFile(cssFilePath, cssDirectory);
    return responseObject.getHttpResponse();
}

/**
 * Serve a JavaScript file to the client.
 * 
 * This function serves the specified JavaScript file to the client by reading its contents and
 * generating an appropriate HTTP response.
 * 
 * @param jsFilePath The path to the JavaScript file.
 * @return The HTTP response containing the JavaScript file content.
 */
std::string WebServer::serveJSFile(std::string jsFilePath){
    Response responseObject;
    responseObject.serveFile(jsFilePath, jsDirectory);
    return responseObject.getHttpResponse();
}


/**
 * Serve a public file to the client.
 * 
 * This function serves the specified file from the public directory to the client by reading its
 * contents and generating an appropriate HTTP response.
 * 
 * @param publicFilePath The path to the public file.
 * @return The HTTP response containing the public file content.
 */
std::string WebServer::servePublicFile(std::string publicFilePath){
    Response responseObject;
    responseObject.serveFile(publicFilePath, publicDirectory);
    return responseObject.getHttpResponse();
}