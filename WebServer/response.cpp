#include "response.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

/**
 * @brief Default constructor for the Response class.
 * 
 * Initializes the Response object with default values for HTTP-related fields:
 *   - HTTP version: "HTTP/1.1"
 *   - HTTP status code: "200"
 *   - HTTP status message: "OK"
 *   - HTTP content type: "text/plain"
 *   - HTTP content: ""
 * After initialization, it initializes the map of HTTP status codes to status messages
 * and creates the HTTP response string.
 */
Response::Response() 
    : httpVersion("HTTP/1.1"),
      httpStatusCode("200"),
      httpStatus("OK"),
      httpContentType("text/plain"),
      httpContent("") 
{
    initHttpStatusCodeMap();
    createHttpResponse();
}

/**
 * @brief Initializes the map of HTTP status codes to status messages.
 * 
 * Populates the httpStatusCodes unordered map with key-value pairs,
 * where the keys are HTTP status codes and the values are their corresponding status messages.
 * This map is used to retrieve status messages based on HTTP status codes when creating HTTP responses.
 */
void Response::initHttpStatusCodeMap(){
    httpStatusCodes = {
    {100, "Continue"},
    {101, "Switching Protocols"},
    {102, "Processing"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {207, "Multi-Status"},
    {208, "Already Reported"},
    {226, "IM Used"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {307, "Temporary Redirect"},
    {308, "Permanent Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Payload Too Large"},
    {414, "URI Too Long"},
    {415, "Unsupported Media Type"},
    {416, "Range Not Satisfiable"},
    {417, "Expectation Failed"},
    {421, "Misdirected Request"},
    {422, "Unprocessable Entity"},
    {423, "Locked"},
    {424, "Failed Dependency"},
    {426, "Upgrade Required"},
    {428, "Precondition Required"},
    {429, "Too Many Requests"},
    {431, "Request Header Fields Too Large"},
    {451, "Unavailable For Legal Reasons"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"},
    {505, "HTTP Version Not Supported"},
    {506, "Variant Also Negotiates"},
    {507, "Insufficient Storage"},
    {508, "Loop Detected"},
    {510, "Not Extended"},
    {511, "Network Authentication Required"}
};
}

/**
 * @brief Retrieves the status message for a given HTTP status code.
 * 
 * @param statusCode The HTTP status code for which to retrieve the status message.
 * @return The status message corresponding to the provided HTTP status code.
 * If the status code is not found in the map, returns "Unknown Status Code".
 */
std::string Response::getStatusMessage(int statusCode) {
    std::unordered_map<int, std::string>::iterator it = httpStatusCodes.find(statusCode);
    if (it != httpStatusCodes.end()) {
        return it->second;
    } else {
        return "Unknown Status Code";
    }
}

/**
 * @brief Constructs the complete HTTP response by combining HTTP version, status code,
 * status message, content type, content length, and content.
 */
void Response::createHttpResponse(){
    httpResponse = httpVersion + " " + httpStatusCode + " " + httpStatus + "\r\n" +
                   "Content-Type: " + httpContentType + "\r\n" +
                   "Content-Length: " + std::to_string(httpContent.size()) + "\r\n" +
                   "\r\n" + httpContent;
}

/**
 * @brief Retrieves the constructed HTTP response.
 * @return The constructed HTTP response as a string.
 */
std::string Response::getHttpResponse(){
    return httpResponse;
}

/**
 * @brief Sets the content of the HTTP response and updates the response accordingly.
 * @param httpContent The content to be set for the HTTP response.
 */
void Response::setContent(const std::string &httpContent){
    this->httpContent = httpContent;
    createHttpResponse();
}

/**
 * @brief Sets the HTTP status code and status message, then updates the response.
 * @param httpStatusCode The status code to be set for the HTTP response.
 */
void Response::setStatusCode(const int &httpStatusCode){
    this->httpStatusCode = std::to_string(httpStatusCode);
    this->httpStatus = getStatusMessage(httpStatusCode);
    createHttpResponse();
}

/**
 * @brief Sets the content type of the HTTP response and updates the response accordingly.
 * @param httpContentType The content type to be set for the HTTP response.
 */
void Response::setContentType(const std::string &httpContentType){
    this->httpContentType = httpContentType;
    createHttpResponse();
}

/**
 * @brief Reads the content from an HTML file, sets it as the HTTP response content, and updates the response.
 * @param relativeFilePath The relative path of the HTML file to be read and served as the HTTP response content.
 */
void Response::render_template(const std::string &relativeFilePath){
    readHTMLFile(relativeFilePath);
    createHttpResponse();
}

/**
 * @brief Reads the content from an HTML file, sets it as the HTTP response content, and updates the response.
 * @param relativeFilePath The relative path of the HTML file to be read and served as the HTTP response content.
 */
void Response::readHTMLFile(const std::string &relativeFilePath){
    // Construct the full file path by appending the relative file path to the templates directory
    std::string fullFilePath = "./templates/"+relativeFilePath;
    
    // Attempt to open the file
    std::ifstream fileStream(fullFilePath);

     // If the file cannot be opened, set an error response and return
    if (!fileStream) {
        std::cerr << "File not found: " << fullFilePath << std::endl;
        httpContent = "File not found";
        httpStatusCode = "404";
        httpStatus = "Not Found";
        httpContentType = "text/plain";
        createHttpResponse();
        return;
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    httpContent = buffer.str();
    httpContentType = "text/html";
    createHttpResponse();
}

/**
 * @brief Reads the contents of a file and returns it as a string.
 * 
 * @param filePath The path to the file to be read.
 * @return std::string The contents of the file as a string.
 * @throw std::runtime_error if the file cannot be opened.
 */
std::string Response::readFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

/**
 * @brief Reads the contents of a binary file and returns it as a vector of characters.
 * 
 * @param filePath The path to the binary file to be read.
 * @return std::vector<char> The contents of the binary file as a vector of characters.
 * @throw std::runtime_error if the file cannot be opened.
 */
std::vector<char> readFileBinary(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    return std::vector<char>(std::istreambuf_iterator<char>(file), {});
}

/**
 * @brief Determines the MIME type of a file based on its extension.
 * 
 * @param filePath The path to the file.
 * @return std::string The MIME type of the file.
 * 
 * This function maps file extensions to their corresponding MIME types using a static unordered_map.
 * If the file extension is recognized, it returns the corresponding MIME type.
 * If the file extension is not recognized, it returns a default MIME type of "application/octet-stream".
 */
std::string Response::getMimeType(const std::string &filePath){
    static std::unordered_map<std::string, std::string> mimeTypes = {
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".html", "text/html"},
        {".pdf", "application/pdf"}
    };
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = filePath.substr(dotPos);
        auto it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream";
}

/**
 * @brief Serves a file to be included in an HTTP response.
 * 
 * @param filePath The path to the file to be served.
 * @param directory The directory where the file is located.
 * 
 * This method attempts to read the specified file from the given directory,
 * extract its content, determine its MIME type, and create an HTTP response
 * with the file's content and appropriate headers.
 * 
 * If the file is successfully read, it sets the HTTP status code to "200 OK"
 * and the MIME type based on the file's extension. If the file is not found
 * or cannot be read, it sets the HTTP status code to "404 Not Found" and
 * returns a plain text response indicating that the file was not found.
 */
void Response::serveFile(const std::string &filePath, const std::string &directory){
    try {
        std::string fullPath = "."+ directory + filePath;
        std::vector<char> fileContent = readFileBinary(fullPath);
        std::string mimeType = getMimeType(fullPath);

        httpContentType = mimeType;
        httpContent.assign(fileContent.begin(), fileContent.end());
        httpStatusCode = "200";
        httpStatus = "OK";
        createHttpResponse();
    } catch (const std::exception& e) {
        httpStatusCode = "404";
        httpStatus = "Not Found";
        httpContentType = "text/plain";
        httpContent = "File not found";
        createHttpResponse();
    }
}

bool Response::operator==(const Response &other) const{
    if(this->httpContent == other.httpContent) return true;
    return false;
}

bool Response::operator!=(const Response &other) const{
    if(this->httpContent != other.httpContent) return true;
    return false;
}

/**
 * @brief Sets the response to be an HTTP redirect.
 * 
 * @param redirectURL The URL to which the client should be redirected. It be relative or absolute
 * @param statusCode The HTTP status code for the redirect. Default is 302 (Found).
 */
void Response::redirect(std::string redirectURL, int statusCode) {
    httpStatusCode = std::to_string(statusCode);
    httpStatus = getStatusMessage(statusCode);
    httpResponse = httpVersion + " " + httpStatusCode + " " + httpStatus + "\r\n" +
                   "Location: " + redirectURL + "\r\n" +
                   "Content-Length: 0\r\n" + 
                   "\r\n";
}