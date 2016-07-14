#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include "scanner.h"
#include<map>

namespace symbol {
	using namespace lexer;
	enum { TYPE_TOKEN=1 };

	class SNode {
	public:
		Token *token;
		unsigned int type;		
		SNode() { token = 0; type = 0; }
		SNode(Token *token, unsigned int type) { this->token = token; this->type = type; }
	};
	
	class Symbol_Table {
	public:
		Symbol_Table();
		~Symbol_Table();
		SNode *input(Token *token);
		SNode *operator[](std::string temp_name) { return table[temp_name]; }
	protected:
		std::map<std::string, SNode *> table;
		
	};
}

#endif

