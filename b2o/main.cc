// b2o binary to C array converter
// for resources
#include<iostream>
#include<fstream>
#include<sstream>

int main(int argc, char **argv) {
	if(argc != 2) {
		std::cerr << "Error requires one argument: " << argv[0] << " file\n";
		return 0;
	}
	std::fstream file;
	file.open(argv[1], std::ios::in | std::ios::binary);
	if(!file.is_open()) {
		std::cerr << "Error could not open: " << argv[1] << "\n";
		return -1;
	}
	std::fstream outfile;
	std::ostringstream stream;
	std::string str;
	for(int i = 0; i < std::string(argv[1]).length(); ++i) {
		if(argv[1][i] == '.') stream << "_";
		else stream << argv[1][i];
	}
	str = stream.str();
	stream.str("");
	stream << argv[1];
	stream << ".c";
	outfile.open(stream.str().c_str(), std::ios::out);
	if(!outfile.is_open()) {
		std::cerr << "Error could not open..: " << stream.str().c_str() << "\n";
		return -1;
	}
	unsigned int len;
	file.seekg(0, std::ios::end);
	len = file.tellg();
	file.seekg(0, std::ios::beg);
	
	len /= sizeof(unsigned int);
	
	outfile << "\n\nunsigned int " << str << "[" << len+1 << "] = {\n";
	for(int z = 0; z < len; ++z) {
		unsigned int temp=0;
		file.read((char*)&temp, sizeof(temp));
		outfile << temp << ", ";
		static int cnt = 0;
		if(((++cnt)%10) == 0) {
			outfile << "\n";
		}
	}
	outfile << "\n0 };\n";
	outfile.close();
	std::string fname=std::string(argv[1])+".h";
	std::fstream houtfile;
	std::cout << stream.str() << "\n" << fname << "\n";
	houtfile.open(fname.c_str(), std::ios::out);
	if(!houtfile.is_open()) {
		std::cerr << "Error could not open: " << fname.c_str() << "\n";
		return -1;
	}
	houtfile << "\n\n#ifndef " << str << "_h\n#define "<<str << "_h \n";
	houtfile << "extern unsigned int " << str << "[" << len+1 << "];";
	houtfile << "\n#endif\n\n";
	file.close();
	return 0;
}
