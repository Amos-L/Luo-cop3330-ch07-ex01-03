/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Amos Luo
 */

/*
	calculator08buggy.cpp | From: Programming -- Principles and Practice Using C++, by Bjarne Stroustrup
	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

#include "std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) : kind(ch), name(n) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char sr = 's';
const char power = 'p';
const char CheckConst = 'c';
const char e = 'e';
const char pi = 'pi';

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch) || ch == '_' ) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			cin.unget();
			if (s == "let") return Token(let);
			if (s == "quit") return Token(name);
			if (s == "pow") return Token(power);
			if (s == "const") return Token(CheckConst);
			if (s == "sqrt") return Token(sr);
			if (s == "e") return Token(e);
			if (s == "pi") return Token(pi);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	bool CheckConst;
	Variable(string n, double v, bool c) :name(n), value(v), CheckConst(c) { }
};

vector<Variable> names;

double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s && names[i].CheckConst == false) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	case pi:
		return 3.141;
	case e:
		return 2.718;
	case sr:
	{
		t = ts.get();
		double x = expression();
		if (x < 0) {
			error("Number is negative, can not be squared");
		}
		return sqrt(x);
	}
	case power: 
	{
		t = ts.get();
		double x = expression();
		t = ts.get();
		double y = expression();
		return pow(x, y);
	}
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	bool constantCheck;
	if (t.kind == 'c') {
		constantCheck = true;
		t = ts.get();
	}
	else {
		constantCheck = false;
	}
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d, constantCheck));
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
