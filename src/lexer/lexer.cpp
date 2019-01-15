#include "lexer.h"

#include <iostream>
#include <cctype>
#include <regex>

Scanner::Scanner(const std::string& input)
	: m_input(input), m_pos(0)
{
	std::cout << "INPUT:\n" << input << std::endl;
}

char Scanner::next() {
	if (m_pos >= m_input.size()) return '\0';
	return m_input[m_pos++];
}

char Scanner::peek() const {
	if (m_pos+1 >= m_input.size()) return '\0';
	return m_input[m_pos + 1];
}

char Scanner::prev() const {
	if (m_pos - 1 < 0) return '\0';
	return m_input[m_pos - 1];
}

void Scanner::advance(int n) {
	if (m_pos+n >= m_input.size()) return;
	m_pos += n;
}

LangLexer::LangLexer(const std::string& input)
	: m_scanner(Scanner(input))
{}

void LangLexer::tokenize() {
	m_tokens.clear();

	const std::regex symbolRE("[-!$%^&*()_+|~=`{}\\[\\]:<>?,.\\/\\\\]");

#define C m_scanner.current()
#define P m_scanner.prev()
#define N m_scanner.next()
#define S std::string(1, C)

	int line = 0;
	int pos = 0;

	while (m_scanner.hasNext()) {
		if (std::isalpha(C) || C == '_') {
			std::string res = "";
			while ((std::isalnum(C) || C == '_') && m_scanner.hasNext()) {
				res += C;
				m_scanner.next();
			}
			Token tok{};
			tok.lexeme = res;
			tok.type = res == "has" || res == "is" ? TokenType::OTHER : TokenType::ID;
			tok.line = line; tok.pos = (pos += res.size());
			m_tokens.push_back(tok);
		} else if (std::isdigit(C) || C == '.') {
			const std::regex re("[0-9a-fA-F.eExX+-]");
			std::string res = "";
			while (std::regex_match(S, re) && m_scanner.hasNext()) {
				res += C;
				m_scanner.next();
			}
			Token tok{};
			tok.lexeme = res;
			tok.type = TokenType::NUMBER;
			tok.numberValue = std::stod(res);
			tok.line = line; tok.pos = (pos += res.size());
			m_tokens.push_back(tok);
		} else if (C == '"' || C == '\'') {
			bool isChar = C == '\'';
			std::string res = "";
			m_scanner.next();
			while (C != '"' && C != '\'' && m_scanner.hasNext()) {
				if (C == '\\') {
					m_scanner.next();
					switch (C) {
						case 'b': res += '\b'; break;
						case 'n': res += '\n'; break;
						case 't': res += '\t'; break;
						case 'f': res += '\f'; break;
						case 'r': res += '\r'; break;
						case '"': res += '"'; break;
						case '\'': res += '\''; break;
						default: break;
					}
					m_scanner.next();
				} else {
					res += C;
					m_scanner.next();
				}
			}
			Token tok{};
			tok.lexeme = res;
			tok.type = isChar ? TokenType::CHAR : TokenType::STRING;
			if (isChar) {
				tok.charValue = res[0];
			} else {
				tok.stringValue = res;
			}
			tok.line = line; tok.pos = (pos += res.size() + 2);
			m_tokens.push_back(tok);
			m_scanner.next();
		} else if (std::isspace(C)) {
			if (C == '\n') {
				line++;
				pos = 0;
			} else pos++;
			m_scanner.next();
		} else if (C == ';') {
			Token tok{};
			tok.lexeme = ";";
			tok.type = TokenType::SEMI;
			tok.line = line; tok.pos = pos;
			m_tokens.push_back(tok);
			m_scanner.next();
			pos++;
		} else if (C == '(' || C == ')' || C == '[' || C == ']' || C == '{' || C == '}') {
			Token tok{};
			tok.lexeme = S;
			tok.type = TokenType::OTHER;
			tok.line = line; tok.pos = pos;
			m_tokens.push_back(tok);
			m_scanner.next();
			pos++;
		} else if (std::regex_match(S, symbolRE)) {
			std::string res = "";
			while (std::regex_match(S, symbolRE) && m_scanner.hasNext()) {
				res += C;
				m_scanner.next();
			}
			Token tok{};
			tok.lexeme = res;
			tok.type = TokenType::OTHER;
			tok.line = line; tok.pos = (pos += res.size());
			m_tokens.push_back(tok);
		} else {
			m_scanner.next();
			pos++;
		}
	}

	Token tok{};
	tok.type = TokenType::END;
	tok.line = line+1; tok.pos = 0;
	m_tokens.push_back(tok);
}

void LangLexer::printTokens() {
	for (auto&& tok : m_tokens) {
		std::cout << tok.toString() << " ";
	}
	std::cout << std::endl;
}
