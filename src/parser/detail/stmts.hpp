#ifndef LANG_STMT_HPP
#define LANG_STMT_HPP

#include "../parser.h"

struct SemicolonStmt : public Node {
	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << ";" << std::endl;
	}
};

struct BreakStmt : public Node {
	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "BREAK" << std::endl;
	}
};

struct ContinueStmt : public Node {
	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "CONT" << std::endl;
	}
};

struct AssignmentStmt : public Node {
	NodePtr left, right;

	AssignmentStmt() = default;
	AssignmentStmt(Node* left, Node* right)
		: left(NodePtr(left)), right(NodePtr(right))
	{}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "AssignmentStmt(" << std::endl;
		left->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << "=" << std::endl;
		right->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct IncrementStmt : public Node {
	bool pre;
	NodePtr node;

	IncrementStmt() = default;
	IncrementStmt(Node* node, bool pre) : node(NodePtr(node)), pre(pre) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "IncrementStmt(" << std::endl;
		if (pre) {
			std::cout << std::string(pad + 4, ' ') << "++" << std::endl;
			node->print(pad + 4);
		} else {
			node->print(pad + 4);
			std::cout << std::string(pad + 4, ' ') << "++" << std::endl;
		}
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct DecrementStmt : public Node {
	bool pre;
	NodePtr node;

	DecrementStmt() = default;
	DecrementStmt(Node* node, bool pre) : node(NodePtr(node)), pre(pre) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "DecrementStmt(" << std::endl;
		if (pre) {
			std::cout << std::string(pad + 4, ' ') << "--" << std::endl;
			node->print(pad + 4);
		} else {
			node->print(pad + 4);
			std::cout << std::string(pad + 4, ' ') << "--" << std::endl;
		}
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct IfStmt;
using IfStmtPtr = std::unique_ptr<IfStmt>;
using IfStmtList = std::vector<IfStmtPtr>;
struct IfStmt : public Node {
	NodePtr cond;
	NodeList stmts;
	IfStmtList elseIfs;
	IfStmtPtr elseStmt;
	
	IfStmt() = default;

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "IfStmt(" << std::endl;
		if (cond) cond->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << "[" << std::endl;
		for (auto&& stmt : stmts) {
			stmt->print(pad + 8);
		}
		std::cout << std::string(pad + 4, ' ') << "]" << std::endl;
		for (auto&& elseif : elseIfs) {
			std::cout << std::string(pad + 4, ' ') << "[else if]";
			elseif->print(pad + 4);
		}
		if (elseStmt) {
			std::cout << std::string(pad + 4, ' ') << "[else]";
			elseStmt->print(pad + 4);
		}
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct ParamStmt : public Node {
	std::string name;
	NodePtr value;

	ParamStmt() = default;
	ParamStmt(const std::string& name, Node* value)
		: name(name), value(NodePtr(value))
	{}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "ParamStmt(" << std::endl;
		std::cout << std::string(pad + 4, ' ') << name << std::endl;
		if (value) {
			value->print(pad + 4);
		}
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

using ParamPtr = std::unique_ptr<ParamStmt>;
using ParamList = std::vector<ParamPtr>;
struct LetStmt : public Node {
	ParamList variableList;
	bool publicLet;

	LetStmt() = default;

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "LetStmt(" << std::endl;
		std::cout << std::string(pad + 4, ' ') << "[" << std::endl;
		for (auto&& var : variableList) {
			var->print(pad + 8);
		}
		std::cout << std::string(pad + 4, ' ') << "]" << std::endl;
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct FuncDefStmt : public Node {
	std::string name;
	ParamList paramList;
	NodeList stmts;
	bool publicFunc;

	FuncDefStmt() = default;

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "FuncDefStmt(" << std::endl;
		std::cout << std::string(pad + 4, ' ') << name << std::endl;
		std::cout << std::string(pad + 4, ' ') << "[" << std::endl;
		for (auto&& var : paramList) {
			var->print(pad + 8);
		}
		std::cout << std::string(pad + 4, ' ') << "]" << std::endl;
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct ReturnStmt : public Node {
	NodePtr value;

	ReturnStmt() = default;
	ReturnStmt(Node* val) : value(NodePtr(val)) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "ReturnStmt(" << std::endl;
		if (value) value->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct ForStmt : public Node {
	NodeList vars, stmts;
	NodePtr iter;

	ForStmt() = default;

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "ForStmt(" << std::endl;
		iter->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct RangeStmt : public Node {
	NodePtr from, to;

	RangeStmt() = default;
	RangeStmt(Node* from, Node* to) : from(NodePtr(from)), to(NodePtr(to)) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "RangeStmt(" << std::endl;
		from->print(pad + 4);
		to->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct WhileStmt : public Node {
	NodeList stmts;
	NodePtr cond;

	WhileStmt() = default;

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "WhileStmt(" << std::endl;
		cond->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

#endif // LANG_STMT_HPP