#pragma once
#include "ICommand.h"
#include "CarRepository.h"

// ADD
class AddCarCommand : public ICommand {
    CarRepository& repo_;
public:
    AddCarCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.addCar(); }
};

// LIST
class ListCarsCommand : public ICommand {
    CarRepository& repo_;
public:
    ListCarsCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.listCars(); }
};

// SEARCH
class SearchCarsCommand : public ICommand {
    CarRepository& repo_;
public:
    SearchCarsCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.searchCars(); }
};

// UPDATE
class UpdateCarCommand : public ICommand {
    CarRepository& repo_;
public:
    UpdateCarCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.updateCar(); }
};

// DELETE
class DeleteCarCommand : public ICommand {
    CarRepository& repo_;
public:
    DeleteCarCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.deleteCar(); }
};

// EXPORT
class ExportCsvCommand : public ICommand {
    CarRepository& repo_;
public:
    ExportCsvCommand(CarRepository& r) : repo_(r) {}
    void execute() override { repo_.exportToCsv(); }
};

// EXIT
class ExitCommand : public ICommand {
    bool& running_;
public:
    ExitCommand(bool& running) : running_(running) {}
    void execute() override { running_ = false; }
};