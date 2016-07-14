#include"syntax.h"


namespace symbol {

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


