#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include<algorithm>
#include"scanner.h"
#include"parser.h"
#include<iterator>
#include<sstream>
#include<unistd.h>
#include<map>

using namespace lexer;


int main(int argc, char **argv) {
	if(argc < 2) {
		std::cerr << argv[0] << ": invalid arugments\nproper use: parseTest -c inputlevel.txt -f output.txt\n";
		return EXIT_FAILURE;
	}
	int opt;
	char *f = argv[1];
	char *of = 0;
	while ((opt = getopt(argc, argv, "c:f:")) != -1) {
		switch(opt) {
			case 'c':
				f = optarg;
			break;
			case 'f':
				of = optarg;
			break;
		}
	}

	std::string txt;
	std::fstream file;
	file.open(f, std::ios::in | std::ios::binary);
	if(!file.is_open()) { std::cout << "could not find source file\n"; return EXIT_FAILURE; }
	/* commented out code doesn't grab \n for some reason
	std::ostringstream advance;
	std::istream_iterator<unsigned char> sin(file);
	std::ostream_iterator<unsigned char> sout(advance);
	std::copy(sin,std::istream_iterator<unsigned char>(),sout);
	std::cout << advance.str() << ":\n";
	 */
	char *fptr=0;
	file.seekg(0, std::ios::end);
	size_t len = file.tellg();
	file.seekg(0, std::ios::beg);
	fptr = new char [ len+1 ];
	file.read(fptr, len);
	file.close();
	txt = fptr;
	delete [] fptr;
	
	Scanner scan(txt);
	parse::MXLevelParser p(&scan);
	p.collectTokens();
	lvl::Level *lvl = p.parse();

	if(lvl == 0) {
		std::cerr << "There were errors parsing cannot continue.. \n";
		return -1;
	}

	if(of != 0) {
		std::fstream file;
		file.open(of, std::ios::out);
		if(file.is_open()) {
			unsigned int i = 0;
			std::map<std::string, std::string> sprite_tokens;
			for(i = 0; i < lvl->sprites.size(); ++i) {
				std::ostringstream shorthand;
				shorthand << "s" << i;
				sprite_tokens[lvl->sprites[i].value] = shorthand.str();
			}
			for(i = 0; i < lvl->sprites.size(); ++i) {
				file << "\"" << lvl->sprites[i].fname << "\":";
				file << "\"" << sprite_tokens[lvl->sprites[i].value] << "\" = {\n";
				file << "\t\t" << lvl->sprites[i].cx << ":" << lvl->sprites[i].cy << ":" << lvl->sprites[i].cw << ":" << lvl->sprites[i].ch << "\n\t\t}\n\n";
			}
			file << lvl->name << "<" << lvl->level_width << "," << lvl->level_height << "," << lvl->level_max_x << ", " << lvl->level_max_y << "> {\n";
			for(int x = 0; x < lvl->level_max_x; ++x) {
				for(int y = 0; y < lvl->level_max_y; ++y) {
					if(lvl->items[x][y] != 0) {
						file << "{" << x << ", " << y << ", " << sprite_tokens[lvl->items[x][y]->value] << "},\n";
					}
				}
			}
			file << " 0 }\n\n";
			file.close();
		} else {
			std::cerr << "Could not open file: " << of << " for output\n";
			return -1;
		}
	} else { p.debugPrintTokens(); }
	//lvl->loadLevelGraphics();
	/*Token *token;
	token_type ttype;
	
	while ((token = scan.GetToken(ttype)) != NULL) {
		// save token
		// 
		switch(token->t_type) {
			case TYPE_DIGIT:
				std::cout << "Digit: ";
				break;
			case TYPE_SYMBOL:
				std::cout << "Symbol: ";
				break;
			case TYPE_ID:
				std::cout << "Text: ";
				break;
			case TYPE_STRING:
				std::cout << "String: ";
				break;
		}
		std::cout << token->line << " @ " << token->over <<  " := " <<  token->t_text << "\n";
	 
		delete token;
	}*/
	return EXIT_SUCCESS;
}


