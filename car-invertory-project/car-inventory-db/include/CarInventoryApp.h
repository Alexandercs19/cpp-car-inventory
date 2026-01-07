#pragma once
#include "sqlite3.h"

class CarInventoryApp {
private:
    sqlite3* db_;
public:
    explicit CarInventoryApp(sqlite3* db);
    void run();
};