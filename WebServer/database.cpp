#include "database.h"
#include <iostream>
#include <vector>

SqliteDatabase::SqliteDatabase(std::string databaseName):databaseName(databaseName), fullFilePath(databaseDirectory + databaseName){ 
    int rc = sqlite3_open(fullFilePath.c_str(), &database);
    if(rc != SQLITE_OK){
        std::cerr << "Database File "<<fullFilePath<<" could not open"<< sqlite3_errmsg(database) << std::endl;
        database = nullptr;
        throw std::runtime_error("Couldn't open database file");
    }
}

SqliteDatabase::~SqliteDatabase(){
    if(database){
        sqlite3_close(database);
    }
}

bool SqliteDatabase::executeQuery(const std::string &query){
    char* errMsg = nullptr;
    int rc = sqlite3_exec(database, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}


std::vector<std::vector<std::string>> SqliteDatabase::executeSelectQuery(const std::string& query) {
    sqlite3_stmt* stmt;
    std::vector<std::vector<std::string>> results;

    int rc = sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
        return results;
    }

    int columnCount = sqlite3_column_count(stmt);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int col = 0; col < columnCount; ++col) {
            const unsigned char* columnText = sqlite3_column_text(stmt, col);
            int columnType = sqlite3_column_type(stmt, col);
            switch (columnType) {
                case SQLITE_INTEGER:
                    row.push_back(std::to_string(sqlite3_column_int(stmt, col)));
                    break;
                case SQLITE_FLOAT:
                    row.push_back(std::to_string(sqlite3_column_double(stmt, col)));
                    break;
                case SQLITE_TEXT: {
                    const unsigned char* columnText = sqlite3_column_text(stmt, col);
                    row.push_back(columnText ? reinterpret_cast<const char*>(columnText) : "");
                    break;
                }
                case SQLITE_NULL:
                    row.push_back("NULL");
                    break;
                default:
                    row.push_back("UNKNOWN_TYPE");
                    break;
            }
        }
        
        results.push_back(row);
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(database) << std::endl;
    }

    sqlite3_finalize(stmt);
    return results;
}


bool SqliteDatabase::executeParameterizedQuery(const std::string& query, const std::vector<SqlParam>& params) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(database) << std::endl;
        return false;
    }

    for (int i = 0; i < params.size(); i++) {
        const SqlParam& param = params[i];
        switch (param.type) {
            case SqlParam::INT:
                sqlite3_bind_int(stmt, static_cast<int>(i + 1), param.intValue);
                break;
            case SqlParam::DOUBLE:
                sqlite3_bind_double(stmt, static_cast<int>(i + 1), param.doubleValue);
                break;
            case SqlParam::TEXT:
                sqlite3_bind_text(stmt, static_cast<int>(i + 1), param.textValue.c_str(), -1, SQLITE_TRANSIENT);
                break;
        }
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(database) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}