#ifndef LANG_TOKENS_H
#define LANG_TOKENS_H

#include <string>
#include <sstream>

enum TokenType {
	END = 0,
	OTHER,
	ID,
	STRING,
	CHAR,
	NUMBER,
	SEMI
};

struct Token {
	TokenType type;
	std::string lexeme;
	int line, pos;

	double numberValue;
	std::string stringValue;
	char charValue;

	std::string toString() {
		std::stringstream ret;
		switch (type) {
			case END: ret << "END"; break;
			case ID: ret << "ID(" << lexeme << ")"; break;
			case NUMBER: ret << "NUM(" << numberValue << ")"; break;
			case CHAR: ret << "CHR('" << charValue << "')"; break;
			case STRING: ret << "STR(\"" << stringValue << "\")"; break;
			case SEMI: ret << ";"; break;
			case OTHER: ret << lexeme; break;
		}
		return ret.str();
	}
};

#endif // LANG_TOKENS_H