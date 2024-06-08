#include "middleware.h"

Response Middleware::nextObj;

void Middleware::push(Response (*middlewareFunction)(Request &)){
    listOfMiddlewareFunctions.push_back(middlewareFunction);
}

Response Middleware::execute(Request &req){
    for(auto it = listOfMiddlewareFunctions.begin(); it != listOfMiddlewareFunctions.end(); it++){
        Response res = (*it)(req);
        if(res != nextObj) return res;
    }
    return nextObj;
}
