#include <iostream>
#include <tgbot/tgbot.h>
#include <fstream>
#include <sqlite3.h> 
using namespace std; 
using namespace TgBot;

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
//функция получения самого активного участника
string getMostActiveUser(sqlite3* db) {
    string sql = "SELECT username, COUNT(*) AS message_count FROM messages GROUP BY username ORDER BY message_count DESC LIMIT 1;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Ошибка при подготовке запроса: " << sqlite3_errmsg(db) << endl;
        return "";
    }

    string result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int messageCount = sqlite3_column_int(stmt, 1);
        result = "@" + username + " с количеством сообщений: " + to_string(messageCount);
    }
    else {
        result = "Нет данных для отображения.";
    }

    sqlite3_finalize(stmt);
    return result;
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

    // инициализация БД
    sqlite3* db;
    int exit = sqlite3_open("chat_messages.db", &db);
    if (exit) {
        cerr << "Не удалось открыть базу данных: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    else {
        cout << "База данных успешно открыта!\n";
    }
    createTable(db);

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

    // обработчик входящих сообщений
    bot.getEvents().onAnyMessage([&bot, db](Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/")) {
            return;
        }
        insertMessage(db, to_string(message->from->id), message->from->username, message->text);
        });

    // команда /active
    bot.getEvents().onCommand("active", [&bot, db](Message::Ptr message) {
        string mostActiveUser = getMostActiveUser(db);
        bot.getApi().sendMessage(message->chat->id, mostActiveUser);
        });
}

