#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include "cmx_scanner.h"
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

	SNode *Symbol_Table::input(Token *token) {
		std::map<std::string, SNode *>::iterator it;
		it = table.find(token->t_text);
		if( it != table.end() ) {
			return it->second;
		}
		SNode *node = new SNode(token, TYPE_TOKEN);
		table[token->t_text] = node;
		return node ;
	}

	Symbol_Table::Symbol_Table() {

	}

	Symbol_Table::~Symbol_Table() {
		std::map<std::string, SNode*>::iterator it;
		for(it = table.begin(); it != table.end(); it++) {
			SNode *node = it->second;
			delete node;
		}
		table.erase(table.begin(), table.end());
	}
}

#endif

