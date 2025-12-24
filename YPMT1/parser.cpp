#include "parser.h"
#include <iostream>
#include <stack>
#include <iomanip>

using namespace std;

Parser::Parser(const vector<Token>& tokenList)
    : tokens(tokenList), currentPos(0), root(nullptr), errorFlag(false) {
}

Parser::~Parser() {
    if (root) {
        delete root;
    }
}

Token Parser::currentToken() const {
    if (currentPos < tokens.size()) {
        return tokens[currentPos];
    }
    return { TOKEN_END, 0 };
}

Token Parser::peekToken() const {
    if (currentPos + 1 < tokens.size()) {
        return tokens[currentPos + 1];
    }
    return { TOKEN_END, 0 };
}

void Parser::consumeToken() {
    if (currentPos < tokens.size()) {
        currentPos++;
    }
}

void Parser::match(TokenTypeEnum expectedType, int expectedCode) {
    Token current = currentToken();

    if (current.type == TOKEN_END && expectedType == TOKEN_END) {
        return;
    }

    if (current.type != expectedType) {
        string typeName;
        switch (expectedType) {
        case TOKEN_ID: typeName = "identifier"; break;
        case TOKEN_DIG: typeName = "number"; break;
        case TOKEN_WORD:
            if (expectedCode == 1) typeName = "'do'";
            else if (expectedCode == 2) typeName = "'while'";
            else if (expectedCode == 3) typeName = "'print'";
            else if (expectedCode == 4) typeName = "'='";
            else if (expectedCode == 5) typeName = "'<'";
            else if (expectedCode == 6) typeName = "'>'";
            else if (expectedCode == 7) typeName = "'+'";
            else if (expectedCode == 8) typeName = "'-'";
            else if (expectedCode == 9) typeName = "';'";
            else typeName = "operator";
            break;
        case TOKEN_END: typeName = "end of program (#)"; break;
        }
        error("expected " + typeName);
        return;
    }

    if (expectedCode != -1 && current.code != expectedCode) {
        error("unexpected token code");
        return;
    }

    consumeToken();
}

void Parser::error(const string& message) {
    if (!errorFlag) {
        errorFlag = true;
        errorMessage = "Error: " + message;
    }
}

bool Parser::parse() {
    root = parseS();

    if (currentToken().type != TOKEN_END) {
        error("expected end of program (#)");
    }

    return !errorFlag;
}

ParseTreeNode* Parser::parseS() {
    Token current = currentToken();

    if (current.type == TOKEN_WORD && current.code == 1) { // do
        ParseTreeNode* doWhileNode = new ParseTreeNode(NODE_DO_WHILE, "do-while");

        match(TOKEN_WORD, 1);

        vector<ParseTreeNode*> statements = parseStatementList();
        if (statements.empty()) {
            error("S: expected statement after 'do'");
            delete doWhileNode;
            return nullptr;
        }
        for (auto stmt : statements) {
            doWhileNode->children.push_back(stmt);
        }

        if (currentToken().type == TOKEN_WORD && currentToken().code == 2) {
            match(TOKEN_WORD, 2); // while
        }
        else {
            error("S: expected 'while' after statements");
            delete doWhileNode;
            return nullptr;
        }

        ParseTreeNode* condition = parseB();
        if (condition) {
            doWhileNode->children.push_back(condition);
        }
        else {
            delete doWhileNode;
            return nullptr;
        }

        return doWhileNode;
    }
    else if (current.type == TOKEN_ID) {
        ParseTreeNode* assignNode = new ParseTreeNode(NODE_ASSIGNMENT, "=");

        ParseTreeNode* idNode = new ParseTreeNode(NODE_IDENTIFIER,
            to_string(current.code));
        assignNode->children.push_back(idNode);

        match(TOKEN_ID);

        if (currentToken().type == TOKEN_WORD && currentToken().code == 4) {
            match(TOKEN_WORD, 4); // =
        }
        else {
            error("S: expected '=' after identifier");
            delete assignNode;
            return nullptr;
        }

        ParseTreeNode* expr = parseE();
        if (expr) {
            assignNode->children.push_back(expr);
        }
        else {
            delete assignNode;
            return nullptr;
        }

        return assignNode;
    }
    else if (current.type == TOKEN_WORD && current.code == 3) { // print
        ParseTreeNode* printNode = new ParseTreeNode(NODE_PRINT, "print");

        match(TOKEN_WORD, 3);

        if (currentToken().type == TOKEN_ID) {
            ParseTreeNode* idNode = new ParseTreeNode(NODE_IDENTIFIER,
                to_string(currentToken().code));
            printNode->children.push_back(idNode);
            match(TOKEN_ID);
        }
        else {
            error("S: expected identifier after 'print'");
            delete printNode;
            return nullptr;
        }

        return printNode;
    }
    else {
        error("S: expected 'do', identifier, or 'print'");
        return nullptr;
    }
}

vector<ParseTreeNode*> Parser::parseStatementList() {
    vector<ParseTreeNode*> statements;

    ParseTreeNode* stmt = parseS();
    if (stmt) {
        statements.push_back(stmt);
    }
    else {
        return statements;
    }

    while (currentToken().type == TOKEN_WORD && currentToken().code == 9) { // ;
        match(TOKEN_WORD, 9); // ;

        if (currentToken().type != TOKEN_WORD || currentToken().code != 2) { // not 'while'
            stmt = parseS();
            if (stmt) {
                statements.push_back(stmt);
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }

    return statements;
}

ParseTreeNode* Parser::parseB() {
    ParseTreeNode* comparisonNode = new ParseTreeNode(NODE_COMPARISON);

    ParseTreeNode* leftExpr = parseE();
    if (!leftExpr) {
        error("B: expected expression");
        delete comparisonNode;
        return nullptr;
    }
    comparisonNode->children.push_back(leftExpr);

    Token opToken = currentToken();
    if (opToken.type == TOKEN_WORD && opToken.code == 5) { // <
        comparisonNode->value = "<";
        match(TOKEN_WORD, 5);
    }
    else if (opToken.type == TOKEN_WORD && opToken.code == 6) { // >
        comparisonNode->value = ">";
        match(TOKEN_WORD, 6);
    }
    else {
        error("B: expected '<' or '>'");
        delete comparisonNode;
        return nullptr;
    }

    ParseTreeNode* rightExpr = parseE();
    if (!rightExpr) {
        error("B: expected expression after operator");
        delete comparisonNode;
        return nullptr;
    }
    comparisonNode->children.push_back(rightExpr);

    return comparisonNode;
}

ParseTreeNode* Parser::parseE() {
    ParseTreeNode* leftTerm = parseT();
    if (!leftTerm) {
        error("E: expected term");
        return nullptr;
    }

    while (currentToken().type == TOKEN_WORD &&
        (currentToken().code == 7 || currentToken().code == 8)) {

        string opStr = (currentToken().code == 7) ? "+" : "-";
        ParseTreeNode* opNode = new ParseTreeNode(NODE_BINARY_OP, opStr);
        opNode->children.push_back(leftTerm);

        match(TOKEN_WORD);

        ParseTreeNode* rightTerm = parseT();
        if (!rightTerm) {
            error("E: expected term after operator");
            delete opNode;
            return nullptr;
        }

        opNode->children.push_back(rightTerm);
        leftTerm = opNode;
    }

    return leftTerm;
}

ParseTreeNode* Parser::parseT() {
    Token current = currentToken();

    if (current.type == TOKEN_ID) {
        ParseTreeNode* idNode = new ParseTreeNode(NODE_IDENTIFIER,
            to_string(current.code));
        match(TOKEN_ID);
        return idNode;
    }
    else if (current.type == TOKEN_DIG) {
        ParseTreeNode* constNode = new ParseTreeNode(NODE_CONSTANT,
            to_string(current.code));
        match(TOKEN_DIG);
        return constNode;
    }
    else {
        error("T: expected identifier or number");
        return nullptr;
    }
}

void printTreeHelper(ParseTreeNode* node, int depth = 0) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        cout << "  ";
    }

    string typeStr;
    switch (node->type) {
    case NODE_PROGRAM: typeStr = "PROGRAM"; break;
    case NODE_DO_WHILE: typeStr = "DO-WHILE"; break;
    case NODE_ASSIGNMENT: typeStr = "ASSIGN"; break;
    case NODE_PRINT: typeStr = "PRINT"; break;
    case NODE_BINARY_OP: typeStr = "BIN_OP"; break;
    case NODE_COMPARISON: typeStr = "COMPARE"; break;
    case NODE_IDENTIFIER: typeStr = "ID"; break;
    case NODE_CONSTANT: typeStr = "CONST"; break;
    case NODE_EXPRESSION: typeStr = "EXPR"; break;
    case NODE_TERM: typeStr = "TERM"; break;
    case NODE_FACTOR: typeStr = "FACTOR"; break;
    }

    cout << "[" << typeStr;
    if (!node->value.empty()) {
        cout << ": " << node->value;
    }
    cout << "]" << endl;

    for (auto child : node->children) {
        printTreeHelper(child, depth + 1);
    }
}

void Parser::printParseTree() const {
    if (!root) {
        cout << "No parse tree (parsing failed or not performed)" << endl;
        return;
    }

    cout << "\n=== PARSE TREE ===" << endl;
    printTreeHelper(root);
}

void Parser::printErrors() const {
    if (errorFlag) {
        cout << errorMessage << endl;
    }
    else {
        cout << "No parsing errors!" << endl;
    }
}
