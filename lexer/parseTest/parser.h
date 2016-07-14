#ifndef __PARSER_H__
#define __PARSER_H__

#include"scanner.h"
#include"syntax.h"
#include<vector>
#include<deque>
#include "mxlvl.h"

namespace parse {
	using namespace lexer;
	using namespace symbol;

	class AST {
	public:
		std::vector<AST> children;
		Token *token;
		AST(Token *t) { token = t; }
		AST() { token = 0; }
	};
		
	typedef class Parser {
		Scanner *scan;
		int index;
		lvl::Level level;
		int error_num;
		bool over;
		int sprite_count;
	public:
		Parser() : scan(0) , error_num(0) { }
		Parser(Scanner *scanner) : scan(scanner), error_num(0) { }
		void collectTokens();
		void debugPrintTokens();
		void sprite_statement();
		void level_statement();
		void error(std::string str);
		lvl::Level *parse();
		Symbol_Table src_tbl;
		std::vector<SNode *> tokens;
		std::vector<Token *> stack;
		SNode *tok;
		
		void nexttoken();		
	} MXLevelParser;
}

#endif


