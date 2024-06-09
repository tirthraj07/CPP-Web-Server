#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "WebServer/server.h"
#include "WebServer/database.h"

SqliteDatabase database("database.db");

// Function that handles '/' route
Response HomePage(Request& req){
    Response res;
    res.render_template("index.html");  // Path to index.html relative to the 'templates' folder
    return res;
}


// Function that handles '/about' route
Response AboutPage(Request& req){
    Response res;
    res.render_template("about.html"); // Path to about.html relative to the 'templates' folder
    return res;
}

// Function that redirects to 'https://www.google.com'
Response redirectToGoogle(Request &req){
    Response res;
    res.redirect("https://www.google.com");
    return res;
}

// Function that serves an image
Response serveImage(Request &req){
    Response res;
    res.serveFile("cppImage.png", "/public/");
    return res;
}


/*
Function that handles '/api/social-media' route

Try the following:
http://127.0.0.1/api/social-media  (Should generate all the links)
http://127.0.0.1/api/social-media?search=linkedin (Should generate the link)
http://127.0.0.1/api/social-media?search=twitter (Should get not found error)

*/
Response GETRequestAPI(Request& req){
    std::unordered_map<std::string,std::string> queryParams =  req.getRequestQuery();

    Response res;
    res.setContentType("application/json");
    std::string jsonContent;

    auto it = queryParams.find("search");

    if(it == queryParams.end()){    
        jsonContent = R"({"linkedin":"https://www.linkedin.com/in/tirthraj-mahajan/", "github":"https://github.com/tirthraj07", "instagram":"https://www.instagram.com/tirthraj07/"})";
        res.setContent(jsonContent);
        res.setStatusCode(200);
    }
    else{
        if(it->second == "linkedin"){
            jsonContent = R"({"linkedin":"https://www.linkedin.com/in/tirthraj-mahajan/"})";
            res.setContent(jsonContent);
            res.setStatusCode(200);
        }
        else if(it->second == "github"){
            jsonContent = R"({"github":"https://github.com/tirthraj07"})";
            res.setContent(jsonContent);
            res.setStatusCode(200);
        }
        else if(it->second == "instagram"){
            jsonContent = R"({"instagram":"https://www.instagram.com/tirthraj07/"})";
            res.setContent(jsonContent);
            res.setStatusCode(200);
        }
        else{
            jsonContent = R"({"error":"Not Found"})";
            res.setContent(jsonContent);
            res.setStatusCode(404);
        }
    }

    return res;
}

// Function that handles the '/api/form' route
Response POSTRequestAPI(Request& req){
    std::unordered_map<std::string, std::string> requestBody = req.getRequestBody();
    std::string name = requestBody["name"];
    std::string email = requestBody["email"];
    
    Response res;
    res.setContentType("application/json");

    if(name == "" || email == ""){
        res.setContent(R"({"status": "error: incomplete credentials"})");
        res.setStatusCode(400);
        return res;
    }

    std::cout<<"Entered name: "<<name<<std::endl;
    std::cout<<"Entered email: "<<email<<std::endl;

    std::vector<SqliteDatabase::SqlParam> params;
    params.emplace_back(name);
    params.emplace_back(email);

    bool success = database.executeParameterizedQuery("INSERT INTO users (NAME, EMAIL) VALUES (?, ?)", params);
    if(success){
        res.setContent(R"({"status":"success"})");
        res.setStatusCode(201);
    }
    else {
        std::string jsonErrorMessage = R"({"status":")" + database.databaseError() + R"("})";
        res.setContent(jsonErrorMessage);
        res.setStatusCode(400);
    }
    return res;
}

// First Middleware for '/treasure' route
Response middlewareFunctionForTreasurePage(Request& req){
    std::unordered_map<std::string, std::string> queryParams = req.getRequestQuery();
    std::string treasureKey = queryParams["key"];
    if(treasureKey == "123"){
        return Middleware::next();
    }

    Response res;
    res.setContentType("application/json");
    res.setContent(R"({"error":"invalid key", "hint":"key=123"})");
    return res;
}

// Second Middleware for '/treasure' route
Response anotherMiddlewareFunctionForTreasurePage(Request& req){
    std::cout<<"Someone is accessing treasure 0_0"<<std::endl;
    return Middleware::next();
}

// Route to handle the '/treasure' route
Response loadTreasurePage(Request& req){
    Response res;
    res.render_template("treasure.html");
    return res;
}


bool InitDatabase(){
    std::string sql = "CREATE TABLE IF NOT EXISTS users (" \
                       "NAME TEXT NOT NULL," \
                       "EMAIL TEXT NOT NULL PRIMARY KEY" \
                       ");";

    bool success = database.executeQuery(sql);
    if(success == false){
        std::cerr << "Database Initialization Failed" << std::endl;
        return false;
    }

    std::cerr << "Database Initialization Success" << std::endl;
    return true;

}

int main(){

    // Declaring the PORT and IP Address
    const char* PORT = "5000";
    const char* IPAddr = "127.0.0.1";
    
    // Instantiate the Server
    WebServer server = WebServer(PORT,IPAddr);

    // Link routes to the server

    // GET Routes
    server.get("/", &HomePage);
    server.get("/about", &AboutPage);
    server.get("/api/social-media", &GETRequestAPI);
    server.get("/google",&redirectToGoogle);
    server.get("/cpp", &serveImage);

    // Create a middleware list for the '/treasure' route
    Middleware treasureRouteMiddleware;
    treasureRouteMiddleware.push(middlewareFunctionForTreasurePage);
    treasureRouteMiddleware.push(anotherMiddlewareFunctionForTreasurePage);

    // link the route with function and middleware list
    server.get("/treasure",&loadTreasurePage,treasureRouteMiddleware);

    // POST Routes
    server.post("/api/form", &POSTRequestAPI);


    // Initialize the database and run the server
    if(InitDatabase()){
        // Run the server
        server.run();        
    };
}
