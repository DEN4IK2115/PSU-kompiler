#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <list>
#include <map>

#include "decw.h"
#include "deckey.h"

using namespace std;

const int ErrMax = 80;
const unsigned MAX_IDENT = 20; //максимальная длина идентификатора

struct textposition { //Структура позиции лексемы в тексте
    int linenumber;
    int charnumber;
};
struct Err { //Струтура положения ощибки в программе
    struct textposition errorposition;
    unsigned errorcode;
};
//struct Key {
//    unsigned codekey;
//    char namekey[9];
//};

static textposition positionnow;
static textposition token;
static Err ErrList[ErrMax];
//static Key keywords[51];

static ifstream Prog("//home//DEN4IK2115//GitHub//kompile//Prog.txt");
static ofstream ch_file("//home//DEN4IK2115//GitHub//kompile//ch_file.txt", ios::out);
static ofstream symbol_file("//home//DEN4IK2115//GitHub//kompile//symbol_file.txt", ios::out);

static char ch;
static bool ErrorOverFlow;
static int ErrInx = -1;
static int ErrI = 0;
static int LastInLine;
static unsigned symbol;
static unsigned lname;
static map <int, string> ErrorList;
static map <string, unsigned/*, string*/> KeyList = { //Словарь специальных слов языка программирования
    {"do", dosy},
    {"if", ifsy},
    {"in", insy},
    {"of", ofsy},
    {"or", orsy},
    {"to", tosy},
    {"and", andsy},
    {"div", divsy},
    {"end", endsy},
    {"for", forsy},
    {"mod", modsy},
    {"nil", nilsy},
    {"not", notsy},
    {"set", setsy},
    {"var", varsy},
    {"case", casesy},
    {"else", elsesy},
    {"file", filesy},
    {"goto", gotosy},
    {"only", onlysy},
    {"then", thensy},
    {"type", typesy},
    {"unit", unitsy},
    {"uses", usessy},
    {"with", withsy},
    {"array", arraysy},
    {"begin", beginsy},
    {"const", constsy},
    {"label", labelsy},
    {"until", untilsy},
    {"while", whilesy},
    {"downto", downtosy},
    {"expory", exportsy},
    {"import", importsy},
    {"module", modulesy},
    {"packed", packedsy},
    {"record", recordsy},
    {"repeat", repeatsy},
    {"vector", vectorsy},
    {"string", stringsy},
    {"forward", forwardsy},
    {"process", processsy},
    {"program", programsy},
    {"segment", segmentsy},
    {"function", functionsy},
    {"separate", separatesy},
    {"interface", interfacesy},
    {"procedure", proceduresy},
    {"qualified", qualifiedsy},
    {"implementation", implementationsy},
};

//static char line[100];
static string line;
static string name;

static int printed = -1;

void error(unsigned errorcode, textposition position) { //Функция заполнения массива ошибок в программе
    if (ErrI == ErrMax)
        ErrorOverFlow = true;
    else {
        ++ErrI;
        ErrList[ErrI].errorcode = errorcode;
        ErrList[ErrI].errorposition.linenumber = position.linenumber;
        ErrList[ErrI].errorposition.charnumber = position.charnumber;
    }
}

void ListErrors() { //Функция вывода всех ошибок для заданной строки
    while (positionnow.linenumber == ErrList[ErrInx].errorposition.linenumber) {
        if (ErrInx < 10)
            cout << "**0" << ErrInx << "**";
        else
            cout << "**" << ErrInx << "**";
        for(int i = 0; i < ErrList[ErrInx].errorposition.charnumber; i++)
            cout << " ";
        cout << "^" << "   Код Ошибки" << ErrList[ErrInx].errorcode << endl;
        cout << "*****" << ErrorList[static_cast<int>(ErrList[ErrInx].errorcode)] << endl;
        ErrInx++;
    }
}

void ListThisLine() { //Функция вывода строки кода программы
    if (positionnow.linenumber == printed) return;
    printed = positionnow.linenumber;
    cout << positionnow.linenumber << "      " << line << endl;
}

void ReadNextLine() { //Функция считывания следующей строки кода программы
    if (!Prog.eof()) {
        //fgets(line, 100, Prog);
        getline(Prog, line);
        LastInLine = static_cast<int>(line.length() - 1);
    }
}

char nextch() { //Функция считывания следующей лексемы кода программы
    //unsigned i;
    //char c;
    if (positionnow.charnumber == LastInLine) {
        ListThisLine();
        if (positionnow.linenumber == ErrList[ErrInx].errorposition.linenumber)
            ListErrors();
        if (!Prog.eof()) {
            ReadNextLine();
            positionnow.linenumber++;
            positionnow.charnumber = 0;
        }
    }
    else
        positionnow.charnumber++;
    ch_file << line[static_cast<unsigned>(positionnow.charnumber)];
    return line[static_cast<unsigned>(positionnow.charnumber)];
}

void number() { //Функция для обработки числа в коде программы
    int digit;
    int nbi = 0; //целое число, либо целая часть вещественного числа
    int nbf = 0; //вещественная часть числа если используются только цифры
    int nbfs = 0;
    while(ch >= '0' && ch <= '9') {
        digit = ch - '0';
        nbi = nbi * 10 + digit;
        ch = nextch();
    }
    if(ch == '.') {
        ch = nextch();
        while(ch >= '0' && ch <= '9') {
            digit = ch - '0';
            nbf = nbf * 10 + digit;
            ch = nextch();
        }
        if(ch == 'e') {
            ch = nextch();
            if(ch == '+' || ch == '-') {
                ch = nextch();
                while(ch>='0' && ch <= '9') {

                    digit = ch - '0';
                    nbfs = nbfs * 10 + digit;
                    ch = nextch();
                }
                symbol = floatc;
            }
            else {
                while(ch>='0' && ch <= '9') {
                    digit = ch - '0';
                    nbfs = nbfs * 10 + digit;
                    ch = nextch();
                }
                symbol = floatc;
            }
        }
        else
            symbol = floatc;
    }
    else {
        if(ch <= '0' || ch >= '9') {
            if(nbi >= -100 && nbi <= 100)
                symbol = intc;
            else
                error(203, token);
        }
        else {

        }
    }
}

void nextsym() { //Функция для формирования символов программы(слов)
    char firstch;
    char chtwo;
    if(ch != EOF) {
        while(ch == ' '/*" "[0]*/ || ch == '\t' || ch == '\n')
            ch = nextch();
        token.linenumber = positionnow.linenumber;
        //cout << token.linenumber;
        token.charnumber = positionnow.charnumber;
        firstch = ch;
        if(ch >= '0' && ch <= '9') ch = '0';
        if((ch >= 'a' && ch <= 'z') || (ch >='A' && ch <= 'Z')) ch = 'a';

        switch(ch) {
        case '0':
            ch = firstch;
            number();
            break;
        case 'a':
            name = "";
            lname = 0;
            ch = firstch;
            while(((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))&& lname < MAX_IDENT) {
                if(lname != 0) {
                    lname ++;
                    name += ch;
                    ch = nextch();
                }
                else {
                    name += ch;
                    ch = nextch();
                }
            }
            if (KeyList[name]/* != NULL*/)
                symbol = KeyList[name];
            else {
                symbol = ident;
            }
            break;
        case '\'':
            if((ch = nextch()) != '\'')
                if((ch = nextch()) != '\'')
                    error(76, token);
            ch = nextch();
            symbol = charc;
            break;
        case '\"':
            do {
                ch = nextch();
            } while(ch != '\"' && ch != EOF);
            if(ch == '\"') {
                ch = nextch();
                symbol = charc;
            }
            else {
                //error();
                symbol = charc;
            }
            break;
        case '*':
            ch = nextch();
            symbol = star;
            break;
        case '/':
            ch = nextch();
            //        if(ch == '/')
            //            do {
            //                ch = nextch();
            //            }while(ch != '\n');
            symbol = slash;
            break;
        case '=':
            ch = nextch();
            symbol = equal;
            break;
        case ',':
            ch = nextch();
            symbol = comma;
            break;
        case ';':
            ch = nextch();
            symbol = semicolon;
            break;
        case ':':
            ch = nextch();
            if(ch == '=') {
                symbol = assign;
                ch = nextch();
            }
            else
                symbol = colon;
            break;
        case '.':
            ch = nextch();
            if(ch == '.') {
                symbol = twopoints;
                ch = nextch();
            }
            else
                symbol = point;
            break;
        case '^':
            ch = nextch();
            symbol = arrow;
            break;
        case '(': // пропускаем коментарий (*Коментарий*)
            ch = nextch();
            if(ch == '*') {
                //symbol = lcomment;
                do {
                    chtwo = ch;
                    ch = nextch();
                }while(!(chtwo == '*' && ch == ')') && ch != EOF);
            }
            symbol = leftpar;
            break;
        case ')':
            ch = nextch();
            symbol = rightpar;
            break;
        case '[':
            ch = nextch();
            symbol = lbracket;
            break;
        case ']':
            ch = nextch();
            symbol = rbracket;
            break;
        case '{': // пропускаем коментарий {Коментарий}
            ch = nextch();
            while (ch != '}' && ch != EOF) {
                ch = nextch();
            }
            //symbol = flpar;
            break;
            //    case '}':
            //        ch = nextch();
            //        symbol = frpar;
            //        break;
        case '<':
            ch = nextch();
            if(ch == '=') {
                symbol = laterequal;
                ch = nextch();
            }
            else if(ch == '>') {
                symbol = latergreater;
                ch = nextch();
            }
            else
                symbol = later;
            break;
        case '>':
            ch = nextch();
            if(ch == '=') {
                symbol = greaterequal;
                ch = nextch();
            }
            else
                symbol = greater;
            break;
        case '+':
            ch = nextch();
            symbol = plus;
            break;
        case '-':
            ch = nextch();
            if(ch >= '0' && ch <= '9') {

            }
            else
                symbol = minus;
            break;
        default:
            error(6,token);
            break;
        }
    }


}

bool chislo(string s) {
    unsigned n = static_cast<unsigned>(s.length());
    for(unsigned i = 0; i < n; i++)
        if (s[i] < '0' || s[i] > '9')
            return false;
    return true;
}

bool ReadAnaliz() {
    unsigned errorcode;
    textposition position;
    ifstream ErrorAnaliz("//home//DEN4IK2115//workspace//vvod-vivod//ErrorAnaliz.txt");
    if(!ErrorAnaliz.is_open()) {
        cout << "Не удалось открыть файл анализатора" << endl;
        ErrorAnaliz.close();
        return false;
    }
    if(ErrorAnaliz.peek() == EOF) {
        cout << "Файл анализатора пуст" << endl;
        //return false;
    }
    try {
        string s;
        while (!ErrorAnaliz.eof()) {
            ErrorAnaliz >> s;
            if(!(position.linenumber = atoi(s.c_str())) || !chislo(s)) throw "Неверный формат номера строки" + s;
            ErrorAnaliz >> s;
            if(!(position.charnumber = atoi(s.c_str())) || !chislo(s)) throw "Неверный формат номера символа" + s;
            ErrorAnaliz >> s;
            if(!(errorcode = static_cast<unsigned>(atoi(s.c_str()))) || !chislo(s)) throw "Неверный формат кода ошибки" + s;
            error(errorcode, position);
        }
    }
    catch(string s) {
        cout << s << endl;
        ErrorAnaliz.close();
        return false;
    }
    ErrorAnaliz.close();
    return true;
}

bool ReadCode() {
    string s;
    int number;
    //int i = 0;
    ifstream ErrorCode("//home//DEN4IK2115//workspace//vvod-vivod//ErrorCode.txt");
    if(!ErrorCode) {
        cout << "Не удалось открыть файл с описанием ошибок" << endl;
        ErrorCode.close();
        return false;
    }
    if(ErrorCode.peek() == EOF) {
            cout << "Файл с описанием ошибок пуст" << endl;
            ErrorCode.close();
            return false;
    }
    getline(ErrorCode, s);
    try {
    while (!ErrorCode.eof()) {
            ErrorCode >> s;
            if(!chislo(s) || !(number = atoi(s.c_str()))) throw "Неверный код ошибки в файле описания ошибок";
            ErrorCode >> s;
            if(s != ":") throw "Неверный формат файла с описанием ошибок";
            getline(ErrorCode, s);
            ErrorList.insert(make_pair(number, s));
        }
    }
    catch(string s) {
        cout << s << endl;
        ErrorCode.close();
        return false;
    }
    ErrorCode.close();
    return true;
}

bool ReadProg() {
    if(!Prog) {
        cout << "Файл прграммы не удалось открыть" << endl;
        Prog.close();
        return false;
    }
    if(Prog.peek() == EOF) {
        cout << "Файл программы пуст" << endl;
        Prog.close();
        return false;
    }
    return true;
}

int main() {
    setlocale(LC_ALL, "rus");
    if(ReadCode() && ReadAnaliz() && ReadProg()) {
        positionnow.linenumber = 1;
        positionnow.charnumber = 0;
        ReadNextLine();
        cout<<line;

        ErrInx = 0;

        ch = line[0];
        do {
            nextsym();
            symbol_file << symbol << "_";
            //ch = nextch();
            //cout << ch;
        } while (!Prog.eof());
        cout << "DEN4IK2115";
        Prog.close();
        ch_file << "Hellow World";
        symbol_file.close();
        ch_file.close();
    }
    else {
        //cout << "" << endl;
    }
    //system{"pause";
    return 1;
}
