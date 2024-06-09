#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite/sqlite3.h"
#include <string>
#include <vector>

class SqliteDatabase{
private:
    const std::string databaseDirectory = "./database/";
    const std::string databaseName;
    const std::string fullFilePath;
    sqlite3* database;
public:
    SqliteDatabase(std::string databaseName);
    ~SqliteDatabase();
    struct SqlParam {
        enum Type { INT, DOUBLE, TEXT } type;
        union {
            int intValue;
            double doubleValue;
        };
        std::string textValue;

        SqlParam(int value) : type(INT), intValue(value) {}
        SqlParam(double value) : type(DOUBLE), doubleValue(value) {}
        SqlParam(std::string value) : type(TEXT), textValue(value) {}    
    };
    bool executeQuery(const std::string& query); 
    std::vector<std::vector<std::string>> executeSelectQuery(const std::string& query);
    bool executeParameterizedQuery(const std::string& query, const std::vector<SqlParam>& params);
    std::string databaseError(){
        return std::string(sqlite3_errmsg(database));
    }

};

#endif