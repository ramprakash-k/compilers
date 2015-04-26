#include <iostream>
#include <stdio.h>
#include <string>
#include "Scanner.h"
#include "Parser.h"

using namespace std;

extern global_sym g_sym;
extern map<std::string,abstract_astnode *> ast;
extern ofstream code;
extern int label;

int main ()
{
	Parser parser;
	parser.line_no=1;
	parser.parse();
	//g_sym.dump_all();
	for (map<std::string,abstract_astnode *>::iterator i = ast.begin(); i != ast.end(); ++i)
	{
		g_sym.set_l_sym(i->first);
		code<<"void "<<i->first<<"() {"<<endl;
		if(i->first=="main")
		{
			if(g_sym.present("main").second==cint)
				code<<"\tpushi(0);"<<endl;
			else if(g_sym.present("main").second==cfloat)
				code<<"\tpushf(0);"<<endl;
		}
		code<<"\tpushi (ebp);"<<endl;
		code<<"\tmove (esp,ebp);"<<endl;
		code<<"\taddi ("<<g_sym.local_size(i->first)<<",esp);"<<endl;
		i->second->generate_code();
		code<<"\tloadi (ind(ebp),ebp);"<<endl;
		code<<"\taddi ("<<-(g_sym.local_size(i->first))<<",esp);"<<endl;
		code<<"\tpopi(1);"<<endl;
		code<<"}"<<endl<<endl;
	}
}
