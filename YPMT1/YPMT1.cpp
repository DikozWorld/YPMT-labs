#include <iostream>
#include <map>
#include <string>
#include <windows.h>

using namespace std;

// Ключевые слова
map<string, int> keywords;

void initKeywords() {
    keywords["do"] = 1;
    keywords["while"] = 2;
    keywords["print"] = 3;
}

int find_word(const string& word) {
    auto it = keywords.find(word);
    return (it != keywords.end()) ? it->second : 0;
}

// Идентификаторы
map<string, int> identifiers;
int nextId = 1;

int make_id(const string& name) {
    if (identifiers.find(name) != identifiers.end()) {
        return identifiers[name];
    }
    identifiers[name] = nextId;
    return nextId++;
}

// Константы
map<int, int> constToCode;
map<int, int> codeToConst;
int nextConstCode = 1;

int make_dig(int value) {
    if (constToCode.find(value) != constToCode.end()) {
        return constToCode[value];
    }
    constToCode[value] = nextConstCode;
    codeToConst[nextConstCode] = value;
    return nextConstCode++;
}

int val_dig(int code) {
    return (codeToConst.find(code) != codeToConst.end()) ? codeToConst[code] : -1;
}

int main() {
    SetConsoleOutputCP(1251);
    initKeywords();
    cout << "Лаба 1" << endl;
    cout << "Информационные таблицы" << endl;
    cout << "Вариант: S -> do S{;S} while B | id = E | print id" << endl;
    cout << endl;

    // Тестируем
    cout << "1. Таблица ключевых слов:" << endl;
    cout << "   find_word('do') = " << find_word("do") << endl;
    cout << "   find_word('while') = " << find_word("while") << endl;
    cout << "   find_word('print') = " << find_word("print") << endl;
    cout << "   find_word('if') = " << find_word("if") << " (не ключевое слово)" << endl;
    cout << endl;

    cout << "2. Таблица идентификаторов:" << endl;
    cout << "   make_id('x') = " << make_id("x") << endl;
    cout << "   make_id('y') = " << make_id("y") << endl;
    cout << "   make_id('x') = " << make_id("x") << " (второй раз)" << endl;
    cout << endl;

    cout << "3. Таблица констант:" << endl;
    cout << "   make_dig(10) = " << make_dig(10) << endl;
    cout << "   make_dig(255) = " << make_dig(255) << endl;
    cout << "   make_dig(10) = " << make_dig(10) << " (второй раз)" << endl;
    cout << "   val_dig(1) = " << val_dig(1) << endl;
    cout << "   val_dig(2) = " << val_dig(2) << endl;

    return 0;
}