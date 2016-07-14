#ifndef __SCANNER_H_
#define __SCANNER_H_
#include<iostream>
#include<string>
#include<cstring>
#include "cmx_token.h"
namespace lexer {
	class Scanner {
	public:
		Scanner(std::string text);
		Scanner() :pos(0), line(1), over(1)  { initScanner(); }
		void setText(std::string t) { text = t; }
		Token *GetToken(token_type&);
		char GetChar();
		char PeekChar();
		void PutBack();
		const int length() { return text.length(); }
		const int position() { return pos; }
		char &operator[](unsigned int pos) {  return text[pos]; }
		char &operator*() { return text[pos]; }
		const int line_index() { return line; }
		const int pos_over() { return over; }
		void operator++() {  if(text[pos] == '\n') { line++; over = 1; }  else ++over; ++pos; }
		void operator--() { --pos; }
	protected:
		std::string text;
		int pos, line, over;
		void initScanner();
		
	};
}
#endif

