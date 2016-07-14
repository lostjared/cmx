/*

  (C) 2010 Jared Bruni under GPL

*/

#ifndef CMXERROR_H_
#define CMXERROR_H_


#include<exception>
#include<iostream>
#include<string>
#include<sstream>

namespace cmx {
	class Exception {
	public:
		Exception() { }
		Exception(std::string err) {
			this->err = err;
		}
		std::string errorString() { return err; }
	protected:
		std::string err;
	};
}

#endif



