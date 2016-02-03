#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <typeinfo>



using namespace std;
template <typename T>
class Vect
{
public:
	T x, y;

	Vect(T x, T y) : x(x), y(y) {};


	Vect operator+(const Vect &rhs) { //add
		return Vect(x + rhs.x, y + rhs.y);
	}

	Vect operator-(const Vect &rhs) { //sub
		return Vect(x - rhs.x, y - rhs.y);
	}

	T operator*(const Vect &rhs) { // dot prod
		return T(x*rhs.x + y*rhs.y);
	}

	Vect operator*(const T &val) { //scal mult
		return Vect(val * x, val * y);
	}

	Vect operator/(const T &val) { // scal div
		return Vect(x / val, y / val);
	}

	T operator%(const Vect &rhs) { //cross
		return T(x*rhs.y - y*rhs.x);
	}

	Vect* operator()() {
		return this;
	}

};

template <typename T>
T operator*(const T &val, const Vect<T> &rhs) {
	return Vect(val * rhs.x, val * rhs.y);
}

template <typename T>
struct Eval {
	bool isScal, isVec;
	Vect<T> vec;
	T scal;
	Eval(T scal, Vect<T> vec, bool isScal, bool isVec) : scal(scal), vec(vec), isScal(isScal), isVec(isVec) {};
};


vector<string> getTokens(const string &input) {
	vector<string> tokens;
	string comps;
	stringstream stream(input);
	while (getline(stream, comps, ' ')) {
		tokens.push_back(comps);
	}
	return tokens;
};


bool isOperator(const char &c) {
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
		return true;
	else
		return false;
};

bool isOpenPar(const char &c) {
	return c == '(';
};

bool isClosePar(const char &c) {
	return c == ')';
};

// helper functions from : https://gist.github.com/mycodeschool/7867739
int getOpWeight(const char &c) {
	int weight = -1;
	switch (c) {
	case '+':
	case '-':
		weight = 1;
	case '*':
	case '/':
	case '%':
		weight = 2;
	}
	return weight;
};


bool IsOperand(char C) {
	if (C >= '0' && C <= '9') return true;
	if (C >= 'a' && C <= 'z') return true;
	if (C >= 'A' && C <= 'Z') return true;
	return false;
};

bool IsVector(const char &c) {
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	return false;
}


bool IsScalar(const char &c) {
	if (c >= '0' && c <= '9') return true;
	return false;
}

bool HasHigherPrec(const char &top, const char &inc) {
	int topW = getOpWeight(top);
	int incW = getOpWeight(inc);
	if (topW == incW) {
		return true;
	}
	return topW > incW ? true : false;
};

template <typename T>
Eval<T> EvalMe(Eval<T> o1, Eval<T> o2, const char &c) {
	switch (c) {
	case'-':
		if ((o1.isScal && o2.isVec) || (o1.isVec && o2.isScal))
			return Eval<T>(-1, o1.vec, true, true);
		else if (o1.isScal)
			return Eval<T>(o1.scal - o2.scal, o1.vec, true, false);
		else
			return Eval<T>(-1, o1.vec - o2.vec, false, true);
		break;
	case'+':
		if ((o1.isScal && o2.isVec) || (o1.isVec && o2.isScal))
			return Eval<T>(-1, o1.vec, true, true);
		else if (o1.isScal)
			return Eval<T>(o1.scal + o2.scal, o1.vec, true, false);
		else
			return Eval<T>(-1, o1.vec + o2.vec, false, true);
		break;
	case'/':
		if (o2.isVec)
			return Eval<T>(-1, o1.vec, true, true);
		else if (o1.isScal)
			return Eval<T>(o1.scal / o2.scal, o1.vec, true, false);
		else
			return Eval<T>(-1, o1.vec / o2.scal, false, true);
		break;
	case'*':
		if (o1.isScal && o2.isScal)
			return Eval<T>(o1.scal * o2.scal, o1.vec, true, false);
		else if (o1.isScal)
			return Eval<T>(-1, o2.vec * o1.scal, false, true);
		else if (o2.isScal)
			return Eval<T>(-1, o1.vec * o2.scal, false, true);
		else
			return Eval<T>(o1.vec * o2.vec, o1.vec, true, false);
		break;
	case'%':
		if (o1.isScal || o2.isScal)
			return Eval<T>(-1, o1.vec, true, true);
		else
			return Eval<T>(o1.vec % o2.vec, o1.vec, true, false);
		break;

	}
	return Eval<T>(-1, o1.vec, true, true);
}

int main() {
	string inputs = "";
	unordered_map < string, Vect<int>> vects;
	vects.clear();

	//get vectors from input
	while (true) {
		getline(cin, inputs);
		if (inputs.find("=") == string::npos) {
			vector<string> tokens = getTokens(inputs);
			int x = stoi(tokens[1].c_str());
			int y = stoi(tokens[2].c_str());
			Vect<int> temp(x, y);
			vects.insert(make_pair(tokens[0], temp));
		}
		else {
			break;
		}
	}
	while (true) {
		getline(cin, inputs);
		string output = "";
		if (inputs.find("=") == string::npos) {
			stack<char> tokens;
			string postfix = "";
			//create postfix string
			bool addmult = false;
			bool addmult_end = false;
			for (char c : inputs) {
				if (c == ' ') {
					continue;
				}
				if (isOperator(c)) {
					while (!tokens.empty() && !isOpenPar(c) && HasHigherPrec(tokens.top(), c)) {
						postfix += tokens.top();
						tokens.pop();
					}
					tokens.push(c);
					addmult_end = false;
					addmult = false;
				}
				else if (IsOperand(c)) {
					if (addmult) {
						tokens.push('*');
					}
					if (addmult_end) {
						tokens.push('*');
					}
					addmult = true;
					addmult_end = false;
					postfix += c;
				}
				else if (isOpenPar(c)) {
					if (addmult) {
						tokens.push('*');
					}
					tokens.push(c);
					addmult = false;
					addmult_end = false;
				}
				else if (isClosePar(c)) {
					while (!tokens.empty() && !isOpenPar(tokens.top())) {
						postfix += tokens.top();
						tokens.pop();
					}
					tokens.pop();
					addmult = false;
					addmult_end = true;
				}
			}
			while (!tokens.empty()) {
				postfix += tokens.top();
				tokens.pop();
			}
			//evaluate postfix
			stack<Eval<int>> operands;
			output = "";
			for (char c : postfix) {
				string finding = "";
				finding += c;
				if (IsVector(c)) {
					Eval<int> temp(-1, vects.find(finding)->second, false, true);
					operands.push(temp);
				}
				else if (IsScalar(c)) {
					int temp_val = c - '0';
					Eval<int> temp(temp_val, vects.begin()->second, true, false);
					operands.push(temp);
				}
				else if (isOperator(c)) {
					if (operands.size() < 2) {
						output = "INVALID";
						break;
					}
					Eval<int> opVal1 = operands.top();
					operands.pop();
					Eval<int> opVal2 = operands.top();
					operands.pop();
					Eval<int> resVal = EvalMe(opVal2, opVal1, c);
					if (resVal.isScal && resVal.isVec) {
						output = "INVALID";
						break;
					}
					else {
						operands.push(resVal);
					}
				}

			}
			if (output.size() == 0) {
				stringstream ss;
				if (operands.size() > 1)
					output = "INVALID";
				else {
					if (operands.top().isScal) {
						ss << to_string(operands.top().scal);
						output = ss.str();
					}

					else {
						ss << to_string(operands.top().vec.x) + " " + to_string(operands.top().vec.y);
						output = ss.str();
					}

				}
			}
			cout << output + "\n";


		}
		else {
			break;
		}
	}

	system("PAUSE");
	return 0;
}