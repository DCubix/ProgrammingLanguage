#ifndef LANG_ATOM_HPP
#define LANG_ATOM_HPP

#include "../parser.h"

struct EOFAtom : public Node {
	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "EOF" << std::endl;
	}
};

struct BoolAtom : public Node {
	bool value;

	BoolAtom() = default;
	BoolAtom(bool value) : value(value) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "BOOL(" << value << ")" << std::endl;
	}
};

struct IdentifierAtom : public Node {
	std::string name;

	IdentifierAtom() = default;
	IdentifierAtom(const std::string& name) : name(name) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "ID(" << name << ")" << std::endl;
	}
};

struct NumberAtom : public Node {
	double value;

	NumberAtom() = default;
	NumberAtom(double value) : value(value) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "NUM(" << value << ")" << std::endl;
	}
};

struct StringAtom : public Node {
	std::string value;

	StringAtom() = default;
	StringAtom(const std::string& value) : value(value) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "STR(" << value << ")" << std::endl;
	}
};

struct CharAtom : public Node {
	char value;

	CharAtom() = default;
	CharAtom(char value) : value(value) {}

	void print(int pad = 0) {
		std::cout << std::string(pad, ' ') << "CHR(" << value << ")" << std::endl;
	}
};

#endif // LANG_ATOM_HPP