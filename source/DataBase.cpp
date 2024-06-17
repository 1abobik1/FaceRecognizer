#include "../header/DataBase.h"

#include <mutex>
#include <stdexcept>
#include <iostream>
#include <opencv2/imgcodecs.hpp>

std::mutex mtx;

DBConnection::DBConnection() : connection_(PQconnectdb(CONNECTION_DB.data()))
{
	if (PQstatus(connection_) != CONNECTION_OK) {
		PQfinish(connection_);
		connection_ = nullptr;
		throw std::runtime_error(PQerrorMessage(connection_));
	}
}

DBConnection::~DBConnection()
{
	PQfinish(connection_);
}

//---------------initialization instance_---------------//
Database* Database::instance_ = nullptr;


std::unique_ptr<Database> Database::createInstance() {
	return std::unique_ptr<Database>(new Database());
}

Database::Database() : users_table_(new UserTable()) {}

Database::~Database() {
	delete users_table_;
}

Database* Database::getDatabase() {
	if (!instance_) {
		std::lock_guard<std::mutex> lock(mtx);
		if (!instance_) {
			instance_ = createInstance().release();
		}
	}
	return instance_;
}

UserTable* Database::getUserTable() const
{
	return users_table_;
}

std::vector<cv::Mat> UserTable::loadFacesFromDB(std::vector<int>& labels)
{
    std::vector<cv::Mat> faces;

    PGresult* res = PQexec(connection_, "SELECT id, face FROM users");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cout << "Select failed: " << PQerrorMessage(connection_) << '\n';
        PQclear(res);
        return faces;
    }

    int nRows = PQntuples(res);
    for (int i = 0; i < nRows; ++i)
    {
        int id = atoi(PQgetvalue(res, i, 0));
        std::string faceData(PQgetvalue(res, i, 1), PQgetlength(res, i, 1));
        std::vector<uchar> buf(faceData.begin(), faceData.end());
        cv::Mat face = cv::imdecode(buf, cv::IMREAD_GRAYSCALE);
        faces.push_back(face);
        labels.push_back(id);
    }

    PQclear(res);
    return faces;
}

void UserTable::saveFaceToDB(const cv::Mat& face, const std::string& name, const std::string& surname)
{
    // Преобразование изображения в буфер
    std::vector<uchar> buf;
    imencode(".jpg", face, buf);
    std::string faceData(buf.begin(), buf.end());

    std::string sql = "INSERT INTO users (name, surname, face) VALUES ($1, $2, $3::bytea)";
    const char* paramValues[3];
    paramValues[0] = name.c_str();
    paramValues[1] = surname.c_str();
    paramValues[2] = faceData.c_str();

    PGresult* res = PQexecParams(connection_, sql.c_str(), 3, NULL, paramValues, NULL, NULL, 1);
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        std::cout << "Insert failed: " << PQerrorMessage(connection_) << '\n';
    }
    PQclear(res);
}
