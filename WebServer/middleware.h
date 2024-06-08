#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H
#include <list>
#include "response.h"
#include "request.h"

/**
 * @brief The Middleware class manages a list of middleware functions that process HTTP requests.
 *
 * This class is designed to allow chaining multiple middleware functions that each take a Request
 * object as a parameter and return a Response object. The middleware functions are stored in a list,
 * and the `execute` method processes a given Request object through these functions sequentially.
 * If a middleware function returns a Response object different from the predefined `next` object,
 * the execution stops and that Response object is returned. Otherwise, it proceeds to the next
 * middleware function.
 */
class Middleware{
private:
    /**
     * @brief A list of middleware functions.
     * 
     * Each middleware function is a function pointer that takes a Request object as a parameter
     * and returns a Response object.
     */
    std::list<Response (*) (Request &)> listOfMiddlewareFunctions;
    
    /**
     * @brief A static Response object used to indicate continuation to the next middleware function.
     */
    static Response nextObj;

    /**
     * @brief Executes the middleware functions sequentially with the given Request object.
     * 
     * Iterates through the list of middleware functions and processes the given Request object.
     * If a middleware function returns a Response object different from `nextObj`, the execution
     * stops and that Response object is returned. If all middleware functions return `nextObj`,
     * the method returns `nextObj`.
     * 
     * @param req The Request object to be processed by the middleware functions.
     * @return The Response object returned by a middleware function or `nextObj`.
     */
    Response execute(Request &req);

public:
    /**
     * @brief Gets the static next Response object used to indicate continuation to the next middleware function.
     * 
     * @return The next Response object.
     */
    static Response next() {return nextObj;};

    /**
     * @brief Adds a middleware function to the list.
     * 
     * @param middlewareFunction A function pointer to a middleware function.
     */
    void push(Response (*middlewareFunction)(Request&));



    friend class AVLTree;
    friend class WebServer;

};

#endif