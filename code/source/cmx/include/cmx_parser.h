#ifndef __PARSER_H__
#define __PARSER_H__

#include"cmx_scanner.h"
#include"cmx_syntax.h"
#include<vector>
#include<map>
#include "cmx_mxlvl.h"

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
		void set(Scanner *scanner) { scan = scanner; }
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

	void Parser::collectTokens() {
		Token *token;
		token_type ttype;
		while ((token=scan->GetToken(ttype)) != NULL) {
			SNode *n = src_tbl.input(token);
			n->type = ttype;
			tokens.push_back(n);
		}
		over = false;
		sprite_count = 0;
	}


	void Parser::debugPrintTokens() {
		int i,z;
		std::cout << "Level title: " << level.name << "\nlevel width: " << level.level_width << "\nlevel height: " << level.level_height << "\nmaxX: " << level.level_max_x << "\nmaxY: " << level.level_max_y << "\n";

		for(i = 0; i < level.level_max_x; ++i) {
			for(z = 0; z < level.level_max_y; ++z) {
				if(level.items[i][z] != 0) {
					std::cout << "Item @[" << i << "," << z << "] = " << level.items[i][z]->value << "\n";
				}
			}
		}
		std::cout << "\nLevel Sprites: \n";
		for(i = 0; i < (int)level.sprites.size(); i++) {
			std::cout << "Sprite Name: " << level.sprites[i].fname << " spriteID: " << level.sprites[i].value << " X: " << level.sprites[i].cx  << " Y: " << level.sprites[i].cy << " W: " << level.sprites[i].cw << " H: " << level.sprites[i].ch << "\n\n";
		}
	}

	void Parser::error(std::string str) {
		++error_num;
		std::cout <<  "Error: #" << error_num << " - " << str << ": " << tok->token->t_text << ":" << "type [" << tok->type << "] " << tok->token->line << "\n";
	}

	void Parser::sprite_statement() {

		if(tok->type == TYPE_STRING) {
			sprite_count++;
			std::string spriteName = tok->token->t_text;
			nexttoken();
			if(tok->type == TYPE_SYMBOL && tok->token->t_text == ":") {
				nexttoken();
				std::string spriteId = tok->token->t_text;
				nexttoken();
				if( tok->type == TYPE_SYMBOL) {
					nexttoken();
					nexttoken();
					int coordX=atoi(tok->token->t_text.c_str());
					nexttoken();
					if(tok->type == TYPE_SYMBOL) {
						nexttoken();
						int coordY=atoi(tok->token->t_text.c_str());
						nexttoken();
						nexttoken();
						int coordW=atoi(tok->token->t_text.c_str());
						nexttoken();
						nexttoken();
						int coordH=atoi(tok->token->t_text.c_str());
						nexttoken();
						nexttoken();
						int tileState = atoi(tok->token->t_text.c_str());
						level.sprites.push_back(lvl::Item(spriteId, spriteName, coordX, coordY, coordW, coordH, (lvl::BLOCK_TYPE)tileState));
						nexttoken();
						nexttoken();
						sprite_statement();
						return;
					}
				} else { error(" Expecting = {"); return; }
			} else { error("missing : in sprite statement\n"); return; }
		}
	}
	void Parser::level_statement() {
		if(tok->type == TYPE_ID) {
			level.name = tok->token->t_text;
			nexttoken();
			if(tok->type == TYPE_SYMBOL) {
				nexttoken();
				level.level_width=atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				level.level_height=atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				level.level_max_x = atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				level.level_max_y = atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				level.block_width = atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				level.block_height = atoi(tok->token->t_text.c_str());
				nexttoken();
				nexttoken();
				if(level.level_height < 0 || level.level_width < 0 || level.level_max_x < 0 || level.level_max_y < 0) {
					std::cout << " error: invalid level width/height\n";
				}
				level.items = new lvl::Item**[level.level_max_x+1];
				for(int z = 0; z < level.level_max_x; ++z) {
					level.items[z] = new lvl::Item*[level.level_max_y+1];
					memset(level.items[z], 0, sizeof(level.items[z]));
				}
				level.layers = new lvl::Item**[level.level_max_x+1];
				for(int z = 0; z < level.level_max_x; z++) {
					level.layers[z] = new lvl::Item*[level.level_max_y+1];
					memset(level.layers[z], 0, sizeof(level.layers[z]));
				}
				nexttoken();
				bool active = true;
				while(active) {
					if(tok->type == TYPE_SYMBOL && tok->token->t_text == "{") {
						nexttoken();
						int blockType = 0;
						if(tok->token->t_text=="b")
							blockType = 1;
						 else if(tok->token->t_text=="o")
							blockType = 2;
						 else if(tok->token->t_text=="e")
							blockType = 3;
						nexttoken();
						nexttoken();
						int posX = atoi(tok->token->t_text.c_str());
						nexttoken();
						nexttoken();
						int posY = atoi(tok->token->t_text.c_str());
						nexttoken();
						nexttoken();
						std::string type;
						type = tok->token->t_text;
						nexttoken();
						nexttoken();
						nexttoken();
						std::vector<lvl::Item>::iterator i;


						for(i = level.sprites.begin(); i != level.sprites.end(); ++i) {
							if(i->value == type) {
								if(blockType == 1) level.items[posX][posY] = &(*i);
								else if(blockType == 2) level.layers[posX][posY] = &(*i);
								break;
							}
						}
					} else {
						active = false;
						return;
					}
				}
			} else {
				error(" Expected Symbol");
				return;
			}
		} else error("Level Statement requires ID first");
	}

	lvl::Level *Parser::parse() {
		index = 0;
		tok = tokens[0];
		sprite_statement();
		if(sprite_count == 0) {
			error("Sprite count must be at least one");
			return 0;
		}
		level_statement();
		if(error_num == 0)
		return &level;
		else return 0;
	}

	void Parser::nexttoken() {
		if(index < (int)tokens.size())
		tok = tokens[++index];
		else {
			tok = tokens[tokens.size()-1];;
			over = true;
		}
	}
}

#endif


