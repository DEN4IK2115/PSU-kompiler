#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <list>

using namespace std;

const int ErrMax = 80;

struct textposition { //для позиции символа в тексте программы
    int linenumber;
    int charnumber;
};

struct Err { //структура для еррор анализа
    struct textposition errorposition;
    unsigned errorcode;
};

//static list <E> ErrList;
static Err ErrList[ErrMax];
static textposition positionnow;

static ifstream Prog("//home//DEN4IK2115//workspace//vvod-vivod//Prog.txt");

static int ErrInx;
static bool ErrorOverFlow;
static string line;
static int LastInLine;
static string ErrorList[350];

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

void ListErrors() { //вывод номера ошибки в списке ошибок программы
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

void nextch(char &ch) {
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
    ch = line[positionnow.charnumber];
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
            if(!chislo(s)) throw "Неверный код ошибки в файле описания ошибок";
            ErrorCode >> s;
            if(s != ":") throw "Неверный формат файла с описанием ошибок";
            getline(ErrorCode, s);
            ErrorList[i] = s;
            i++;
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
        char ch;
        do {
            nextch(ch);
        } while (!Prog.eof());
        /*do {
            nextch(ch);
        } while (positionnow.charnumber != LastInLine);*/
        cout << "DEN4IK2115";
        Prog.close();
    }
    else {
        //cout << "" << endl;
    }
    //system{"pause";
    return 0;
}
