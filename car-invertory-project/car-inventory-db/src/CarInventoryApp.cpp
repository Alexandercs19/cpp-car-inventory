#include <iostream>
#include <map>

#include "CarInventoryApp.h"
#include "CarRepository.h"
#include "Command.h"

CarInventoryApp::CarInventoryApp(sqlite3* db) : db_(db) {}

void CarInventoryApp::run() {
    CarRepository repo(db_);

    if (!repo.ensureSchema()) {
        std::cout << "DB init failed\n";
        return;
    }

    bool running = true;

    AddCarCommand add(repo);
    ListCarsCommand list(repo);
    SearchCarsCommand search(repo);
    UpdateCarCommand update(repo);
    DeleteCarCommand del(repo);
    ExportCsvCommand exportCsv(repo);
    ExitCommand exitCmd(running);

    std::map<int, ICommand*> commands = {
        {1, &add},
        {2, &list},
        {3, &search},
        {4, &update},
        {5, &del},
        {6, &exportCsv},
        {7, &exitCmd}
    };

    while (running) {
        std::cout
            << "\n1) Add\n2) List\n3) Search\n4) Update\n5) Delete\n6) Export\n7) Exit\nChoose: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (commands.count(choice)) {
            commands[choice]->execute(); 
        } else {
            std::cout << "Invalid option\n";
        }
    }
}