#include <iostream>
#include <fstream>
#include <regex>
#include <cctype>
#include <string>
#include <map>
#include <stdio.h>
#include <vector>
#include "tinyexpr.h"



using namespace std;


void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

bool isNumber(const std::string& str) {
    try {
        int num = std::stoi(str);
        return true;
    } catch(std::invalid_argument& e) {
        return false;
    }
}

string cut_line(string line, unsigned long length){
    if (line != "") line = line.substr(length);
    while(line[0] == ' ') line = line.substr(1);
    return line;
}

int int_expression(string str){
    str.pop_back();
    const char * c = str.c_str();
    double answer = te_interp(c, 0);
    int ans = (int)round(answer);
    return answer;
}

int check_name_length(string line){
    int count = 0;
    while(line[count] != ';' and line[count] != ' '){
        count += 1;
    }
    return count;
}

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '!');
}

bool isValidExpression(const string& expression) {
    stack<char> brackets;
    
    for (char c : expression) {
        if (isOperator(c) || isalnum(c) || c == '<' || c == '>' || c == '|' || c == '!' || c == '=' || c == '&' ) {
            continue;
        } else if (c == '(') {
            brackets.push('(');
        } else if (c == ')') {
            if (brackets.empty() || brackets.top() != '(') {
                return false;
            } else {
                brackets.pop();
            }
        } else {
            return false;
        }
    }
    
    return brackets.empty();
}

bool lexer(){
    //Зарезервированные слова языка: char, int, float, void, if, else, while, for, return.
    map<string, string> reserved = {
        {"int", "T_int"},
        {"char", "T_char"},
        {"float", "T_float"},
        {"void", "T_void"},
        {"if", "T_if"},
        {"else", "T_else"},
        {"while", "T_while"},
        {"for", "T_for"},
        {"return", "T_return"},
        {"printf", "T_printf"}
    };
    
    map<string, string> indents_allowers = {
        {"int", "T_int"},
        {"char", "T_char"},
        {"float", "T_float"},
        {"void", "T_void"},
        
    };

    map<string, string> symbols = {
        {"=", "T_assign"},
        {"+", "T_add"},
        {"-", "T_subtract"},
        {"*", "T_multiply"},
        {"/", "T_divide"},
        {"%", "T_remainder"},
        {"!", "T_factorial"},
        {"==", "T_equal"},
        {"!=", "T_not_equal"},
        {">=", "T_greater_equal"},
        {">", "T_greater"},
        {"<=", "T_less_equal"},
        {"<", "T_less"},
        {"&&", "T_and"},
        {"||", "T_or"},
        {"(", "T_open_parenthesis"},
        {")", "T_close_parenthesis"},
        {"{", "T_open_brace"},
        {"}", "T_close_brace"},
        {"[", "T_open_square_bracket"},
        {"]", "T_close_square_bracket"},
        {";", "T_semicolon"},
        {",", "T_comma"}
    };
    
    string line;
    string delimeter = " ";
    bool indentcreation = 0;             //возможность инициализации переменной
    int indentcreation_expire = 0;
    string indentslist[10];              //список существующих переменных
    int numberofindents = 0;
    bool functionbody = 0;
    
    string commentstopper = "\*/";
    string input;
    ifstream file ("/Users/vsevolod/Documents/RPOP/RPOP/input2.txt");
    int linecounter = 0;
    while(getline(file, line)){//берем по строчке
        int pos = 0;
        linecounter += 1;
//        indentcreation = 0;
        
        while(line != ""){

            string token = line.substr(0, line.find(delimeter));//берем новый токен
            
            if(token == "/*"){                                       //comments noindent influence
                while(token != commentstopper){
                    token = line.substr(0, line.find(delimeter));
                    pos += token.length() + 1;
                    line = line.substr(token.length());
                    while(line[0] == ' ') line = line.substr(1);
                }
            }
            
            else if(reserved[token] != ""){                          //reserved
                cout << reserved[token] << ", -" << endl;
                if(indents_allowers[token] != "") indentcreation = 1; //отслеживание предпологаемого обозначения новой переменной
            }
            
            else if(symbols[token] != ""){                           //symbols
                cout << symbols[token] << ", -" << endl;
                if (symbols[token] == "T_open_brace") functionbody = 1; //отслеживание нахождения в теле функции
                if (symbols[token] == "T_close_brace") functionbody = 0;
            }
            
            else if(token[0] == '\"'){
                cout << "T_stringcon, " << token;
                cout << endl;
            }
            
            else if(token != ""){
                if(find(begin(indentslist), end(indentslist), token) != end(indentslist) /* and functionbody == 1*/){  //если такая переменная уже cуществует вывести
                    cout << "T_indent, " << token << endl;
                    
                }else if(indentcreation == 1){                                              //если переменную можно создать + создание
                    indentslist[numberofindents] = token;
                    numberofindents += 1;
                    cout << "T_indent, " << token << endl;
                    //indentcreation = 0; //завершение создание переменной
                    
                }
                else if(isNumber(token)){                                                                 //если это число
                    cout << "T_indent, " << token << endl;
                    
                }else{
                    cout << endl << "Error on " << linecounter << ":" << pos + 1 << endl;
                    cout << "Problematic token is: " << token << endl;
                    return true;
                }
                
            }
            
            if(indentcreation == 1 and indentcreation_expire == 1){indentcreation = 0; indentcreation_expire = 0;}
            else if(indentcreation == 1 and indentcreation_expire == 0){indentcreation_expire = 1;}
            pos += token.length() + 1;
            if (line != "") line = line.substr(token.length());
            while(line[0] == ' ') line = line.substr(1);
        }
    }
    if(functionbody == 1){ cout << " SECOND } NOT FOUND" << endl; return false;}
    return true;

}


bool task2(){
    map<string, string> int_operations = {
        {"=", "bin"},
        {"+", "bin"},
        {"-", "bin"},
        {"*", "bin"},
        {"/", "bin"},
        {"%", "bin"},
        {"!", "un"}
    };
    map<string, string> char_operations = {
        {"+", "bin"},
        {"=", "bin"}
    };
    map<string, string> same_operations = {
        {"==", "bin"},
        {"!=", "bin"},
        {">=", "bin"},
        {">", "bin"},
        {"<=", "bin"},
        {"<", "bin"}
    };
    map<string, string> bool_operations = {
        {"&&", "bin"},
        {"||", "bin"}
    };
    vector<string> operators = {"=", "+", "-", "*", "/", "%", "!", "==", "!=", ">=", ">", "<=", "<", "&&", "||"};
    // =, +, -, *, /, %, !
    //string  =, +,
    //int =, +, -, *, /, %, !
    //same type ==, !=, >=, >, <=, <,    result is bool
    //bool &&, ||          result is int
    ifstream file ("/Users/vsevolod/Documents/RPOP_3/RPOP_3/input1.txt");
    string line;
    cout << "tested line is: " << line << endl;
    string variables[5][3]; //максимум 5 переменных, содержит тип, имя, значение
    int current_pos_var = 0;
    while(getline(file, line)){//берем по строчке
        
    
        while(line != ""){
            if(line[line.length()-1] != ';'){
                cout << "Bad line syntax" << endl;
                return false;
            }
            string token = line.substr(0, line.find(" "));
            if(token == "int"){
                line = cut_line(line, token.length());//убираем тип
                string name = line.substr(0, check_name_length(line));
                line = cut_line(line, name.length());//убираем имя
                int int_value;
                if(line[0] == '='){
                    variables[current_pos_var][0] = "int";
                    variables[current_pos_var][1] = name;
                    line = cut_line(line, 2);//убираем равенство
                    int_value = int_expression(line);
                    variables[current_pos_var][2] = to_string(int_value);
                }else if(line[0] == ';'){
                    variables[current_pos_var][0] = "int";
                    variables[current_pos_var][1] = name;
                    variables[current_pos_var][2] = "0";
                }else{
                    cout << "Bad int syntax" << endl;
                    return false;
                }
                cout << "Found int token [" << variables[current_pos_var][0] << " " << variables[current_pos_var][1] << " " << variables[current_pos_var][2] << "]" << endl;
                
                for(int i = 0; i < current_pos_var; i++){
                    if(variables[i][1] == variables[current_pos_var][1]){
                        cout << "var rename error" << endl;
                        return false;
                    }
                }
                
                current_pos_var += 1;
            }
            else if (token == "char"){
                line = cut_line(line, token.length());//убираем тип
                string name = line.substr(0,  check_name_length(line));
                line = cut_line(line, name.length());//убираем имя
                string char_value;
                if(line[0] == '='){
                    variables[current_pos_var][0] = "char";
                    variables[current_pos_var][1] = name;
                    line = cut_line(line, 2);//убираем равенство
                    char_value = line[1];
                    if(line[0] != '\'' or line[2] != '\''){cout << "Bad char syntax" << endl; return false;}
                    variables[current_pos_var][2] = char_value;
                }else if(line[0] == ';'){
                    variables[current_pos_var][0] = "char";
                    variables[current_pos_var][1] = name;
                    variables[current_pos_var][2] = "None";
                }else{
                    cout << "Bad char syntax" << endl;
                    return false;
                }
                cout << "Found char token [" << variables[current_pos_var][0] << " " << variables[current_pos_var][1] << " " << variables[current_pos_var][2] << "]" << endl;
                
                for(int i = 0; i < current_pos_var; i++){
                    if(variables[i][1] == variables[current_pos_var][1]){
                        cout << "var rename error" << endl;
                        return false;
                    }
                }
                current_pos_var += 1;
                
            }
            else if (token == "bool"){
                line = cut_line(line, token.length());//убираем тип
                string name = line.substr(0,  check_name_length(line));
                line = cut_line(line, name.length());//убираем имя
                string bool_value;
                if(line[0] == '='){
                    variables[current_pos_var][0] = "bool";
                    variables[current_pos_var][1] = name;
                    line = cut_line(line, 2);//убираем равенство
                    line.pop_back();
                    if(line == "true"){
                        variables[current_pos_var][2] = "true";
                    }else if(line == "false"){
                        variables[current_pos_var][2] = "false";
                    }else{
                        cout << "Bad bool syntax" << endl;
                        return false;
                    }
                }else if(line[0] == ';'){
                    variables[current_pos_var][0] = "bool";
                    variables[current_pos_var][1] = name;
                    variables[current_pos_var][2] = "false";
                }else{
                    cout << "Bad char syntax" << endl;
                    return false;
                }
                cout << "Found bool token [" << variables[current_pos_var][0] << " " << variables[current_pos_var][1] << " " << variables[current_pos_var][2] << "]" << endl;
                
                
                for(int i = 0; i < current_pos_var; i++){
                    if(variables[i][1] == variables[current_pos_var][1]){
                        cout << "var rename error" << endl;
                        return false;
                    }
                }
                current_pos_var += 1;
                
            }
            else{
                if(token.find_first_not_of("abcdefghijklmnopqrstuvwxyz") != string::npos){//проверка на то что это ток буквы
                    cout << "Bad syntax (isalpha)" << endl;
                    return false;
                }
                bool is_variable = false;
                for(int i = 0; i < 5; i++){
                    if(variables[i][1] == token) is_variable = true;
                }
                if(is_variable == false){
                    cout << "Bad variable syntax" << endl;
                    return 0;
                }
                //тут должен быть чекер того что выражение синтаксически верное
                string line1 = line;
                line1 = line1.substr(line1.find("= ") + 2, line1.length());
                line1.erase(remove(line1.begin(), line1.end(), ' '), line1.end());
                line1.pop_back();
                if (isValidExpression(line1)) {
                    cout << "Expr step syntax passed" << endl;
                } else {
                    cout << "Bad expression syntax" << endl;
                    return false;
                }
                
                //тут должен быть чекер что все переменные в выражении одного типа
                string line2 = line;
                line2.pop_back();
                //проверка первой переменной для полседующего сравнения
                string first_var_type;
                token = line2.substr(0, line2.find(" "));
                if(token.find_first_not_of("+=*/%-<!|&") != string::npos){//если это не операции
                    for(int i = 0; i < current_pos_var; i++){
                        if(variables[i][1] == token){
                            first_var_type = variables[i][0];
                        }
                    }
                    
                }else{
                    cout << "Bad first var name" << endl;
                    return false;
                }
                line2 = cut_line(line2, token.length());
                
                while(line2 != ""){
                    token = line2.substr(0, line2.find(" "));
                    
                    if(token.find_first_not_of("+=*/%-<!|&") != string::npos){//если это не операции
                        for(int i = 0; i < current_pos_var; i++){
                            if(variables[i][1] == token){
                                if(variables[i][0] != first_var_type){//типы не совпадают
                                    cout << "Expression with multiple types!" << endl;
                                    return false;
                                }
                            }
                        }
                    }
                    
                    line2 = cut_line(line2, token.length());
                }
                cout << "Expr step type passed" << endl;
                
                
                //тут должен быть чекер что все операции корректны по синтаксису + совместимы с типом первой переменной
                string line3 = line;
                line3.pop_back();
                while(line3 != ""){
                    token = line3.substr(0, line3.find(" "));
                    if(token.find_first_not_of("abcdefghijklmnopqrstuvwxyz") != string::npos){//проверка на то что это ток буквы
                        //сначала проверяем что операции корректны по синтаксису
                        bool is_legit_operation = false;
                        for(const string& op : operators) {
                            if(token == op) is_legit_operation = true;
                        }
                        if(is_legit_operation == false){
                            cout << "Bad operation syntax: " << token << endl;
                            return false;
                        }
                        
                        //проверяем что операции принадлежат к тому типу
                        switch (first_var_type[0]) {
                            case 'i':
                                if(int_operations[token] == "" and same_operations[token] == ""){
                                    cout << "Bad integer operation: " << token << endl;
                                    return false;
                                }
                                break;
                            case 'c':
                                if(char_operations[token] == "" and same_operations[token] == ""){
                                    cout << "Bad char operation: " << token << endl;
                                    return false;
                                }
                                break;
                            case 'b':
                                if(bool_operations[token] == "" and same_operations[token] == ""){
                                    cout << "Bad bool operation: " << token << endl;
                                    return false;
                                }
                                break;
                                
                            default:
                                cout << "Bad type?" << endl;
                                return false;
                                break;
                        }
                    }
                    line3 = cut_line(line3, token.length());
                }
                cout << "Expr step operations passed" << endl;
                
                //если прошлые окей то выдаем соответствующий первому тип результата не считая(юпии)
                cout << "Expression is fully ok, expected result type is: " << first_var_type << endl;
            }
            
            if(current_pos_var > 5){cout << "Too many vars" << endl; return false;}
            line = "";
            }
    }
    return true;
}


int main(){
    ofstream output1;
    output1.open("/Users/vsevolod/Documents/RPOP_3/RPOP_3/output.txt");
    output1.close();
    int num;
    num = 2;
    if (num == 2){
        if(task2()) cout << "Task completed succesfully!" << endl;
        else cout << "Task ended with an error, try again" << endl;
    }else lexer();
    return 0;
}
