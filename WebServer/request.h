#ifndef REQUEST_H
#define REQUEST_H
#include <string>
#include <unordered_map>

/**
 * @class Request
 * @brief Represents an HTTP request and parses its components.
 *
 * The Request class is designed to handle the parsing of raw HTTP request strings.
 * It extracts the request type (e.g., GET, POST, PUT, PATCH, DELETE), the request route, any query parameters,
 * and the request body. It also handles parsing the content type of the request.
 *
 * The constructor is private to ensure that only the WebServer class can create an instance
 * of the Request class, maintaining control over the request handling process.
 *
 * @note The class uses several private member variables to store parsed data from the request.
 *       It provides public getter methods to access these data members.
 */
class Request{
private:
    std::string requestType;    ///< The HTTP request type (e.g., GET, POST, PUT, PATCH, DELETE).
    std::string requestRoute;   ///< The requested route
    std::unordered_map<std::string, std::string> requestBody;   ///< The request body parameters, typically for POST requests.
    std::unordered_map<std::string, std::string> requestQueryParams;      ///< The query parameters from the URL
    std::string contentType;    ///< The content type of the request

    Request(std::string& rawRequest);         // Only WebServer Class can create an instance of the Request class

    void parseRequest(std::string& rawRequest);
    void parseQueryParameters(const std::string& queryString);
    void parseRequestBody(const std::string& body);

public:
    friend class WebServer;

    // Getters
    
    /**
     * @brief Gets the request type.
     *
     * @return The request type (e.g., GET, POST).
     */
    const std::string& getRequestType() const { return requestType; }
    
    /**
     * @brief Gets the request route.
     *
     * @return The request route (e.g., "/home").
     */
    const std::string& getRequestRoute() const { return requestRoute; }

    /**
     * @brief Gets the request body parameters.
     *
     * @return An std::unordered_map of type <std::string, std::string> containing request body parameters.
     */
    const std::unordered_map<std::string, std::string>& getRequestBody() const { return requestBody; }
    
    /**
     * @brief Gets the request query parameters.
     *
     * @return An std::unordered_map of type <std::string, std::string> containing query parameters from the URL.
     */
    const std::unordered_map<std::string, std::string>& getRequestQuery() const { return requestQueryParams; }

};

#endif