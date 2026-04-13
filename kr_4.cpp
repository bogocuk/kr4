#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <clocale>
#include <cstring>
#include <algorithm>

using namespace std;


enum Comfort { LUX, JUNIOR_SUITE, STANDARD, ECONOMY };


struct RoomDetails {
    char options[100];
    bool seaView;
};

struct HotelRoom {
    char hotelName[50];
    int roomNumber;
    Comfort comfort;
    int capacity;
    double price;
    RoomDetails extra; 
};



void printRoom(const HotelRoom& r) {
    cout << "Отель: " << r.hotelName << " | №" << r.roomNumber
        << " | Цена: " << r.price << " | Опции: " << r.extra.options
        << (r.extra.seaView ? " [Море]" : "") << endl;
}

void displayFilteredRooms(const vector<HotelRoom>& rooms, string title) {
    cout << "\n--- " << title << " ---" << endl;
    if (rooms.empty()) {
        cout << "Список пуст." << endl;
        return;
    }
    for (const auto& r : rooms) {
        printRoom(r);
    }
}

void saveToBinary(const char* filename, const vector<HotelRoom>& rooms) {
    ofstream fout(filename, ios::binary | ios::trunc);
    if (fout.is_open()) {
        size_t count = rooms.size();
        fout.write((char*)&count, sizeof(count));
        fout.write((char*)rooms.data(), count * sizeof(HotelRoom));
        fout.close();
    }
}

void loadFromBinary(const char* filename, vector<HotelRoom>& rooms) {
    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) return;
    size_t count;
    fin.read((char*)&count, sizeof(count));
    rooms.resize(count);
    fin.read((char*)rooms.data(), count * sizeof(HotelRoom));
    fin.close();
}

void syncDataFromText(vector<HotelRoom>& rooms) {
    ifstream txtFile("updates.txt");
    if (!txtFile.is_open()) return;
    int fileRoomNum;
    double fileNewPrice;
    while (txtFile >> fileRoomNum >> fileNewPrice) {
        for (auto& r : rooms) {
            if (r.roomNumber == fileRoomNum) {
                r.price = fileNewPrice;
            }
        }
    }
    txtFile.close();
}

void bubbleSort(vector<HotelRoom>& rooms) {
    if (rooms.empty()) return;
    for (size_t i = 0; i < rooms.size(); i++)
        for (size_t j = 0; j < rooms.size() - 1 - i; j++)
            if (rooms[j].price > rooms[j + 1].price)
                swap(rooms[j], rooms[j + 1]);
}

int main() {
    setlocale(LC_ALL, "Russian");
    const char* binFile = "hotel_data.dat";
    vector<HotelRoom> allRooms;

    loadFromBinary(binFile, allRooms);

    if (allRooms.empty()) {
        allRooms = {
            {"Grand Plaza", 101, LUX, 2, 8000, {"WiFi, Bar", true}},
            {"Ocean Plaza", 102, STANDARD, 2, 2000, {"TV, AC", true}},
            {"Sky Plaza", 103, ECONOMY, 1, 1000, {"None", false}},
            {"Beach Resort", 104, LUX, 2, 9000, {"All inclusive", true}},
            {"City Plaza", 105, STANDARD, 1, 1500, {"WiFi", false}},
            {"Golden Plaza", 201, JUNIOR_SUITE, 2, 4500, {"TV, Balcony", true}},
            {"Mountain View", 202, ECONOMY, 2, 1300, {"Heater", false}},
            {"Royal Plaza", 203, LUX, 3, 12000, {"Jacuzzi, Gym", false}},
            {"Sea Breeze", 204, STANDARD, 2, 3200, {"AC, Terrace", true}},
            {"Central Hotel", 301, ECONOMY, 1, 900, {"None", false}},
            {"Emerald Plaza", 302, JUNIOR_SUITE, 2, 5500, {"WiFi, Coffee", true}},
            {"Sunset Plaza", 303, STANDARD, 2, 2800, {"TV, Sea-view", true}},
            {"Park Inn", 304, ECONOMY, 1, 1100, {"WiFi", false}},
            {"Marina Plaza", 401, LUX, 2, 8800, {"Bar, Pool", true}},
            {"Nordic Star", 402, STANDARD, 3, 3500, {"TV, AC", false}},
            {"Azure Plaza", 403, JUNIOR_SUITE, 2, 5100, {"Balcony", true}},
            {"Hostel 24", 501, ECONOMY, 4, 800, {"Kitchen", false}},
            {"Elite Plaza", 502, LUX, 2, 15000, {"Spa, Chef", true}},
            {"Green Valley", 503, STANDARD, 2, 2200, {"Garden", false}}
        };
    }

    syncDataFromText(allRooms);


    int searchId = 102;
    cout << "\n--- Поиск номера " << searchId << " ---" << endl;
    bool found = false;
    for (const auto& r : allRooms) {
        if (r.roomNumber == searchId) {
            printRoom(r);
            found = true;
            break;
        }
    }
    if (!found) cout << "Номер не найден." << endl;

 
    vector<HotelRoom> plazaRooms;
    for (const auto& r : allRooms) {
        string name = r.hotelName;
        if (name.find("Plaza") != string::npos) {
            plazaRooms.push_back(r);
        }
    }
    bubbleSort(plazaRooms);

    cout << "\n--- ТОП-3 дешевых в Plaza ---" << endl;
    for (int i = 0; i < 3 && i < (int)plazaRooms.size(); i++) {
        cout << "№" << plazaRooms[i].roomNumber
            << " | Цена: " << plazaRooms[i].price
            << " | Опции: " << plazaRooms[i].extra.options << endl;
    }


    vector<HotelRoom> seaViewRooms;
    for (const auto& r : allRooms) {
        if (r.extra.seaView) seaViewRooms.push_back(r);
    }

    displayFilteredRooms(seaViewRooms, "Номера с видом на море");

    saveToBinary(binFile, allRooms);

    return 0;
}
