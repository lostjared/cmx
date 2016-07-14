#include "cmx_token.h"
#include "cmx_scanner.h"


namespace lexer {

	Scanner::Scanner(std::string text) : pos(0), line(1), over(1) {
		this->text = text;
		initScanner();
	}

	void Scanner::initScanner() {
		unsigned char c = 0;
		memset(token_map, 0, sizeof(token_map));
		for(c = 'a'; c <= 'z'; c++)
			token_map[c] = TYPE_ID;
		for(c = 'A'; c <= 'Z'; c++)
			token_map[c] = TYPE_ID;
		token_map['_'] = TYPE_ID;
		for(c = '0'; c <= '9'; c++)
			token_map[c] = TYPE_DIGIT;
		token_map[','] = token_map['.'] = token_map['<']  =
		token_map['>'] = token_map['!'] = token_map['@']  =
		token_map['#'] = token_map['$'] = token_map['%']  =
		token_map['^'] = token_map['&'] = token_map['*']  =
		token_map['('] = token_map[')'] = token_map['{']  =
		token_map['}'] = token_map['/'] = token_map['\\'] =
		token_map[':'] = token_map[';'] = token_map['`']  = token_map['='] =
		token_map['~'] = token_map['|'] = token_map['+']  = token_map['-'] = TYPE_SYMBOL;
		token_map['\n'] = token_map['\r'] = token_map[' '] = token_map['\t'] = TYPE_NULL;
		token_map['\"'] = token_map['\''] = TYPE_STRING;
	}

	char Scanner::GetChar() {
		if(pos >= 0 && pos < (int)text.length())
		{ over++; if(text[pos] == '\n') { line++; over = 1; } return text[pos++]; }
		return 0;
	}

	char Scanner::PeekChar() {
		if(pos >= 0 && pos < (int)text.length())
			return text[pos];
		return 0;
	}

	void Scanner::PutBack() {
		if(pos > 0) pos--; over--;
	}

	Token *Scanner::GetToken(token_type &ttype) {
		unsigned char c = operator*();
		if(c == ' ' || c == '\t' || c == '\n') {
			do {
				operator++();
				c = operator*();

			} while (c == ' ' || c == '\t' || c =='\n');
		}

		ttype = static_cast<token_type>(token_map[c]);
		switch(ttype) {
			case TYPE_STRING: {
				std::string temp;
				bool active = true;
				int nover = over, nline = line;
				operator++();
				c = operator*();
				do {
					if(c == '\\') {
						operator++();
						c = operator*();
						switch(c) {
							case '\\':
								temp += "\\";
							case 'n':
								temp += "\n";
								break;
							case 't':
								temp += "\t";
								break;
							case '\"':
								temp += "\"";
								break;
						}
					} else temp += c;
					operator++();
					c = operator*();
					if(token_map[c] == TYPE_STRING) { active = false; operator++(); }
				} while( c != 0 && active );
				Token *token;
				token = new StringToken(TYPE_STRING, temp);
				token->over = nover;
				token->line = nline;
				return token;
			}
				break;
			case TYPE_ID: {
				std::string temp;
				int nline = line, nover = over;
				do {
					temp += c;
					operator++();
					c = operator*();
				} while (token_map[c] == TYPE_ID || token_map[c] == TYPE_DIGIT);
				Token *token;
				token = new IdToken(TYPE_ID, temp);
				token->line = nline;
				token->over = nover;
				return token;
			}
				break;
			case TYPE_DIGIT: {
				int nover = over;
				int nline = this->line;
				std::string str;
				unsigned char c = operator*();
					do {
						str += c;
						operator++();
						c = operator*();
					} while ( token_map[c] == TYPE_DIGIT || c == '.');
					Token *token = new DigitToken(TYPE_DIGIT, str);
					token->line = nline;
					token->over = nover;
					return token;
				}
					break;
				case TYPE_SYMBOL:
				{
					int nover = over;
					int nline = line;
					Token *token=0;
					std::string temp;
					temp += c;
					if(pos+1 < (int)text.length() ) {
						OperatorType t;
						char cc = text[pos+1];
						bool ok = true;
						switch(c) {
							case '(':
							case ')':
							case '[':
							case ']':
							case '{':
							case '}':
							case ',':
							case ';':
							case ':':
							case '`':
							case '~':
								ok = false;
							break;
						}
						if(ok == false) {
							t = OPTYPE_SINGLE;
							operator++();
						} else if(cc == c) {
							temp += cc;
							operator++();
							operator++();
							t = OPTYPE_S;
						} else if(cc == '=') {
							temp += cc;
							operator++();
							operator++();
							t = OPTYPE_E;
						} else {
							t = OPTYPE_SINGLE;
							operator++();
						}
					} else { temp=""; temp+=c; operator++(); }
					token = new SymbolToken(TYPE_SYMBOL, temp);
					token->line = nline;
					token->over = nover;
					return token;
				}
					break;
			default:
				if(pos < (int)text.length()) {
					std::cout << "unrecongized character: " << int(c) << "/" << c << std::endl;
					operator++();
					return GetToken(ttype); // skip
				}
				else return NULL;
		}
		return NULL;
	}

	unsigned char token_map[0xFF];
	const char *op_z[] = { "!", "!=", "+", "+=", "++", "-", "-=", "--","*", "*=", "**", "/", "/=","^", "^=",  "&", "&=", "|", "|=", "=", "==", "<", "<=", ">", ">=",  0 };


	Token::Token(token_type type, std::string text) {
		t_type = type;
		t_text = text;
	}

	token_type DigitToken::GetToken(Scanner *scanner) {
		unsigned char ch = scanner->GetChar();
		std::string text;

		while( ch != 0 && ((token_map[ch] == TYPE_DIGIT) || ch == '.')) {
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
