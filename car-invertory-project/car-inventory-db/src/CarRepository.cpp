#include <iostream>
#include <string>
#include <fstream>

#include "CarRepository.h"

CarRepository::CarRepository(sqlite3* db) : db_(db) {}

bool CarRepository::execSql(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite error: " << (errMsg ? errMsg : "unknown") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::string CarRepository::csvEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out.push_back('"');
    return out;
}

void CarRepository::printCarRow(sqlite3_stmt* stmt) {
    const unsigned char* vin = sqlite3_column_text(stmt, 0);
    const unsigned char* make = sqlite3_column_text(stmt, 1);
    const unsigned char* model = sqlite3_column_text(stmt, 2);
    int year = sqlite3_column_int(stmt, 3);
    int miles = sqlite3_column_int(stmt, 4);
    double price = sqlite3_column_double(stmt, 5);
    const unsigned char* status = sqlite3_column_text(stmt, 6);
    const unsigned char* created = sqlite3_column_text(stmt, 7);

    std::cout
        << (vin ? (const char*)vin : "") << " | "
        << (make ? (const char*)make : "") << " | "
        << (model ? (const char*)model : "") << " | "
        << year << " | "
        << miles << " | "
        << price << " | "
        << (status ? (const char*)status : "") << " | "
        << (created ? (const char*)created : "") << "\n";
}

bool CarRepository::ensureSchema() {
    std::string createTableSql = R"sql(
        CREATE TABLE IF NOT EXISTS cars (
          vin TEXT PRIMARY KEY,
          make TEXT NOT NULL,
          model TEXT NOT NULL,
          year INTEGER NOT NULL,
          miles INTEGER NOT NULL,
          price REAL NOT NULL,
          status TEXT NOT NULL,
          created_at TEXT DEFAULT (datetime('now'))
        );
    )sql";
    return execSql(createTableSql);
}

bool CarRepository::addCar() {
    std::string vin, make, model, status;
    int year = 0, miles = 0;
    double price = 0.0;

    std::cout << "VIN: ";
    std::getline(std::cin, vin);
    std::cout << "Make: ";
    std::getline(std::cin, make);
    std::cout << "Model: ";
    std::getline(std::cin, model);

    std::cout << "Year: ";
    if (!(std::cin >> year)) {
        std::cin.clear();
        std::cin.ignore(100000, '\n');
        std::cout << "Invalid year.\n";
        return false;
    }

    std::cout << "Miles: ";
    if (!(std::cin >> miles)) {
        std::cin.clear();
        std::cin.ignore(100000, '\n');
        std::cout << "Invalid miles.\n";
        return false;
    }

    std::cout << "Price: ";
    if (!(std::cin >> price)) {
        std::cin.clear();
        std::cin.ignore(100000, '\n');
        std::cout << "Invalid price.\n";
        return false;
    }

    std::cin.ignore(100000, '\n');
    std::cout << "Status (available/sold): ";
    std::getline(std::cin, status);

    const char* sql =
        "INSERT INTO cars(vin, make, model, year, miles, price, status) "
        "VALUES(?,?,?,?,?,?,?);";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, make.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, model.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, year);
    sqlite3_bind_int(stmt, 5, miles);
    sqlite3_bind_double(stmt, 6, price);
    sqlite3_bind_text(stmt, 7, status.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    std::cout << "✅ Car added.\n";
    return true;
}

void CarRepository::listCars() {
    const char* sql =
        "SELECT vin, make, model, year, miles, price, status, created_at "
        "FROM cars ORDER BY created_at DESC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return;
    }

    std::cout << "\nVIN | Make | Model | Year | Miles | Price | Status | Created\n";
    std::cout << "-----------------------------------------------------------------\n";

    int rows = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        printCarRow(stmt);
        rows++;
    }
    sqlite3_finalize(stmt);

    if (rows == 0) std::cout << "No cars yet.\n";
}

void CarRepository::searchCars() {
    std::cout << "\n--- Search ---\n1) Search by VIN\n2) Search by Make/Model\nChoose: ";
    int s;
    if (!(std::cin >> s)) {
        std::cin.clear();
        std::cin.ignore(100000, '\n');
        std::cout << "Invalid input.\n";
        return;
    }
    std::cin.ignore(100000, '\n');

    if (s == 1) {
        std::string vin;
        std::cout << "VIN: ";
        std::getline(std::cin, vin);

        const char* sql =
            "SELECT vin, make, model, year, miles, price, status, created_at "
            "FROM cars WHERE vin = ?;";

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
            return;
        }

        sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

        std::cout << "\nVIN | Make | Model | Year | Miles | Price | Status | Created\n";
        std::cout << "-----------------------------------------------------------------\n";

        int rows = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            printCarRow(stmt);
            rows++;
        }
        sqlite3_finalize(stmt);

        if (rows == 0) std::cout << "No results.\n";
        return;
    }

    if (s == 2) {
        std::string term;
        std::cout << "Search term: ";
        std::getline(std::cin, term);
        std::string pattern = "%" + term + "%";

        const char* sql =
            "SELECT vin, make, model, year, miles, price, status, created_at "
            "FROM cars WHERE make LIKE ? OR model LIKE ? ORDER BY created_at DESC;";

        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
            return;
        }

        sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);

        std::cout << "\nVIN | Make | Model | Year | Miles | Price | Status | Created\n";
        std::cout << "-----------------------------------------------------------------\n";

        int rows = 0;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            printCarRow(stmt);
            rows++;
        }
        sqlite3_finalize(stmt);

        if (rows == 0) std::cout << "No results.\n";
        return;
    }

    std::cout << "Invalid option.\n";
}

bool CarRepository::updateCar() {
    std::cout << "\n--- Update ---\n";
    std::string vin;
    std::cout << "Enter VIN to update: ";
    std::getline(std::cin, vin);

    std::cout << "What do you want to update?\n"
              << "1) Miles\n"
              << "2) Price\n"
              << "3) Status\n"
              << "Choose: ";

    int u = 0;
    if (!(std::cin >> u)) {
        std::cin.clear();
        std::cin.ignore(100000, '\n');
        std::cout << "Invalid input.\n";
        return false;
    }
    std::cin.ignore(100000, '\n');

    const char* sqlMiles = "UPDATE cars SET miles = ? WHERE vin = ?;";
    const char* sqlPrice = "UPDATE cars SET price = ? WHERE vin = ?;";
    const char* sqlStatus = "UPDATE cars SET status = ? WHERE vin = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = SQLITE_ERROR;

    if (u == 1) {
        int miles = 0;
        std::cout << "New miles: ";
        if (!(std::cin >> miles)) {
            std::cin.clear();
            std::cin.ignore(100000, '\n');
            std::cout << "Invalid miles.\n";
            return false;
        }
        std::cin.ignore(100000, '\n');

        rc = sqlite3_prepare_v2(db_, sqlMiles, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) { std::cerr << sqlite3_errmsg(db_) << "\n"; return false; }
        sqlite3_bind_int(stmt, 1, miles);
        sqlite3_bind_text(stmt, 2, vin.c_str(), -1, SQLITE_TRANSIENT);
    } else if (u == 2) {
        double price = 0.0;
        std::cout << "New price: ";
        if (!(std::cin >> price)) {
            std::cin.clear();
            std::cin.ignore(100000, '\n');
            std::cout << "Invalid price.\n";
            return false;
        }
        std::cin.ignore(100000, '\n');

        rc = sqlite3_prepare_v2(db_, sqlPrice, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) { std::cerr << sqlite3_errmsg(db_) << "\n"; return false; }
        sqlite3_bind_double(stmt, 1, price);
        sqlite3_bind_text(stmt, 2, vin.c_str(), -1, SQLITE_TRANSIENT);
    } else if (u == 3) {
        std::string status;
        std::cout << "New status (available/sold): ";
        std::getline(std::cin, status);

        rc = sqlite3_prepare_v2(db_, sqlStatus, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) { std::cerr << sqlite3_errmsg(db_) << "\n"; return false; }
        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, vin.c_str(), -1, SQLITE_TRANSIENT);
    } else {
        std::cout << "Invalid option.\n";
        return false;
    }

    rc = sqlite3_step(stmt);
    int changed = sqlite3_changes(db_);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Update failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    if (changed == 0) {
        std::cout << "No rows updated. (VIN not found?)\n";
        return false;
    }

    std::cout << "✅ Updated successfully.\n";
    return true;
}

bool CarRepository::deleteCar() {
    std::cout << "\n--- Delete Car ---\n";
    std::string vin;
    std::cout << "Enter VIN to delete: ";
    std::getline(std::cin, vin);

    std::cout << "Are you sure you want to delete this car? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore(100000, '\n');

    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Delete cancelled.\n";
        return false;
    }

    const char* sql = "DELETE FROM cars WHERE vin = ?;";
    sqlite3_stmt* stmt = nullptr;

    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, vin.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    int changes = sqlite3_changes(db_);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Delete failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    if (changes == 0) {
        std::cout << "No car deleted (VIN not found).\n";
        return false;
    }

    std::cout << "✅ Car deleted successfully.\n";
    return true;
}

bool CarRepository::exportToCsv() {
    std::cout << "\n--- Export to CSV ---\n";

    std::string filename;
    std::cout << "Filename (default cars_export.csv): ";
    std::getline(std::cin, filename);
    if (filename.empty()) filename = "cars_export.csv";

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing: " << filename << "\n";
        return false;
    }

    file << "vin,make,model,year,miles,price,status,created_at\n";

    const char* sql =
        "SELECT vin, make, model, year, miles, price, status, created_at "
        "FROM cars ORDER BY created_at DESC;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_) << "\n";
        file.close();
        return false;
    }

    int rows = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* vin = sqlite3_column_text(stmt, 0);
        const unsigned char* make = sqlite3_column_text(stmt, 1);
        const unsigned char* model = sqlite3_column_text(stmt, 2);
        int year = sqlite3_column_int(stmt, 3);
        int miles = sqlite3_column_int(stmt, 4);
        double price = sqlite3_column_double(stmt, 5);
        const unsigned char* status = sqlite3_column_text(stmt, 6);
        const unsigned char* created = sqlite3_column_text(stmt, 7);

        std::string vinS = vin ? (const char*)vin : "";
        std::string makeS = make ? (const char*)make : "";
        std::string modelS = model ? (const char*)model : "";
        std::string statusS = status ? (const char*)status : "";
        std::string createdS = created ? (const char*)created : "";

        file << csvEscape(vinS) << ','
             << csvEscape(makeS) << ','
             << csvEscape(modelS) << ','
             << year << ','
             << miles << ','
             << price << ','
             << csvEscape(statusS) << ','
             << csvEscape(createdS) << '\n';

        rows++;
    }

    sqlite3_finalize(stmt);
    file.close();

    std::cout << "✅ Exported " << rows << " car(s) to: " << filename << "\n";
    return true;
}