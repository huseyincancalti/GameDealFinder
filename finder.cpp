#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <map>
#include <set>

using namespace std;
namespace fs = filesystem;

struct GameEntry {
    string platform;
    string name;
    double price;
};

string toLower(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos ? "" : s.substr(a, b - a + 1));
}

vector<GameEntry> readDataFromFile(const string& filepath, const string& platform) {
    vector<GameEntry> v;
    ifstream f(filepath);
    string line;
    while (getline(f, line)) {
        stringstream ss(line);
        string name, priceStr;
        if (getline(ss, name, ';') && getline(ss, priceStr)) {
            v.push_back({platform, trim(name), stod(trim(priceStr))});
        }
    }
    return v;
}

void rewriteFile(const string& filepath, const vector<GameEntry>& v) {
    ofstream f(filepath);
    for (auto& e : v) {
        f << e.name << ";" << e.price << "\n";
    }
}

void addGamesBulk(vector<GameEntry>& allGames, const string& dataFolder) {
    cout << "\nEnter lines in format: GameName;Platform;Price\n"
         << "Leave an empty line to finish\n\n";
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line) || trim(line).empty()) break;

        size_t p1 = line.find(';');
        size_t p2 = line.rfind(';');
        if (p1 == string::npos || p2 == p1) {
            cout << "  Invalid format, skipped.\n";
            continue;
        }

        string name     = trim(line.substr(0, p1));
        string plat     = trim(line.substr(p1 + 1, p2 - p1 - 1));
        string priceStr = trim(line.substr(p2 + 1));
        double price;
        try {
            price = stod(priceStr);
        } catch (...) {
            cout << "  Invalid price, skipped.\n";
            continue;
        }

        string lowPlat  = toLower(plat);
        string filepath = dataFolder + lowPlat + ".txt";

        // Load existing entries for this platform
        auto fileGames = readDataFromFile(filepath, plat);
        bool found = false;
        for (auto& g : fileGames) {
            if (toLower(g.name) == toLower(name)) {
                g.price = price;
                found = true;
                break;
            }
        }
        if (!found) {
            fileGames.push_back({plat, name, price});
        }

        // Rewrite platform file
        rewriteFile(filepath, fileGames);

        // Update in-memory list
        bool memFound = false;
        for (auto& g : allGames) {
            if (toLower(g.name) == toLower(name) && toLower(g.platform) == lowPlat) {
                g.price = price;
                memFound = true;
                break;
            }
        }
        if (!memFound) {
            allGames.push_back({plat, name, price});
        }

        cout << "  [" << name << " @ " << plat << "] "
             << (found ? "updated" : "added")
             << " at " << price << " TL\n";
    }
    cout << "\nBulk entry complete. Press Enter to continue...";
    cin.get();
}

void listAllGames(const vector<GameEntry>& allGames) {
    cout << "\nAvailable games:\n";
    set<string> seen;
    for (auto& g : allGames) {
        string low = toLower(g.name);
        if (!seen.count(low)) {
            seen.insert(low);
            cout << "- " << g.name << "\n";
        }
    }
    cout << "---------------------------------\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void searchGame(const vector<GameEntry>& allGames) {
    cout << "Enter game name to search: ";
    string term;
    getline(cin, term);
    string lowTerm = toLower(trim(term));

    map<string, vector<GameEntry>> hits;
    for (auto& g : allGames) {
        if (toLower(g.name).find(lowTerm) != string::npos) {
            hits[toLower(g.name)].push_back(g);
        }
    }

    if (hits.empty()) {
        cout << "\nNo results found for \"" << term << "\".\n";
    } else {
        for (auto& [_, list] : hits) {
            cout << "\nGame: " << list[0].name << "\n";
            cout << "--------------------------------\n";
            cout << "| " << left << setw(15) << "Platform"
                 << "| " << setw(8) << "Price" << " |\n";
            cout << "--------------------------------\n";
            double mn = list[0].price;
            for (auto& e : list) {
                printf("| %-15s | %8.2f |\n",
                       e.platform.c_str(), e.price);
                mn = min(mn, e.price);
            }
            cout << "--------------------------------\n";
            cout << "Cheapest: " << mn << " TL on ";
            bool first = true;
            for (auto& e : list) {
                if (e.price == mn) {
                    if (!first) cout << " - ";
                    cout << e.platform;
                    first = false;
                }
            }
            cout << "\n";
        }
    }
    cout << "\nPress Enter to continue...";
    cin.get();
}

void deleteGame(vector<GameEntry>& allGames, const string& dataFolder) {
    while (true) {
        // 1) Arama terimini al
        cout << "Enter part of the game name to delete: ";
        string input;
        getline(cin, input);
        string term = toLower(trim(input));

        // 2) Eşleşen oyun isimlerini topla
        set<string> matchedNamesSet;
        for (auto& g : allGames) {
            if (toLower(g.name).find(term) != string::npos)
                matchedNamesSet.insert(g.name);
        }

        if (matchedNamesSet.empty()) {
            cout << "No games found matching \"" << input << "\".\n"
                 << "Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        // 3) Listele, 0=all seçeneği ekle
        vector<string> matchedNames(matchedNamesSet.begin(), matchedNamesSet.end());
        cout << "\nFound these games:\n";
        cout << "0) DELETE ALL LISTED GAMES\n";
        for (size_t i = 0; i < matchedNames.size(); ++i)
            cout << i+1 << ") " << matchedNames[i] << "\n";

        // 4) Geçerli seçim alana kadar tekrar sor
        set<int> choices;
        while (true) {
            cout << "Enter number(s) to delete (comma-separated): ";
            string sel;
            getline(cin, sel);

            // parse
            choices.clear();
            stringstream ss(sel);
            string tok;
            while (getline(ss, tok, ',')) {
                try {
                    int idx = stoi(trim(tok));
                    if (idx >= 0 && idx <= (int)matchedNames.size())
                        choices.insert(idx);
                } catch (...) { /* atla */ }
            }
            if (!choices.empty()) break;
            cout << "Invalid selection, please try again.\n";
        }

        // 5) Eğer "0" varsa toplu silme onayı
        if (choices.count(0)) {
            cout << "\nYou are about to delete ALL " 
                 << matchedNames.size() 
                 << " games and all their platform entries.\n"
                 << "Are you sure? (Y/N): ";
            string confirm;
            getline(cin, confirm);
            if (!confirm.empty() && (confirm[0]=='Y'||confirm[0]=='y')) {
                // toplu sil
                for (auto& nameToDelete : matchedNames) {
                    string lowName = toLower(nameToDelete);
                    // hangi platformlarda?
                    set<string> plats;
                    for (auto& g : allGames)
                        if (toLower(g.name)==lowName)
                            plats.insert(toLower(g.platform));
                    // her plat dosyasından sil
                    for (auto& plat : plats) {
                        string filepath = dataFolder + plat + ".txt";
                        auto fileGames = readDataFromFile(filepath, plat);
                        vector<GameEntry> updated;
                        for (auto& fg : fileGames)
                            if (toLower(fg.name)!=lowName)
                                updated.push_back(fg);
                        rewriteFile(filepath, updated);
                    }
                    // belleği temizle
                    allGames.erase(remove_if(allGames.begin(), allGames.end(),
                        [&](const GameEntry& g){
                            return toLower(g.name)==lowName;
                        }),
                        allGames.end());
                }
                cout << "All selected games deleted.\n";
            } else {
                cout << "Bulk deletion cancelled.\n";
            }
        }
        else {
            // 6) Tek tek isim silme
            for (int nameIdx : choices) {
                string nameToDelete = matchedNames[nameIdx-1];
                string lowName      = toLower(nameToDelete);

                // platformları bul
                set<string> plats;
                for (auto& g : allGames)
                    if (toLower(g.name)==lowName)
                        plats.insert(toLower(g.platform));

                vector<string> opts(plats.begin(), plats.end());
                cout << "\n\"" << nameToDelete << "\" found on:\n";
                for (size_t i = 0; i < opts.size(); ++i)
                    cout << i+1 << ") " << opts[i] << "\n";
                cout << "(0 = all) Enter platform numbers: ";

                // geçerli plat seçeneği al
                set<int> pchoices;
                while (true) {
                    string ps;
                    getline(cin, ps);
                    pchoices.clear();
                    stringstream ss(ps);
                    string tok;
                    while (getline(ss, tok, ',')) {
                        try {
                            int idx = stoi(trim(tok));
                            if (idx >= 0 && idx <= (int)opts.size())
                                pchoices.insert(idx);
                        } catch(...){}
                    }
                    if (!pchoices.empty()) break;
                    cout << "Invalid selection, please try again.\n";
                }

                bool deleteAllPlat = pchoices.count(0);
                for (size_t pi = 0; pi < opts.size(); ++pi) {
                    if (deleteAllPlat || pchoices.count(pi+1)) {
                        string plat = opts[pi];
                        string filepath = dataFolder + plat + ".txt";
                        auto fileGames = readDataFromFile(filepath, plat);
                        vector<GameEntry> updated;
                        for (auto& fg : fileGames)
                            if (toLower(fg.name)!=lowName)
                                updated.push_back(fg);
                        rewriteFile(filepath, updated);
                        allGames.erase(remove_if(allGames.begin(), allGames.end(),
                            [&](const GameEntry& g){
                                return toLower(g.name)==lowName
                                    && toLower(g.platform)==plat;
                            }),
                            allGames.end());
                    }
                }
                cout << "\"" << nameToDelete << "\" deleted.\n";
            }
        }

        // 7) Yeniden silmek isteyip istemediğini sor
        cout << "\nDo you want to delete another game? (Y/N): ";
        string again;
        getline(cin, again);
        if (again.empty() || (again[0]!='Y' && again[0]!='y')) {
            cout << "Returning to main menu...\n";
            break;
        }
    }

    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void updateGamePrice(vector<GameEntry>& allGames, const string& dataFolder) {
    cout << "Enter game name to update price: ";
    string name;
    getline(cin, name);
    cout << "Enter platform: ";
    string plat;
    getline(cin, plat);
    cout << "Enter new price: ";
    double price;
    cin >> price;
    cin.ignore();

    string lowName = toLower(trim(name));
    string lowPlat = toLower(trim(plat));
    string filepath = dataFolder + lowPlat + ".txt";
    auto fileGames = readDataFromFile(filepath, plat);
    bool found = false;

    for (auto& g : fileGames) {
        if (toLower(g.name) == lowName) {
            g.price = price;
            found = true;
        }
    }
    rewriteFile(filepath, fileGames);

    if (found) {
        for (auto& g : allGames) {
            if (toLower(g.name) == lowName && toLower(g.platform) == lowPlat) {
                g.price = price;
            }
        }
        cout << "Price updated successfully.\n";
    } else {
        cout << "Game not found on specified platform.\n";
    }
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    vector<GameEntry> allGames;
    string dataFolder = "data/";
    if (!fs::exists(dataFolder)) {
        fs::create_directory(dataFolder);
    }

    for (auto& e : fs::directory_iterator(dataFolder)) {
        string path     = e.path().string();
        string plat     = e.path().stem().string();
        auto entries    = readDataFromFile(path, plat);
        allGames.insert(allGames.end(), entries.begin(), entries.end());
    }

    while (true) {
        cout << "\n--- MENU ---\n"
             << "1) Search for a game\n"
             << "2) List all available games\n"
             << "3) Bulk add\n"
             << "4) Delete a game\n"
             << "5) Update the price of a game\n"
             << "6) Exit\n"
             << "Enter choice (1-6): ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            searchGame(allGames);
        } else if (choice == "2") {
            listAllGames(allGames);
        } else if (choice == "3") {
            addGamesBulk(allGames, dataFolder);
        } else if (choice == "4") {
            deleteGame(allGames, dataFolder);
        } else if (choice == "5") {
            updateGamePrice(allGames, dataFolder);
        } else if (choice == "6") {
            cout << "Exiting program. Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
