# 🚗 Car Inventory Database (C++ & SQLite)

A console-based **Car Inventory Management System** built with **C++**, **SQLite**, and **CMake**.  
This project demonstrates clean **Object-Oriented Programming**, **interfaces**, **polymorphism**, and the **Command Pattern**, while managing real persistent data.

It is designed to be easy to understand for beginners, yet structured like a real-world software engineering project.

---

## 📌 Features

### Core Inventory
- Add new cars to the inventory
- List all cars
- Search cars by **VIN** or **Make/Model**
- Update car data (miles, price, status)
- Delete cars
- Export inventory to **CSV**
- Persistent storage using **SQLite**

### Budget Management
- Track car prices as part of inventory budgeting
- Adjust pricing to fit dealership or personal budget constraints
- Export inventory data to CSV for budget analysis
- Use exported data in Excel or Google Sheets for financial planning

---

## 🧠 Architecture Overview

This project follows a **clean and scalable architecture**, similar to real-world C++ applications.

---

### 1️⃣ Interface (Polymorphism)

```cpp
class ICommand {
public:
    virtual void execute() = 0;
    virtual ~ICommand() {}
};
