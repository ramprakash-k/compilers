#ifndef REG_MAN_H
#define REG_MAN_H

#include <iostream>
#include <string>
#include "basic.h"

using namespace std;

#define NUMREG 6

class RegMan {
public:
	string allocate(basicType t) {
		cout<<"Allocate"<<endl;
		int r = regs.front();
		string reg = numtoreg(r);
		regs.pop_front();
		used.push_back(r);
		types[r].push_back(t);
		return reg;
	}
	void free(string reg) {
		cout<<"Free"<<endl;
		int r = regtonum(reg);
		used.remove(r);
		regs.push_front(r);
		types[r].pop_back();
		cout<<"FreeEnd"<<endl;
	}
	void prepare(string reg, string rr) {
		int left = regtonum(reg);
		int right = regtonum(rr);

		if(fcount[left] == fcount[right]) {
			return;
		} else if(fcount[right]+1 == fcount[left]) {
			getFromStack(left);
			fcount[right]--;
		} else if(fcount[right] != fcount[left]+1) {
			cerr<<"Error: Register Manager "<<reg<<" "<<fcount[left]<<" "<<fcount[right]<<endl;
		}
	}
	RegMan() {
		for (int i = 0; i < NUMREG; ++i) {
			regs.push_back(i);
			fcount[i] = 0;
		}
	}
private:
	list<int> regs;
	list<int> used;
	int fcount[NUMREG];
	vector<basicType> types[NUMREG];
	string numtoreg(int n) {
		if(n<4)return string("e")+char('a'+n)+"x";
		if(n==4) return string("edi");
		if(n==5) return string("esi");
	}
	int regtonum(string reg) {
		if(reg[2]=='x')return reg[1]-'a';
		if(reg[1]=='d')return 4;
		return 5;
	}
	void moveToStack(int r) {
		string reg = numtoreg(r);
		vector<basicType> &vtypes = types[r];

		code<<"\tpush"<<(vtypes[vtypes.size()-1]==cfloat?"f":"i")
			<<"("<<reg<<");"<<endl;code_line++;
	}
	void getFromStack(int r) {
		string reg = numtoreg(r);
		vector<basicType> &vtypes = types[r];

		code<<"\tload"<<(vtypes[vtypes.size()-1]==cfloat?"f":"i")
			<<"(ind(esp),"<<reg<<");"<<endl;code_line++;
		code<<"pop"<<(vtypes[vtypes.size()-1]==cfloat?"f":"i")
			<<"(1);"<<endl;code_line++;
	}
};

#endif // REG_MAN_H
