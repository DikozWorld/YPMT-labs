#include <iostream>
#include "tables.h"
#include "scanner.h"
#include "parser.h"

using namespace std;

void demonstrateParserExamples() {
    cout << "\n=== LABORATORY WORK 3: SYNTAX ANALYZER ===" << endl;

    vector<pair<string, string>> testCases = {
        {"do x = 10; print x while x < 20#", "accepted"},
        {"x = 5#", "accepted"},
        {"print y#", "accepted"},
        {"do a = 1; b = a + 5 while b > 0#", "accepted"},
        {"a = 1 + 2 - 3#", "accepted"},
        {"do x = 5; y = x + 10; print y while y < 100#", "accepted"},
        {"x = y + 15#", "accepted"},
        {"do print counter; counter = counter + 1 while counter < 10#", "accepted"},

        {"do x = 10; print x while x <#", "Error: B: expected expression after operator"},
        {"do x = 10; print x while < 20#", "Error: B: expected expression"},
        {"x = #", "Error: E: expected term"},
        {"print#", "Error: S: expected identifier after 'print'"},
        {"do while x < 20#", "Error: S: expected statement after 'do'"},
        {"do x = 10; ; while x < 20#", "Error: S: expected 'do', identifier, or 'print'"},
        {"x = 5 +#", "Error: T: expected identifier or number"},
        {"do x = 5 while#", "Error: B: expected expression"},
        {"x = 5 10#", "Error: expected ';' or end of program"},
        {"do x = 5; print x while#", "Error: B: expected expression"},
        {"print 123#", "Error: S: expected identifier after 'print'"},
        {"do x = + 5 while x < 10#", "Error: T: expected identifier or number"}
    };

    for (size_t i = 0; i < testCases.size(); i++) {
        cout << "\nExample " << (i + 1) << ":" << endl;
        cout << "Program: \"" << testCases[i].first << "\"" << endl;

        vector<Token> tokens = scanner(testCases[i].first);
        cout << "Tokens: ";
        print_tokens(tokens);

        Parser parser(tokens);
        bool success = parser.parse();

        if (success && testCases[i].second == "accepted") {
            cout << "✓ " << testCases[i].second << endl;
        }
        else if (!success && testCases[i].second.find("Error:") != string::npos) {
            cout << "✗ " << testCases[i].second << endl;
            parser.printErrors();
        }
        else {
            cout << "✗ UNEXPECTED RESULT!" << endl;
            if (success) {
                cout << "Error expected, but program was accepted" << endl;
            }
            else {
                parser.printErrors();
            }
        }
    }
}

void printRecursiveDescentSchemes() {
    cout << "\n=== RECURSIVE DESCENT PROCEDURE SCHEMES ===" << endl;

    cout << "\nPROCEDURE S SCHEME:" << endl;
    cout << "   IF current token = 'do' THEN" << endl;
    cout << "      consume 'do'" << endl;
    cout << "      CALL S (first statement)" << endl;
    cout << "      WHILE current token = ';' DO" << endl;
    cout << "         consume ';'" << endl;
    cout << "         CALL S (next statement)" << endl;
    cout << "      consume 'while'" << endl;
    cout << "      CALL B (condition)" << endl;
    cout << "   ELSE IF current token = identifier THEN" << endl;
    cout << "      consume identifier" << endl;
    cout << "      consume '='" << endl;
    cout << "      CALL E (expression)" << endl;
    cout << "   ELSE IF current token = 'print' THEN" << endl;
    cout << "      consume 'print'" << endl;
    cout << "      consume identifier" << endl;
    cout << "   ELSE ERROR: 'expected do, identifier or print'" << endl;

    cout << "\nPROCEDURE B SCHEME (B → E < E | E > E):" << endl;
    cout << "   CALL E (left expression)" << endl;
    cout << "   IF current token = '<' THEN" << endl;
    cout << "      consume '<'" << endl;
    cout << "   ELSE IF current token = '>' THEN" << endl;
    cout << "      consume '>'" << endl;
    cout << "   ELSE ERROR: 'expected < or >'" << endl;
    cout << "   CALL E (right expression)" << endl;

    cout << "\nPROCEDURE E SCHEME (E → T {+T | -T}):" << endl;
    cout << "   CALL T (first term)" << endl;
    cout << "   WHILE current token = '+' OR '-' DO" << endl;
    cout << "      consume operator ('+' or '-')" << endl;
    cout << "      CALL T (next term)" << endl;

    cout << "\nPROCEDURE T SCHEME (T → num | id):" << endl;
    cout << "   IF current token = identifier THEN" << endl;
    cout << "      consume identifier" << endl;
    cout << "   ELSE IF current token = number THEN" << endl;
    cout << "      consume number" << endl;
    cout << "   ELSE ERROR: 'expected identifier or number'" << endl;
}

int main() {
    initKeywords();

    cout << "LABORATORY WORKS 1, 2 and 3" << endl;
    cout << "1. Information tables" << endl;
    cout << "2. Lexical analyzer (scanner)" << endl;
    cout << "3. Syntax analyzer (parser)" << endl;
    cout << "Language grammar:" << endl;
    cout << "S → do S{;S} while B | id = E | print id" << endl;
    cout << "B → E < E | E > E" << endl;
    cout << "E → T {+T | -T}" << endl;
    cout << "T → num | id" << endl;
    cout << endl;

    cout << "PART 1: INFORMATION TABLES" << endl;

    cout << "\n1. Keywords table:" << endl;
    cout << "   find_word(\"do\") = " << find_word("do") << endl;
    cout << "   find_word(\"while\") = " << find_word("while") << endl;
    cout << "   find_word(\"print\") = " << find_word("print") << endl;
    cout << "   find_word(\"if\") = " << find_word("if") << " (not a keyword)" << endl;

    cout << "\n2. Identifiers table:" << endl;
    cout << "   make_id(\"x\") = " << make_id("x") << endl;
    cout << "   make_id(\"y\") = " << make_id("y") << endl;
    cout << "   make_id(\"counter\") = " << make_id("counter") << endl;
    cout << "   make_id(\"x\") = " << make_id("x") << " (again - same code)" << endl;

    cout << "\n3. Constants table:" << endl;
    cout << "   make_dig(10) = " << make_dig(10) << endl;
    cout << "   make_dig(255) = " << make_dig(255) << endl;
    cout << "   make_dig(5) = " << make_dig(5) << endl;
    cout << "   make_dig(10) = " << make_dig(10) << " (again - same code)" << endl;
    cout << "   val_dig(1) = " << val_dig(1) << endl;
    cout << "   val_dig(2) = " << val_dig(2) << endl;

    cout << "\n\nPART 2: LEXICAL ANALYZER" << endl;

    string program1 = "do x = 10; print x while x < 20#";
    cout << "\nExample 1: \"" << program1 << "\"" << endl;
    vector<Token> tokens1 = scanner(program1);
    cout << "Token vector: ";
    print_tokens(tokens1);

    string program2 = "do x = x + 5; y = 100; print y while y > 0#";
    cout << "\nExample 2: \"" << program2 << "\"" << endl;
    vector<Token> tokens2 = scanner(program2);
    cout << "Token vector: ";
    print_tokens(tokens2);

    string program3 = "a = 1 + 2 - 3#";
    cout << "\nExample 3: \"" << program3 << "\"" << endl;
    vector<Token> tokens3 = scanner(program3);
    cout << "Token vector: ";
    print_tokens(tokens3);

    cout << "\nToken to source code correspondence (Example 1):" << endl;
    demonstrate_token_correspondence(program1, tokens1);

    print_state_diagram();

    cout << "\nTABLE CONTENTS" << endl;

    cout << "\nIdentifiers table:" << endl;
    for (const auto& id : identifiers) {
        cout << "  " << id.first << " -> code " << id.second << endl;
    }

    cout << "\nConstants table:" << endl;
    for (const auto& cnst : codeToConst) {
        cout << "  code " << cnst.first << " -> value " << cnst.second << endl;
    }

    printRecursiveDescentSchemes();
    demonstrateParserExamples();

    cout << "\nADDITIONAL TEST" << endl;
    cout << "Enter a program to check (end with #):" << endl;
    cout << "Example: do x = 10; print x while x < 20#" << endl;
    cout << "Input: ";

    string userProgram;
    getline(cin, userProgram);

    if (userProgram.find('#') == string::npos) {
        userProgram += "#";
    }

    cout << "\nChecking program: \"" << userProgram << "\"" << endl;

    vector<Token> userTokens = scanner(userProgram);
    cout << "Tokens: ";
    print_tokens(userTokens);

    Parser userParser(userTokens);
    bool userSuccess = userParser.parse();

    if (userSuccess) {
        cout << "✓ Program is syntactically correct" << endl;
        cout << "\nParse tree:" << endl;
        userParser.printParseTree();
    }
    else {
        cout << "✗ Program contains syntax errors" << endl;
        userParser.printErrors();
    }

    cout << "\n=== EXECUTION COMPLETED ===" << endl;
    system("pause");
    return 0;
}
