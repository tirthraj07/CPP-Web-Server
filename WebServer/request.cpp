#include "request.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "nlohmann/json.hpp"

/**
 * @brief Private constructor to parse a raw HTTP request.
 * 
 * This constructor is private to ensure that only the WebServer class can create an instance
 * of the Request class. It initializes the Request object by parsing the provided raw request string.
 *
 * @param rawRequest The raw HTTP request string.
 */
Request::Request(std::string& rawRequest){
    parseRequest(rawRequest);
}

/**
 * @brief Parses the raw HTTP request string.
 *
 * This method extracts the request type, route, query parameters, and body from the raw request string.
 *
 * @param rawRequest The raw HTTP request string.
 */
void Request::parseRequest(std::string &rawRequest){
    std::istringstream requestStream(rawRequest);
    std::string line;
    std::getline(requestStream, line);

    std::istringstream lineStream(line);
    lineStream >> requestType >> requestRoute;

    std::string::size_type questionMarkPos = requestRoute.find('?');
    if (questionMarkPos != std::string::npos) {
        std::string queryString = requestRoute.substr(questionMarkPos + 1);
        requestRoute = requestRoute.substr(0, questionMarkPos);
        parseQueryParameters(queryString);
    }
    while (std::getline(requestStream, line) && !line.empty() && line != "\r") {
        if (line.find("Content-Type:") != std::string::npos) {
            contentType = line.substr(line.find(":") + 2);
            if(contentType[contentType.length()-1]=='\r'){
                contentType = contentType.substr(0,contentType.length()-1);
            }
        }
    }
    if (requestType == "POST" || requestType == "PUT" || requestType == "PATCH" || requestType == "DELETE") {
        std::stringstream bodyStream;
        bodyStream << requestStream.rdbuf(); 
        std::string body = bodyStream.str();
        parseRequestBody(body);
    }

}

/**
 * @brief Parses the query parameters from the URL.
 *
 * This method extracts key-value pairs from the query string and stores them in the requestQueryParams map.
 *
 * @param queryString The query string part of the URL.
 */
void Request::parseQueryParameters(const std::string &queryString){
    std::istringstream queryStream(queryString);
    std::string pair;

    while (std::getline(queryStream, pair, '&')) {
        std::string::size_type equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            requestQueryParams[key] = value;
        }
    }
}

/**
 * @brief Parses the request body.
 *
 * This method parses the request body based on the content type and stores the extracted data in the requestBody map.
 *
 * @param body The request body string.
 */
void Request::parseRequestBody(const std::string& body) {
    if(body.empty()) return;

    if(contentType == "application/json"){
        try {
            nlohmann::json jsonBody = nlohmann::json::parse(body);
            for (auto it = jsonBody.begin(); it != jsonBody.end(); ++it) {
                const auto& key = it.key();
                const auto& value = it.value();
                requestBody[key] = value;
            }
        } catch (const nlohmann::json::parse_error& e) {
            std::cerr << "There is a problem with the JSON"<<std::endl;
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            std::cerr << "Received JSON Body: "<<body<<std::endl;
            return;
        }
    }
    else{
        std::istringstream bodyStream(body);
        std::string pair;

        while (std::getline(bodyStream, pair, '&')) {
            std::string::size_type equalPos = pair.find('=');
            if (equalPos != std::string::npos) {
                std::string key = pair.substr(0, equalPos);
                std::string value = pair.substr(equalPos + 1);
                requestBody[key] = value;
            }
        }
    }
}

