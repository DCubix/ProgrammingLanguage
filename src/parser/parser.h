#ifndef LANG_PARSER_H
#define LANG_PARSER_H

#include <iostream>
#include <memory>
#include "../lexer/lexer.h"

#define log(x) std::cout << x << std::endl
#define error(x) std::cerr << x << std::endl

struct Node;
using NodePtr = std::unique_ptr<Node>;
using NodeList = std::vector<NodePtr>;
struct Node {
	virtual void visit() {}
	virtual void print(int pad = 0) { log("NaN"); }
};

struct Program : public Node {
	NodeList stmts;

	Program() = default;

	void print(int pad = 0) {
		for (auto&& node : stmts) node->print(pad);
	}
};

class LangParser {
public:
	LangParser() = default;
	~LangParser() = default;

	LangParser(const std::vector<Token>& tokens);

	bool accept(TokenType type, const std::string& param = "", bool regex = true, bool forceCheck = false);
	bool expect(TokenType type, const std::string& param = "", bool regex = true, bool forceCheck = false);

	bool next();
	void stepBack();

	Token& current() { return m_tokens[m_pos]; }
	Token& last();

	void parse();

private:
	std::vector<Token> m_tokens;
	int m_pos;

	std::unique_ptr<Node> m_ast;

	Node* atom();
	Node* power();
	Node* factor();

	// term: factor (('*'|'/'|'%') factor)*
	Node* term();
	Node* arithExpr();

	Node* shiftExpr();
	Node* andExpr();
	Node* xorExpr();
	Node* expr();

	// augassign (\+=|-=|\*=|\/=|%=|&=|\|=|^=|<<=|>>=|\*\*=)
	// comp (<=|>=|!=|==|is|has|>|<)
	Node* comparison();
	Node* notTest();
	Node* andTest();
	Node* orTest();
	Node* test();

	std::vector<Node*> argList();

	Node* stmt();
	Node* ifStmt();
	std::vector<Node*> stmtList();

	Node* letStmt();

	Node* param(bool checkAssign = true);
	std::vector<Node*> paramList(bool checkAssign = true);

	Node* funcDef();

	Node* forStmt();
	Node* whileStmt();

};

#endif // LANG_PARSER_H