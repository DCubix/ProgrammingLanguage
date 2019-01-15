#include "parser.h"

#include <regex>

#include "detail/atom.hpp"
#include "detail/ops.hpp"
#include "detail/stmts.hpp"

LangParser::LangParser(const std::vector<Token>& tokens)
	: m_tokens(tokens), m_pos(0)
{}

bool LangParser::accept(TokenType type, const std::string& param, bool regex, bool forceCheck) {
	if (m_pos >= m_tokens.size()) return false;
	if (current().type == type) {
		if (type == TokenType::OTHER || forceCheck) {
			bool test = regex ? std::regex_match(current().lexeme, std::regex(param)) : current().lexeme == param;
			if (test) return next();
			else return false;
		}
		return next();
	}
	return false;
}

bool LangParser::expect(TokenType type, const std::string& param, bool regex, bool forceCheck) {
	if (accept(type, param, regex, forceCheck)) {
		return true;
	}

	std::string expc = "Unknown";
	switch (type) {
		case TokenType::ID: expc = "Identifier"; break;
		case TokenType::NUMBER: expc = "Number"; break;
		case TokenType::STRING: expc = "String"; break;
		case TokenType::CHAR: expc = "Character"; break;
		case TokenType::OTHER: expc = "Symbol"; break;
		case TokenType::SEMI: expc = "Semicolon"; break;
		case TokenType::END: expc = "EOF"; break;
		default: break;
	}

	error(
		"ERROR(" <<
		current().line << 
		":" <<
		current().pos <<
		"): Unexpected symbol \"" <<
		current().lexeme <<
		"\". Expected \"" <<
		expc <<
		"\"."
	);
	return false;
}

bool LangParser::next() {
	if (m_pos >= m_tokens.size()) return false;
	m_pos++;
	return true;
}

void LangParser::stepBack() {
	if (m_pos - 1 < 0) return;
	m_pos--;
}

Token& LangParser::last() {
	if (m_pos - 1 < 0) return m_tokens[m_pos];
	return m_tokens[m_pos - 1];
}

void LangParser::parse() {
	std::vector<Node*> stmts;
	while (m_pos < m_tokens.size()) {
		stmts.push_back(stmt());
	}

	std::unique_ptr<Program> prog(new Program());
	for (Node* n : stmts) {
		if (n == nullptr) continue;
		prog->stmts.push_back(NodePtr(n));
	}
	prog->print();
}

Node* LangParser::atom() {
	if (accept(TokenType::NUMBER)) {
		return new NumberAtom(last().numberValue);
	} else if (accept(TokenType::STRING)) {
		return new StringAtom(last().stringValue);
	} else if (accept(TokenType::CHAR)) {
		return new CharAtom(last().charValue);
	} else if (accept(TokenType::ID)) {
		return new IdentifierAtom(last().lexeme);
	} else if (accept(TokenType::OTHER, "(", false)) {
		Node* res = test();
		expect(TokenType::OTHER, ")", false);
		return res;
	} else if (accept(TokenType::ID, "true", false, true)) {
		return new BoolAtom(true);
	} else if (accept(TokenType::ID, "false", false, true)) {
		return new BoolAtom(false);
	} else {
		next();
		return nullptr;
	}
}

Node* LangParser::power() {
	Node* left = atom();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "(", false)) {
		std::vector<Node*> args;
		if (current().lexeme != ")") {
			args = argList();
		}
		Node* node = new CallOp(left, args);
		if (!expect(TokenType::OTHER, ")", false)) {
			delete node;
			return nullptr;
		}
		return node;
	} else if (accept(TokenType::OTHER, "[", false)) {
		Node* node = test();
		if (node == nullptr) return nullptr;
		if (!expect(TokenType::OTHER, "]", false)) return nullptr;
		return node;
	} else if (accept(TokenType::OTHER, ".", false)) {
		expect(TokenType::ID);
		Node* right = atom();
		if (right == nullptr) return nullptr;
		return right;
	}

	if (accept(TokenType::OTHER, "**", false)) {
		Node* right = factor();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, "**");
	}

	return left;
}

Node* LangParser::factor() {
	if (accept(TokenType::OTHER, "[+\\-~]")) {
		std::string op = last().lexeme;
		Node* right = factor();
		if (right == nullptr) return nullptr;

		return new UnOp(right, op);
	} else {
		return power();
	}
}

Node* LangParser::term() {
	Node* left = factor();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "[*\/%]")) {
		std::string op = last().lexeme;
		Node* right = factor();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::arithExpr() {
	Node* left = term();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "[+-]")) {
		std::string op = last().lexeme;
		Node* right = term();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::shiftExpr() {
	Node* left = arithExpr();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "(>>|<<)")) {
		std::string op = last().lexeme;
		Node* right = arithExpr();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::andExpr() {
	Node* left = shiftExpr();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "&", false)) {
		std::string op = last().lexeme;
		Node* right = shiftExpr();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::xorExpr() {
	Node* left = andExpr();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "^", false)) {
		std::string op = last().lexeme;
		Node* right = andExpr();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::expr() {
	Node* left = xorExpr();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "|", false)) {
		std::string op = last().lexeme;
		Node* right = xorExpr();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::comparison() {
	Node* left = expr();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "(<=|>=|!=|==|is|has|>|<)")) {
		std::string op = last().lexeme;
		Node* right = expr();
		if (right == nullptr) return nullptr;
		return new BinOp(left, right, op);
	}

	return left;
}

Node* LangParser::notTest() {
	if (accept(TokenType::OTHER, "!", false)) {
		Node* right = comparison();
		if (right == nullptr) return nullptr;
		return new UnOp(right, "!");
	} else {
		return comparison();
	}
	return nullptr;
}

Node* LangParser::andTest() {
	Node* left = notTest();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "&&", false)) {
		Node* right = notTest();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, "&&");
	}

	return left;
}

Node* LangParser::orTest() {
	Node* left = andTest();
	if (left == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "||", false)) {
		Node* right = andTest();
		if (right == nullptr) return nullptr;

		return new BinOp(left, right, "||");
	}

	return left;
}

Node* LangParser::test() {
	Node* cond = orTest();
	if (cond == nullptr) return nullptr;

	if (accept(TokenType::OTHER, "?", false)) {
		Node* left = test();
		if (left == nullptr) return nullptr;
		next();
		Node* right = test();
		if (right == nullptr) return nullptr;

		return new TernaryOp(cond, left, right);
	}

	return cond;
}

std::vector<Node*> LangParser::argList() {
	std::vector<Node*> nodes;
	Node* arg = test();
	if (arg == nullptr) return nodes;

	nodes.push_back(arg);
	while (accept(TokenType::OTHER, ",", false)) {
		Node* next = test();
		if (next == nullptr) continue;
		nodes.push_back(next);
	}

	return nodes;
}

Node* LangParser::stmt() {
	if (accept(TokenType::OTHER, "}", false)) {
		stepBack();
		return nullptr;
	}

	if (accept(TokenType::OTHER, ";", false)) {
		return new SemicolonStmt();
	} else if (accept(TokenType::ID, "break", false, true)) {
		Node* node = new BreakStmt();
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else if (accept(TokenType::ID, "continue", false, true)) {
		Node* node = new ContinueStmt();
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else if (accept(TokenType::ID, "return", false, true)) {
		Node* val = test();
		Node* node = new ReturnStmt(val);
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else if (accept(TokenType::OTHER, "++", false)) {
		Node* right = test();
		if (right == nullptr) return nullptr;

		Node* node = new IncrementStmt(right, true);
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else if (accept(TokenType::OTHER, "--", false)) {
		Node* right = test();
		if (right == nullptr) return nullptr;

		Node* node = new DecrementStmt(right, true);
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else if (accept(TokenType::ID, "if", false, true)) {
		return ifStmt();
	}

	Node* letDef = letStmt();
	if (letDef != nullptr) {
		return letDef;
	}

	Node* forDef = forStmt();
	if (forDef != nullptr) {
		return forDef;
	}

	Node* whileDef = whileStmt();
	if (whileDef != nullptr) {
		return whileDef;
	}

	Node* func = funcDef();
	if (func != nullptr) {
		return func;
	}

	Node* left = test();
	if (accept(TokenType::OTHER, "=", false)) {
		Node* right = test();
		if (right == nullptr) return nullptr;
		if (expect(TokenType::SEMI)) return new AssignmentStmt(left, right);
	} else if (accept(TokenType::OTHER, "(\\+=|-=|\\*=|\\/=|%=|&=|\\|=|^=|<<=|>>=|\\*\\*=)")) {
		std::string lex = last().lexeme;
		std::string op = lex.substr(0, lex.find_last_of('='));
		Node* right = test();
		if (right == nullptr) return nullptr;

		Node* node = new AssignmentStmt(left, new BinOp(left, right, op));
		if (expect(TokenType::SEMI)) return node;
		else {
			delete node;
			return nullptr;
		}
	} else {
		return left;
	}

	return new EOFAtom();
}

// 'if' test '{' stmt* '}' ('else if' test '{' stmt* '}')* ('else' '{' stmt* '}')?
Node* LangParser::ifStmt() {
	IfStmt* ifstmt = new IfStmt();
	Node* cond = test();
	if (cond == nullptr) stepBack();

	if (expect(TokenType::OTHER, "{", false)) {
		if (cond != nullptr) ifstmt->cond = NodePtr(cond);

		std::vector<Node*> stmts0 = stmtList();
		for (Node* node : stmts0) {
			if (node == nullptr) continue;
			ifstmt->stmts.push_back(NodePtr(node));
		}

		bool hasElseIf = false;
		while (accept(TokenType::ID, "else", false, true) &&
			   accept(TokenType::ID, "if", false, true))
		{
			IfStmt* elseifstmt = new IfStmt();
			elseifstmt->cond = NodePtr(test());

			next();
			std::vector<Node*> stmts1 = stmtList();
			for (Node* node : stmts1) {
				if (node == nullptr) continue;
				elseifstmt->stmts.push_back(NodePtr(node));
			}

			ifstmt->elseIfs.push_back(IfStmtPtr(elseifstmt));
			hasElseIf = true;
		}

		if (hasElseIf) {
			stepBack();
		}

		if (accept(TokenType::ID, "else", false, true)) {
			IfStmt* elsestmt = new IfStmt();

			next();
			std::vector<Node*> stmts1 = stmtList();
			for (Node* node : stmts1) {
				if (node == nullptr) continue;
				elsestmt->stmts.push_back(NodePtr(node));
			}
			ifstmt->elseStmt = IfStmtPtr(elsestmt);
		}
	} else {
		delete ifstmt;
		return nullptr;
	}
	return ifstmt;
}

std::vector<Node*> LangParser::stmtList() {
	std::vector<Node*> stmts;
	int balance = 1;
	stepBack();
	if (accept(TokenType::OTHER, "{", false)) {
		while (balance > 0) {
			Node* n = stmt();
			if (n != nullptr) stmts.push_back(n);
			if (current().lexeme == "{") {
				balance++;
			} else if (current().lexeme == "}") {
				balance--;
			}
		}
		next();
	}
	return stmts;
}

Node* LangParser::letStmt() {
	bool publicLet = false;
	if (accept(TokenType::ID, "pub", false, true)) {
		publicLet = true;
	}
	if (accept(TokenType::ID, "let", false, true) ||
		accept(TokenType::ID, "const", false, true))
	{
		std::vector<Node*> params = paramList();
		if (params.empty()) {
			error(
				"ERROR(" <<
				current().line <<
				":" <<
				current().pos <<
				"): Expected variable list."
			);
			return nullptr;
		}

		LetStmt* let = new LetStmt();
		for (Node* node : params) {
			let->variableList.push_back(ParamPtr((ParamStmt*) node));
		}
		let->publicLet = publicLet;

		return let;
	}
	return nullptr;
}

Node* LangParser::funcDef() {
	bool publicFunc = false;
	if (accept(TokenType::ID, "pub", false, true)) {
		publicFunc = true;
	}

	if (accept(TokenType::ID, "func", false, true)) {
		std::string name = current().lexeme;
		next();

		if (expect(TokenType::OTHER, "(", false)) {
			std::vector<Node*> params;
			if (current().lexeme != ")")
				params = paramList();
			next();

			FuncDefStmt* func = new FuncDefStmt();
			
			for (Node* node : params) {
				func->paramList.push_back(ParamPtr((ParamStmt*) node));
			}
			func->publicFunc = publicFunc;
			func->name = name;

			std::vector<Node*> stmts;
			if (expect(TokenType::OTHER, "{", false)) {
				stmts = stmtList();
			}

			for (Node* n : stmts) {
				func->stmts.push_back(NodePtr(n));
			}

			return func;
		}
	}
	return nullptr;
}

Node* LangParser::forStmt() {
	if (accept(TokenType::ID, "for", false, true)) {
		std::vector<Node*> idList = paramList(false);
		if (expect(TokenType::ID, "in", false, true)) {
			Node* a = test();
			Node* iter = a;
			if (accept(TokenType::OTHER, "..", false)) {
				Node* b = test();
				if (b == nullptr) return nullptr;
				iter = new RangeStmt(a, b);
			}

			std::vector<Node*> stmts;
			if (expect(TokenType::OTHER, "{", false)) {
				stmts = stmtList();
			}

			ForStmt* forStmt = new ForStmt();
			forStmt->iter = NodePtr(iter);
			
			for (Node* n : idList) {
				forStmt->vars.push_back(NodePtr(n));
			}

			for (Node* n : stmts) {
				forStmt->stmts.push_back(NodePtr(n));
			}

			return forStmt;
		}
	}
	return nullptr;
}

Node* LangParser::whileStmt() {
	if (accept(TokenType::ID, "while", false, true)) {
		Node* cond = test();
		if (cond == nullptr) return nullptr;

		std::vector<Node*> stmts;
		if (expect(TokenType::OTHER, "{", false)) {
			stmts = stmtList();
		}

		WhileStmt* whileStmt = new WhileStmt();
		whileStmt->cond = NodePtr(cond);
		for (Node* n : stmts) {
			whileStmt->stmts.push_back(NodePtr(n));
		}

		return whileStmt;
	}
	return nullptr;
}

Node* LangParser::param(bool checkAssign) {
	if (expect(TokenType::ID)) {
		ParamStmt* p = new ParamStmt();
		p->name = last().lexeme;
		if (accept(TokenType::OTHER, "=", false) && checkAssign) {
			Node* val = test();
			p->value = NodePtr(val);
		}
		return p;
	}
	return nullptr;
}

std::vector<Node*> LangParser::paramList(bool checkAssign) {
	std::vector<Node*> nodes;
	Node* arg = param(checkAssign);
	if (arg == nullptr) return nodes;

	nodes.push_back(arg);
	while (accept(TokenType::OTHER, ",", false)) {
		Node* next = param(checkAssign);
		if (next == nullptr) continue;
		nodes.push_back(next);
	}

	return nodes;
}
