#include <iostream>
#include <tgbot/tgbot.h>
#include <fstream>
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

