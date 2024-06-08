#ifndef RESPONSE_H
#define RESPONSE_H
// #include <server.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

/**
 * @brief The Response class represents an HTTP response.
 * 
 * This class encapsulates the components of an HTTP response, such as the HTTP version,
 * status code, content type, and content. It provides methods to set and retrieve these
 * components, as well as methods to generate an HTTP response string.
 */
class Response{
private:
    std::string httpResponse;   /**< The generated HTTP response string. */
    std::string httpVersion;    /**< The HTTP version (e.g., "HTTP/1.1"). */
    std::string httpStatusCode; /**< The HTTP status code (e.g., "200"). */
    std::string httpStatus; /**< The HTTP status message (e.g., "OK"). */
    std::string httpContentType; /**< The content type of the HTTP response. */
    std::string httpContent; /**< The content of the HTTP response. */

    std::unordered_map<int, std::string> httpStatusCodes; /**< Map of HTTP status codes to status messages. */
    void initHttpStatusCodeMap();
    std::string getStatusMessage(int statusCode);

    void createHttpResponse();

    std::string getHttpResponse();

    void readHTMLFile(const std::string &relativeFilePath);

    std::string readFile(const std::string& filePath);
    std::string getMimeType(const std::string& filePath);

    bool operator==(const Response& other) const;
    bool operator!=(const Response& other) const;


public:
    Response();

    void setContent(const std::string& httpContent);
    void setStatusCode(const int& httpStatusCode);
    void setContentType(const std::string& httpContentType);
    void render_template(const std::string& relativeFilePath);
    void serveFile(const std::string& filePath, const std::string& directory);
    void redirect(std::string redirectURL, int statusCode=302);

    friend class Middleware;
    friend class AVLTree;
    friend class WebServer;
};

#endif