#include <iostream>

#include "lexer/lexer.h"
#include "parser/parser.h"

int main(int argc, char** argv) {
	LangLexer lex(R"(
let foo = "\tHello World! \"test\"";

func add(a, b) {
	return a + b;
}

add(2.5f, 10);

)");

	lex.tokenize();
	lex.printTokens();

	LangParser par(lex.tokens());
	par.parse();

	getchar();
	return 0;
}