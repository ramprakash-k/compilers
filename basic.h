#ifndef BASIC_H
#define BASIC_H

#include <string>
using namespace std;

enum basicType
{
	cint,
	cfloat,
	cvoid,
	cstring
};

static string btToString(basicType t) {
	switch(t) {
		case cint: return "int";
		case cfloat: return "float";
		case cvoid: return "void";
		case cstring: return "string";
		default: return "";
	}
}

static int sizeofbt(basicType t) {
	switch(t) {
		case cint: return sizeof(int);
		case cfloat: return sizeof(float);
		case cvoid: return 0;
		case cstring: return 0;
		default: return 0;
	}
}

#endif // BASIC_H
