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
		//cout<<"Allocate"<<endl;
		if(regs.empty()) {
			int x = used.front();
			used.pop_front();
			moveToStack(x);
			fcount[x]++;
			//cout<<"fcount "<<numtoreg(x)<<" : "<<fcount[x]<<endl;
			regs.push_back(x);
		}
		int r = regs.front();
		string reg = numtoreg(r);
		regs.pop_front();
		used.push_back(r);
		types[r].push_back(t);
		return reg;
	}
	void free(string reg) {
		//cout<<"Free"<<endl;
		int r = regtonum(reg);
		used.remove(r);
		regs.push_front(r);
		types[r].pop_back();
		//cout<<"FreeEnd"<<endl;
	}
	void prepare(string &reg, string rr,int orig) {
		int left = regtonum(reg);
		int right = regtonum(rr);
		if(left == right) {
			//cout<<"Same"<<endl;
			string newreg = allocate(*(types[left].end()-2));
			if(newreg == rr) {
				cout<<"O_o"<<endl;
				exit(1);
				//free(newreg);
				//newreg = allocate(*(types[left].end()-2));
			}
			int nr = regtonum(newreg);
			vector<basicType> &vtypes = types[left];
			code<<"\tload"<<(vtypes[vtypes.size()-2]==cfloat?"f":"i")
				<<"(ind(esp),"<<newreg<<");"<<endl;
			code<<"\tpop"<<(vtypes[vtypes.size()-2]==cfloat?"f":"i")
				<<"(1);"<<endl;
			fcount[left]--;
			//cout<<"fcount "<<fcount[left]<<endl;
			vtypes.erase(vtypes.begin() + vtypes.size()-2);
			reg = newreg;
			return;
		}
		if(fcount[left] == orig) {
			//cout<<"Equal"<<endl;
			return;
		} else if(orig+1 == fcount[left]) {
			//cout<<"Get"<<endl;
			getFromStack(left);
			fcount[left]--;
			//cout<<"fcount "<<fcount[left]<<endl;
			regs.remove(left);
			used.push_back(left);
		} else if(orig != fcount[left]+1) {
			cerr<<"Error: Register Manager "<<reg<<" "<<fcount[left]<<" "<<fcount[right]<<endl;
		}
	}
	int reg_in_use() {
		return used.size();
	}
	int getf(string reg) {
		return fcount[regtonum(reg)];
	}
	RegMan() {
		for (int i = 0; i < NUMREG; ++i) {
			regs.push_back(i);
			fcount[i] = 0;
		}
	}
	~RegMan() {
		//for (int i = 0; i < NUMREG; ++i)
		//{
			//cout<<fcount[i]<<" "<<types[i].size()<<" ";
		//}
		//cout<<endl;
		while(!regs.empty()) {
			//cout<<regs.front()<<" ";
			regs.pop_front();
		}
		//cout<<endl;
		while(!used.empty()) {
			//cout<<used.front()<<" ";
			used.pop_front();
		}
		//cout<<endl;
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
			<<"("<<reg<<");"<<endl;
	}
	void getFromStack(int r) {
		string reg = numtoreg(r);
		vector<basicType> &vtypes = types[r];
		code<<"\tload"<<(vtypes[vtypes.size()-1]==cfloat?"f":"i")
			<<"(ind(esp),"<<reg<<");"<<endl;
		code<<"\tpop"<<(vtypes[vtypes.size()-1]==cfloat?"f":"i")
			<<"(1);"<<endl;
	}
};

#endif // REG_MAN_H
