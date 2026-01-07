#pragma once
#include <string>
#include "sqlite3.h"

class CarRepository {
private:
    sqlite3* db_;

    bool execSql(const std::string& sql);
    static void printCarRow(sqlite3_stmt* stmt);
    static std::string csvEscape(const std::string& s);

public:
    explicit CarRepository(sqlite3* db);

    bool ensureSchema();

    bool addCar();
    void listCars();
    void searchCars();
    bool updateCar();
    bool deleteCar();
    bool exportToCsv();
};