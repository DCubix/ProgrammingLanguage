#ifndef LANG_OPS_HPP
#define LANG_OPS_HPP

#include "../parser.h"

struct BinOp : public Node {
	NodePtr left, right;
	std::string op;

	BinOp() = default;
	BinOp(Node* left, Node* right, const std::string& op)
		: left(NodePtr(left)), right(NodePtr(right)), op(op)
	{}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "BinOp(" << std::endl;
		left->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << op << std::endl;
		right->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct UnOp : public Node {
	NodePtr right;
	std::string op;

	UnOp() = default;
	UnOp(Node* right, const std::string& op)
		: right(NodePtr(right)), op(op) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "UnOp(" << std::endl;
		std::cout << std::string(pad + 4, ' ') << op << std::endl;
		right->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

struct TernaryOp : public Node {
	NodePtr cond, left, right;

	TernaryOp() = default;
	TernaryOp(Node* cond, Node* left, Node* right)
		: cond(NodePtr(cond)), left(NodePtr(left)), right(NodePtr(right))
	{}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "TernaryOp(" << std::endl;
		cond->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << "?" << std::endl;
		left->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << ":" << std::endl;
		right->print(pad + 4);
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

//struct ListOp : public Node {
//	NodeList items;
//
//	ListOp() = default;
//	ListOp(const NodeList& items) : items(items) {}
//
//	void print(int pad = 0) {
//		std::cout << std::string(pad, ' ') << "ListOp(" << std::endl;
//		std::cout << std::string(pad + 4, ' ') << "... (" << items.size() << "item(s))" << std::endl;
//		std::cout << std::string(pad, ' ') << ")" << std::endl;
//	}
//};

struct CallOp : public Node {
	NodePtr func;
	NodeList items;

	CallOp() = default;
	CallOp(Node* func, const std::vector<Node*>& items)
		: func(NodePtr(func))
	{
		for (Node* node : items) {
			this->items.push_back(NodePtr(node));
		}
	}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "CallOp(" << std::endl;
		func->print(pad + 4);
		std::cout << std::string(pad + 4, ' ') << "[" << std::endl;
		for (auto&& node : items) {
			node->print(pad + 8);
		}
		std::cout << std::string(pad + 4, ' ') << "]" << std::endl;
		std::cout << std::string(pad, ' ') << ")" << std::endl;
	}
};

#endif // LANG_OPS_HPP