#include <iostream>
#include <filesystem>
#include "sqlite3.h"
#include "CarInventoryApp.h"

int main() {
    sqlite3* db = nullptr;
    std::filesystem::create_directories("db");

    if (sqlite3_open("db/cars.db", &db) != SQLITE_OK) {
        std::cerr << "DB error\n";
        return 1;
    }

    CarInventoryApp app(db);
    app.run();

    sqlite3_close(db);
    return 0;
}