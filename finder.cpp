#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;
namespace fs = filesystem;

struct GameEntry {
    string platform;
    string name;
    double price;
};

string toLower(const string& str) {
    string lowerStr = str;
    transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
    return (first == string::npos) ? "" : str.substr(first, last - first + 1);
}

vector<GameEntry> readDataFromFile(const string& filepath, const string& platformName) {
    vector<GameEntry> games;
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filepath << "\n";
        return games;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string name, priceStr;
        if (getline(ss, name, ';') && getline(ss, priceStr)) {
            GameEntry entry;
            entry.platform = platformName;
            entry.name = trim(name);
            entry.price = stod(priceStr);
            games.push_back(entry);
        }
    }
    return games;
}

void listAllGames(const vector<GameEntry>& allGames) {
    cout << "\nAvailable games in database:\n";
    vector<string> gameNames;

    for (const auto& game : allGames) {
        string lowered = toLower(game.name);
        if (find(gameNames.begin(), gameNames.end(), lowered) == gameNames.end()) {
            gameNames.push_back(lowered);
            cout << "- " << game.name << "\n";
        }
    }

    cout << "---------------------------------------------\n";
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Burada Enter'a basıldığında işlemi sonlandırır.
}

void searchGame(const string& inputName, const vector<GameEntry>& allGames) {
    string searchTerm = toLower(trim(inputName));
    vector<GameEntry> matches;

    for (const auto& game : allGames) {
        if (toLower(game.name).find(searchTerm) != string::npos) {
            matches.push_back(game);
        }
    }

    if (matches.empty()) {
        cout << "\nNo data found for \"" << inputName << "\".\n";
    } else {
        // Gruplama için map (oyun adı → oyun listesi)
        map<string, vector<GameEntry>> groupedGames;
        for (const auto& game : matches) {
            string key = toLower(game.name);
            groupedGames[key].push_back(game);
        }

        cout << "\n--- Search results for \"" << inputName << "\": ---\n";

        for (const auto& group : groupedGames) {
            const vector<GameEntry>& gameList = group.second;

            // Oyun adını yazdır
            cout << "\nGame: " << gameList[0].name << "\n";

            // Başlıklar
            cout << "-------------------------------------------------------------\n";
            cout << "| " << left << setw(30) << "Platform" << " | "
                 << left << setw(10) << "Price (TL)" << " |\n";
            cout << "-------------------------------------------------------------\n";

            double minPrice = gameList[0].price;
            for (const auto& game : gameList) {
                printf("| %-30s | %-10.2f |\n", game.platform.c_str(), game.price);
                if (game.price < minPrice) {
                    minPrice = game.price;
                }
            }

            // En ucuz fiyatı sunan platformları listele
            vector<string> cheapestPlatforms;
            for (const auto& game : gameList) {
                if (game.price == minPrice) {
                    cheapestPlatforms.push_back(game.platform);
                }
            }

            // Tek satırda yazdır
            cout << "-------------------------------------------------------------\n";
            cout << "Cheapest Deal: " << minPrice << " TL on: ";
            for (size_t i = 0; i < cheapestPlatforms.size(); ++i) {
                cout << cheapestPlatforms[i];
                if (i != cheapestPlatforms.size() - 1) {
                    cout << " - ";
                }
            }
            cout << "\n=============================================================\n";
        }
    }

    cout << "Press Enter to continue...";
    cin.get();
}



void addGame(vector<GameEntry>& allGames, const string& dataFolder) {
    string name, platform;
    double price;

    cout << "Enter game name: ";
    getline(cin, name);
    cout << "Enter platform name (cannot be empty): ";
    getline(cin, platform);

    name = trim(name);
    platform = trim(platform);

    if (platform.empty()) {
        cout << "Platform name cannot be empty. Game not added.\n";
        cout << "---------------------------------------------\n";
        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Burada Enter'a basıldığında işlemi sonlandırır.
        return;
    }

    cout << "Enter price (TL): ";
    cin >> price;
    cin.ignore();

    GameEntry newGame = {platform, name, price};
    allGames.push_back(newGame);

    string filepath = dataFolder + toLower(platform) + ".txt";
    ofstream outFile(filepath, ios::app);
    if (outFile.is_open()) {
        outFile << name << ";" << price << "\n";
        outFile.close();
        cout << "Game added successfully to " << platform << ".\n";
    } else {
        cerr << "Failed to write to file: " << filepath << "\n";
    }

    cout << "---------------------------------------------\n";
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Burada Enter'a basıldığında işlemi sonlandırır.
}

void deleteGame(vector<GameEntry>& allGames, const string& dataFolder) {
    string nameToDelete;
    cout << "Enter game name to delete: ";
    getline(cin, nameToDelete);
    string loweredName = toLower(trim(nameToDelete));
    bool found = false;

    for (const auto& entry : fs::directory_iterator(dataFolder)) {
        string filepath = entry.path().string();
        string platform = entry.path().stem().string();
        vector<GameEntry> games = readDataFromFile(filepath, platform);
        vector<GameEntry> updatedGames;

        for (const auto& game : games) {
            if (toLower(game.name).find(loweredName) == string::npos) {
                updatedGames.push_back(game);
            } else {
                found = true;
            }
        }

        ofstream outFile(filepath);
        if (outFile.is_open()) {
            for (const auto& game : updatedGames) {
                outFile << game.name << ";" << game.price << "\n";
            }
        } else {
            cerr << "Failed to open file for writing: " << filepath << "\n";
        }
    }

    if (found) {
        allGames.erase(remove_if(allGames.begin(), allGames.end(), [&](const GameEntry& g) {
            return toLower(g.name).find(loweredName) != string::npos;
        }), allGames.end());
        cout << "Game(s) successfully deleted.\n";
    } else {
        cout << "Game not found.\n";
    }
    cout << "---------------------------------------------\n";
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Burada Enter'a basıldığında işlemi sonlandırır.
}

void updateGamePrice(vector<GameEntry>& allGames, const string& dataFolder) {
    string gameName, platform;
    double newPrice;

    cout << "Enter game name to update: ";
    getline(cin, gameName);
    cout << "Enter platform name: ";
    getline(cin, platform);
    cout << "Enter new price: ";
    cin >> newPrice;
    cin.ignore();

    gameName = trim(gameName);
    platform = trim(platform);

    string loweredName = toLower(gameName);
    string loweredPlatform = toLower(platform);
    bool updated = false;

    string filepath = dataFolder + loweredPlatform + ".txt";
    vector<GameEntry> games = readDataFromFile(filepath, platform);

    for (auto& game : games) {
        if (toLower(game.name) == loweredName && toLower(game.platform) == loweredPlatform) {
            game.price = newPrice;
            updated = true;
        }
    }

    ofstream outFile(filepath);
    if (outFile.is_open()) {
        for (const auto& game : games) {
            outFile << game.name << ";" << game.price << "\n";
        }
    } else {
        cerr << "Failed to open file for writing: " << filepath << "\n";
    }

    if (updated) {
        for (auto& game : allGames) {
            if (toLower(game.name) == loweredName && toLower(game.platform) == loweredPlatform) {
                game.price = newPrice;
            }
        }
        cout << "Game price updated successfully.\n";
    } else {
        cout << "Game not found.\n";
    }
    cout << "---------------------------------------------\n";
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Burada Enter'a basıldığında işlemi sonlandırır.
}

int main() {
    vector<GameEntry> allGames;
    string dataFolder = "data/";

    if (!fs::exists(dataFolder)) {
        fs::create_directory(dataFolder);
    }

    for (const auto& entry : fs::directory_iterator(dataFolder)) {
        string path = entry.path().string();
        string platform = entry.path().stem().string();
        vector<GameEntry> games = readDataFromFile(path, platform);
        allGames.insert(allGames.end(), games.begin(), games.end());
    }

    string choice;
    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Search for a game\n";
        cout << "2. List all available games\n";
        cout << "3. Add a new game\n";
        cout << "4. Delete a game\n";
        cout << "5. Update game price\n";
        cout << "6. Exit\n";
        cout << "Enter your choice (1-6): ";
        getline(cin, choice);

        if (choice == "1") {
            string gameName;
            cout << "Enter game name: ";
            getline(cin, gameName);
            searchGame(gameName, allGames);
        }
        else if (choice == "2") {
            listAllGames(allGames);
        }
        else if (choice == "3") {
            addGame(allGames, dataFolder);
        }
        else if (choice == "4") {
            deleteGame(allGames, dataFolder);
        }
        else if (choice == "5") {
            updateGamePrice(allGames, dataFolder);
        }
        else if (choice == "6") {
            cout << "Exiting program. Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
