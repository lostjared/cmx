#ifndef __SCANER_H__
#define __SCANER_H__
#include<iostream>
#include<string>
#include<cstdlib>
#include<cstring>
#include<cstdio>

namespace lexer {
	enum token_type { TYPE_NULL=0,TYPE_DIGIT,TYPE_ID,TYPE_STRING, TYPE_SYMBOL };
	extern unsigned char token_map[0xFF];
	extern const char *op_z[];
	enum { OP_NOT=0, OP_NOTEQ, OP_PLUS, OP_PLUSEQ,  OP_PLUSPLUS, OP_MINUS, OP_MINUSEQ, OP_MINMIN, OP_MUL, OP_MULEQ, OP_MULMUL, OP_DIVX, OP_DIV, OP_DIVEQ, OP_XOR, OP_XOREQ, OP_AND,
		OP_ANDEQ, OP_OR, OP_EQ, OP_EQEQ,  OP_LT, OP_LTE, OPT_GR, OPT_GRE }
	;	enum OperatorType { OPTYPE_SINGLE, OPTYPE_S, OPTYPE_E , OPTYPE_NULL };

	class Scanner;

	class Token {
	public:
		Token(token_type type, std::string text);
		token_type t_type;
		std::string t_text;
		int pos, line, over;
		Token() { pos = line = over = 0; }
		virtual ~Token() { }
		virtual token_type GetToken(Scanner *) { return TYPE_NULL; }
	};
	
	class DigitToken : public Token {
	public:
		DigitToken(token_type t, std::string text) : Token(t, text) { value = atof(text.c_str()); }
		virtual token_type GetToken(Scanner *);
		float value;
	};
	
	class SymbolToken : public Token {
	public:	
		SymbolToken(token_type t, std::string text) : Token(t, text) { op_type = static_cast<OperatorType> (lookupOp(text)); }
		OperatorType op_type;
		unsigned int lookupOp(std::string s);
	};
	
	class IdToken : public Token {
	public:
		IdToken(token_type t, std::string text) : Token(t, text) { }
	};
	
	class StringToken : public Token {	
	public:
		StringToken(token_type t, std::string text) : Token(t, text) { }
	};
}
#endif

