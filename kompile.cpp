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

struct textposition {
    int linenumber;
    int charnumber;
};
struct Err {
    struct textposition errorposition;
    unsigned errorcode;
};
struct Key {
    unsigned codekey;
    char namekey[9];
};

static textposition positionnow;
static textposition token;
static Err ErrList[ErrMax];
static Key keywords[51];

static ifstream Prog("//home//DEN4IK2115//workspace//vvod-vivod//Prog.txt");

static char ch;
static bool ErrorOverFlow;
static int ErrInx;
static int LastInLine;
static unsigned symbol;
static unsigned lname;
static map <int, string> ErrorList;
static map <string, unsigned/*, string*/> KeyList = {
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
static string line;
static string name;

static int printed = 0;

void error(unsigned errorcode, textposition position) {
    if (ErrInx == ErrMax)
        ErrorOverFlow = true;
    else {
        ++ErrInx;
        ErrList[ErrInx].errorcode = errorcode;
        ErrList[ErrInx].errorposition.linenumber = position.linenumber;
        ErrList[ErrInx].errorposition.charnumber = position.charnumber;
    }
}

void ListErrors() {
    while (positionnow.linenumber == ErrList[ErrInx].errorposition.linenumber) {
        if (ErrInx < 10)
            cout << "**0" << ErrInx << "**";
        else
            cout << "**" << ErrInx << "**";
        for(int i = 0; i < ErrList[ErrInx].errorposition.charnumber; i++)
            cout << " ";
        cout << "^" << "   Код Ошибки" << ErrList[ErrInx].errorcode << endl;
        cout << "*****" << ErrorList[ErrList[ErrInx].errorcode] << endl;
        ErrInx++;
    }
}

void ListThisLine() {
    if (positionnow.linenumber == printed) return;
    printed = positionnow.linenumber;
    cout << positionnow.linenumber << "      " << line << endl;
}

void ReadNextLine() {
    if (!Prog.eof()) {
        getline(Prog, line);
        LastInLine = line.length();
    }
}

char nextch() {
    if (positionnow.charnumber == LastInLine) {
        ListThisLine();
        if (positionnow.linenumber == ErrList[ErrInx].errorposition.linenumber)
            ListErrors();
        if (!Prog.eof()) {
            ReadNextLine();
            positionnow.linenumber++;
            positionnow.charnumber = 1;
        }
    }
    else
        positionnow.charnumber++;

    return line[positionnow.charnumber];
}

void number() {
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
            nbf = nbi * 10 + digit;
            ch = nextch();
        }
        if(ch == 'e') {
            ch = nextch();
            if(ch == '+' || ch == '-') {
                ch = nextch();
                while(ch>='0' && ch <= 9) {
                    ch = nextch();
                    digit = ch - '0';
                    nbfs = nbfs * 10 + digit;
                }
                symbol = floatc;
            }
            else {
                while(ch>='0' && ch <= 9) {
                    ch = nextch();
                    digit = ch - '0';
                    nbfs = nbfs * 10 + digit;
                }
                symbol = floatc;
            }
        }
        else
            symbol = floatc;
    }
    else {
        symbol = intc;
    }
}

//void testkey() {

//}

void nextsym() {
    char firstch;
    char chtwo;
    if(ch != EOF) {
        while(ch == ' ' || ch == '\t' || ch == '\n')
            ch = nextch();
        token.linenumber = positionnow.linenumber;
        token.charnumber = positionnow.charnumber;
        firstch = ch;
        if(ch >= '0' && ch <= '9') ch = 0;
        if((ch >= 'a' && ch <= 'z') || (ch >='A' && ch <= 'Z')) ch = 'a';
//        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
//            do {
//                ch = nextch();
//            }while((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch>= '0' && ch <= '9'));
//            //symbol = ?
//        }
//        else
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
                    name[lname] = ch;
                    ch = nextch();
                }
                else {
                    name[lname] = ch;
                    ch = nextch();
                }
            }
            if (KeyList[name] != NULL)
                symbol = KeyList[name];
            else {
                symbol = ident;
            }
            break;
        case '\'':
            if((ch = nextch()) != '\'')
                if((ch = nextch()) != '\'')
                    error();
//            do {
//                ch = nextch();
//            } while(ch != '\'');
            ch = nextch();
            symbol = charc;
            break;
        case '\"':
            do {
                ch = nextch();
            } while(ch != '\"');
            ch = nextch();
            symbol = charc;
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
            symbol = leftpar;
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
            symbol = minus;
            break;
        }
    }


}

bool chislo(string s) {
    int n = s.length();
    for(int i = 0; i < n; i++)
        if (s[i]<'0' || s[i]>'9')
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
            if(!(errorcode = atoi(s.c_str())) || !chislo(s)) throw "Неверный формат кода ошибки" + s;
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
    int i = 0;
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
        positionnow.linenumber = 0;
        positionnow.charnumber = 0;
        //ReadNextLine();
        //cout<<line;
        LastInLine = 0;
        ErrInx = 1;
        char ch_;
        do {
            ch_ = nextch();
        } while (!Prog.eof());
        cout << "DEN4IK2115";
        Prog.close();
    }
    else {
        //cout << "" << endl;
    }
    //system{"pause";
    return 0;
}
