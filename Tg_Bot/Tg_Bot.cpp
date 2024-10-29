#include <iostream>
#include <tgbot/tgbot.h>
#include <fstream>
#include <sqlite3.h> 
using namespace std; 

//функция извлечения токена из файла
string getToken(const string& filePath) {
    ifstream file(filePath);

    if (!file.is_open())
        throw runtime_error("Не удалось открыть файл с токеном.");

    string token;
    getline(file, token);
    return token;
}
//функция создания таблицы для сообщений
void createTable(sqlite3* db) {
    const char* sql = "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id TEXT NOT NULL, "
        "username TEXT NOT NULL, "
        "message TEXT NOT NULL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* errorMessage = nullptr;
    int rc = sqlite3_exec(db, sql, 0, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cerr << "Ошибка при создании таблицы: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}
//функция вставки сообщения в таблицу
void insertMessage(sqlite3* db, const string& userId, const string& username, const string& message) {
    string sql = "INSERT INTO messages (user_id, username, message) VALUES ('" + userId + "', '" + username + "', '" + message + "');";

    char* errorMessage = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cerr << "Ошибка при вставке сообщения: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
}

int main()
{
    setlocale(LC_ALL, "Ru");

    //вытягивание токена из файла
    string filePath = "C:\\Users\\gafar\\Desktop\\token.txt";
    string token;

    try {
        token = getToken(filePath);
    }
    catch(const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return 1;
    }

    //инициализация бота
    TgBot::Bot bot(token);

    //проверка запуска бота
    try {
        cout << "Бот запущен...\n";
        TgBot::TgLongPoll longPoll(bot);
        while (true) 
            longPoll.start();  
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }

}

