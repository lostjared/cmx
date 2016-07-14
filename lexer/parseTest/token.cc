#include "token.h"
#include "scanner.h"




namespace lexer {

	unsigned char token_map[0xFF];
	const char *op_z[] = { "!", "!=", "+", "+=", "++", "-", "-=", "--","*", "*=", "**", "/", "/=","^", "^=",  "&", "&=", "|", "|=", "=", "==", "<", "<=", ">", ">=",  0 };
	
	
	Token::Token(token_type type, std::string text) {
		t_type = type;
		t_text = text;
	}
	
	token_type DigitToken::GetToken(Scanner *scanner) {
		unsigned char ch = scanner->GetChar();
		std::string text;
		
		while((ch != 0) && ((token_map[ch] == TYPE_DIGIT) || ch == '.')) {
			if(ch != 0) text += ch;
			ch = scanner->GetChar();
		}
		
		t_type = TYPE_DIGIT;
		t_text = text;
		line = scanner->line_index();
		pos = scanner->position();
		over = scanner->pos_over();
		return t_type;
	}
	
	unsigned int SymbolToken::lookupOp(std::string s)
	{
		size_t i=0;
		for(i = 0; op_z[i] != 0; ++i) 
			if(s == op_z[i]) return i;
		
		return OPTYPE_NULL;
	}
}
