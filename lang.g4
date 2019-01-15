// This grammar is supposed to parse this type of syntax:
/*
// this is a comment

pub static let a: number = 10;

func add(a, b) {
	return a + b;
}

func test() {
	let b = 5;
	if (b > 10) {
		return 42 * b;
	}
	return 42 * a;
}

test();
*/

grammar lang;

// PARSER RULES
program : block? EOF;

return_stmt : 'return' test? ';';

if_stmt
	: 'if' test '{' stmt* '}' ('else if' test '{' stmt* '}')* ('else' '{' stmt* '}')?
	;

for_stmt
	: 'for' idlist 'in' (range | test) '{' stmt* '}'
	;

while_stmt
	: 'while' test '{' stmt* '}'
	;

range
	: test '.' '.' test
	;

param
	: ID ('=' test)?
	;
params : param (',' param)*;
block : stmt+;

func_def_stmt
	: 'func' ID func_args '{' block? '}'
	;

func_args
	: '(' params? ')'
	;

lambda_def_stmt
	: '|' params? '|' '{' block? '}' ';'
	;

let_def_stmt
	: 'pub'? ('const' | 'let') params ';'
	;

stmt
	: ';'
	| 'break' ';'
	| 'continue' ';'
	| test '=' test ';'
	| test op_augassign test ';'
	| test ('++' | '--') ';'
	| ('++' | '--') test ';'
	| if_stmt
	| test ';'
	| let_def_stmt
	| func_def_stmt
	| for_stmt
	| while_stmt
	| lambda_def_stmt
	| return_stmt
	;

test: or_test ('?' test ':' test)?
    ;
or_test: and_test ('||' and_test)*
    ;
and_test: not_test ('&&' not_test)*
    ;
not_test: '!' not_test | comparison
    ;
comparison: expr (op_comp expr)*
    ;

op_augassign : '+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' | '<<=' | '>>=' | '**=';
op_comp : '<' | '>' | '<=' | '>=' | '!=' | '==' | 'is' | 'has';

expr: xor_expr ('|' xor_expr)*
	;
xor_expr: and_expr ('^' and_expr)*
	;
and_expr: shift_expr ('&' shift_expr)*
	;
shift_expr: arith_expr (('<<'|'>>') arith_expr)*
	;
arith_expr: term (('+'|'-') term)*
	;
term: factor (('*'|'/'|'%') factor)*
	;
factor: ('+'|'-'|'~') factor | power
	;
power: atom trailer* ('**' factor)?
	;

atom : ID | NUMBER | STRING | '(' test ')';

trailer
	: '(' (arglist)? ')' | '[' test ']' | '.' ID
    ;

arglist
	: test (',' test)*
	;

idlist
	: ID (',' ID)*
	;

// LEXER RULES
ID : [a-zA-Z_][a-zA-Z0-9_]*;

STRING : '"' (ESC_SEQ | ~('\\'|'"'))* '"';
CHAR : '\'' (ESC_SEQ | ~('\\'|'\'')) '\'';

NUMBER
	: SIGN? [0-9]+ EXPONENT?
	| SIGN? [0-9]+ '.' [0-9]+ EXPONENT? 'f'?
	| SIGN? '.' [0-9]+ EXPONENT? 'f'?
	| SIGN? HEX_NUMBER
	;

fragment
HEX_NUMBER
	: '0' [xX] HEX+
	;

fragment
HEX
	: [a-fA-F0-9]
	;

EXPONENT
	: [eE] SIGN? [0-9]+
	;

fragment
ESC_SEQ
	: '\\' ('b'|'t'|'n'|'f'|'r'|'"'|'\''|'\\')
	| UNICODE_ESC
	;

fragment
UNICODE_ESC
	: '\\' 'u' HEX HEX HEX HEX
	;

WS
   : [ \r\n\t]+ -> skip
   ;

fragment
SIGN
	: [+-]
	;