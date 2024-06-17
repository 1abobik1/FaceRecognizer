#pragma once

#include <libpq-fe.h>

#include <string_view>
#include <string>
#include <libpq-fe.h>
#include <memory>
#include <set>
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>

constexpr std::string_view CONNECTION_DB = "host=localhost dbname=face_DB user=postgres password=Zopa_kek12";

// RAII for PGconn*
class DBConnection {
private:
    PGconn* connection_;

public:

    DBConnection();

    ~DBConnection();

    operator PGconn* () const
    {
        return connection_;
    }

};

class Database {
private:
    static Database* instance_;
    static std::unique_ptr<Database> createInstance();

    UserTable* users_table_;

    Database();
public:
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    ~Database();

    static Database* getDatabase();

    UserTable* getUserTable() const;
};

class UserTable {
private:
    DBConnection connection_;
    UserTable() = default;

    friend Database;
public:
    std::vector<cv::Mat> loadFacesFromDB(std::vector<int>& labels);

    void saveFaceToDB(const cv::Mat& face, const std::string& name, const std::string& surname);
};