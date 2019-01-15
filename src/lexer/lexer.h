#ifndef LANG_LEXER_H
#define LANG_LEXER_H

#include "tokens.h"

#include <vector>

class Scanner {
public:
	Scanner() = default;
	~Scanner() = default;

	Scanner(const std::string& input);

	char next();
	char current() const { return m_input[m_pos]; }
	char peek() const;
	char prev() const;
	bool hasNext() const { return m_pos < m_input.size(); }

private:
	std::string m_input;
	int m_pos;

	void advance(int n = 1);
};

class LangLexer {
public:
	LangLexer() = default;
	~LangLexer() = default;

	LangLexer(const std::string& input);
	void tokenize();

	std::vector<Token> tokens() const { return m_tokens; }

	void printTokens();

private:
	std::vector<Token> m_tokens;
	Scanner m_scanner;
};

#endif // LANG_LEXER_H