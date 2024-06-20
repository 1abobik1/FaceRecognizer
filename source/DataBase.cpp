#include "../header/DataBase.h"
#include <stdexcept>

DBConnection::DBConnection() {
    connection_ = PQconnectdb(CONNECTION_DB.data());
    if (PQstatus(connection_) != CONNECTION_OK) {
        throw std::runtime_error("Failed to connect to database");
    }
}

DBConnection::~DBConnection() {
    PQfinish(connection_);
}

Database* Database::instance_ = nullptr;

std::unique_ptr<Database> Database::createInstance() {
    return std::unique_ptr<Database>(new Database());
}

Database::Database() {
    users_table_ = new UserTable();
}

Database::~Database() {
    delete users_table_;
}

Database* Database::getDatabase() {
    if (!instance_) {
        instance_ = createInstance().release();
    }
    return instance_;
}

UserTable* Database::getUserTable() const {
    return users_table_;
}

int UserTable::getUserID(const std::string& name, const std::string& surname) {
    std::string query = "SELECT id FROM users WHERE name = '" + name + "' AND surname = '" + surname + "';";
    PGresult* res = PQexec(connection_, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to get user ID from database");
    }

    int id = -1;
    if (PQntuples(res) > 0) {
        id = std::stoi(PQgetvalue(res, 0, 0));
    }
    PQclear(res);
    return id;
}

void UserTable::addUserToDB(const std::string& name, const std::string& surname) {
    std::string query = "INSERT INTO users (name, surname) VALUES ('" + name + "', '" + surname + "');";
    PGresult* res = PQexec(connection_, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        throw std::runtime_error("Failed to add user to database");
    }
    PQclear(res);
}
