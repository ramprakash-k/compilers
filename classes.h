#ifndef Classes_h_included
#define Classes_h_included

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <stdlib.h>
#include <sstream>
#include "basic.h"
using namespace std;

extern int offset;

extern int size;
extern stringstream code;
extern int label;
extern int code_line;

#include "reg_man.h"

extern RegMan regman;

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

struct typeExp {
	typeExp *t;
	basicType type;
	int arr;
	typeExp(){}
	typeExp(basicType t1)
	{
		t=NULL;
		type=t1;
		arr=0;
	}
	typeExp(basicType t1,string s)
	{
		type = t1;
		string temp=s.substr(s.find_last_of('[')+1);
		temp=temp.substr(0,temp.size()-1);
		arr=atoi(temp.c_str());
		temp=s.substr(0,s.find_last_of('['));
		if(temp == "")
			t = new typeExp(t1);
		else t = new typeExp(t1,temp);
	}
	typeExp(typeExp *t1,int t2)
	{
		t=t1;
		arr=t2;
		type=t1->type;
	}
	void print(bool top)
	{
		if(top)
			cout<<btToString(type);
		if(arr != 0) cout<<"["<<arr<<"]";
		if(t!=NULL)
			t->print(false);
	}
	int size()
	{
		int s = sizeofbt(type);
		if(t!=NULL)
			s=t->size();
		if(arr!=0) s *= arr;
		return s;
	}
};

static bool operator==(typeExp t1,typeExp t2) {
	basicType a=t1.type;
	basicType b=t2.type;
	if(a==cvoid||a==cstring||b==cvoid||b==cstring)return false;
	if(t1.t==NULL&&t2.t==NULL&&t1.type==t2.type)return true;
	if(t1.t==NULL||t2.t==NULL)return false;
	return (*(t1.t)==*(t2.t));
}

class l_entry {
public:
	l_entry(basicType t, string n) {
		if(n[0]=='[')
		{
			type=new typeExp(t,n.substr(0,n.find_last_of(']')+1));
			name=n.substr(n.find_last_of(']')+1);
		}
		else
		{
			type=new typeExp(t);
			name=n;
		}
	}
	string get_name() {
		return name;
	}
	typeExp* get_type() {
		return type;
	}
	void dump()
	{
		cout<<"("<<name<<",";
		type->print(true);
		cout<<","<<off<<")"<<endl;
	}
	int get_size() {
		return type->size();
	}
	void set_off(int o) {
		off = o;
	}
	int get_off() {
		return off;
	}
private:
	typeExp *type;
	string name;
	int off;
};

class local_sym {
public:
	local_sym(){}
	pair<bool,string> params_insert(l_entry *l) {
		l->set_off(offset);
		offset += l->get_size();
		params.push_back(l);
		if(local.find(l->get_name()) != local.end()) return make_pair(false,l->get_name());
		local[l->get_name()] = l;
		return make_pair(true,"");
	}
	pair<bool,string> local_insert(basicType type, list<string> *n) {
		bool flag=true;
		string s="";
		for (list<string>::iterator it=n->begin();it!=n->end();it++)
		{
			l_entry *l = new l_entry(type,*it);
			offset -= l->get_size();
			l->set_off(offset);
			if(local.find(l->get_name()) != local.end()){ flag = false; s = l->get_name(); }
			else local[l->get_name()] = l;
		}
		delete(n);
		return make_pair(flag,s);
	}
	pair<bool,typeExp> present(string s) {
		if(local.find(s) == local.end()) return make_pair(false,typeExp(cvoid));
		return make_pair(true,*(local[s]->get_type()));
	}
	vector<typeExp> get_param_type() {
		vector<typeExp> temp;
		for(unsigned int i=0;i<params.size();i++) {
			temp.push_back(*(params[i]->get_type()));
		}
		return temp;
	}
	void dump() {
		if(params.size() == 0) {
			cout<<"Params: None"<<endl<<endl;
		} else {
			cout<<"Params: ("<<endl;
			for (int i = 0; i < params.size(); ++i)
			{
				cout<<"\t"; params[i]->get_type()->print(true);
				if(i!=params.size()-1) cout<<",";
				cout<<endl;
			}
			cout<<")"<<endl<<endl;
		}
		cout<<"Symbol Table:"<<endl;
		for(map<string, l_entry *>::iterator iter = local.begin(); iter != local.end(); iter++)
		{
			iter->second->dump();
		}
		cout<<endl;
		cout<<"Return offset: "<<return_offset<<endl<<endl;
	}
	void setrt(basicType rt) {
		return_offset = offset;
		ret_type = rt;
	}
	int local_size() {
		int ans=0;
		for(map<string, l_entry *>::iterator iter = local.begin(); iter != local.end(); iter++)
		{
			int temp=iter->second->get_off();
			if(temp<ans)ans=temp;
		}
		return ans;
	}
	basicType ret_type;
	int return_offset;
	int get_offset(string name) {
		return local[name]->get_off();
	}
private:
	vector<l_entry *> params;
	map<string, l_entry *> local;
};

extern local_sym  *l_sym;

class g_entry {
public:
	g_entry(basicType t, local_sym *l) {
		type = t;
		l_sym = l;
		ret_size = sizeofbt(t);
		l_sym->setrt(t);
	}
	basicType get_type() {
		return type;
	}
	vector<typeExp> get_param_type() {
		return l_sym->get_param_type();
	}
	void dump() {
		cout<<"Return type: "<<btToString(type)<<endl;
		cout<<"Return size: "<<ret_size<<endl<<endl;
		l_sym->dump();
	}
	int local_size() {
		return l_sym->local_size();
	}
	void set_l_sym() {
		::l_sym = this->l_sym;
	}
private:
	basicType type;
	local_sym *l_sym;
	int ret_size;
};

class global_sym {
public:
	pair<bool,string> insert(string n, g_entry *g) {
		if(glo.find(n) != glo.end()) return make_pair(false,n);
		glo[n] = g;
		return make_pair(true,"");
	}
	pair<bool,basicType> present(string name) {
		if(glo.find(name) == glo.end()) return make_pair(false,cvoid);
		else return make_pair(true,glo[name]->get_type());
	}
	vector<typeExp> get_param_type(string name) {
		return glo[name]->get_param_type();
	}
	void dump_entry(string f) {
		if(glo.find(f)==glo.end())
		{
			cerr<<"Error: No entry for "<<f<<" in global symbol table"<<endl;
			return;
		}
		g_entry *entry = glo[f];
		cout<<"Name: "<<f<<endl;
		entry->dump();
	}
	void dump_all() {
		for (map<string,g_entry *>::iterator i = glo.begin(); i != glo.end(); ++i)
		{
			dump_entry(i->first);
		}
	}
	int local_size(string s) {
		return glo[s]->local_size();
	}
	void set_l_sym(string name) {
		if(glo.find(name)==glo.end())
		{
			cerr<<"Error: No entry for "<<name<<" in global symbol table"<<endl;
			return;
		}
		g_entry *entry = glo[name];
		entry->set_l_sym();
	}
private:
	map<string,g_entry *> glo;
};

class abstract_astnode {
public:
	virtual void print (int n)  = 0;
	void spaces(int n)
	{
		for (int i = 0; i < n; i++)
			cout<<" ";
	}
 	virtual void generate_code() = 0;
};

class stmt_ast : public abstract_astnode {
public:
	virtual void print(int n) = 0;
	virtual void generate_code(){};
};

class exp_ast : public abstract_astnode {
public:
	list<int> truelist;
	list<int> falselist;
	string result;
	bool isImmediate;
	void load() {}
	virtual void print(int n) = 0;
	virtual void generate_code(){};
	virtual void generate_address(){};
	typeExp getType() {
		return astnode_type;
	}
protected:
	void setType(typeExp t) {
		astnode_type = t;
	}
private:
	typeExp astnode_type;
};

class empty_ast : public stmt_ast {
public:
	void generate_code() {}
	void print(int n) {
		cout<<"(Empty)";
	}
};

class block_ast : public stmt_ast {
public:
	block_ast() {}
	block_ast(block_ast *block) {
		children = block->children;
	}
	void print(int n) {
		cout<<"(Block [";
		list<stmt_ast *>::iterator iter = children.begin();
		(*iter)->print(n+8);iter++;
		for(; iter!=children.end(); iter++)
		{
			cout<<endl;spaces(n+8);
			(*iter)->print(n+8);
		}
		cout<<"])";
	}
	void add_stmt(stmt_ast *stmt) {
		children.push_back(stmt);
	}
	void generate_code() {
		for(list<stmt_ast *>::iterator i = children.begin(); i != children.end(); i++) {
			cout<<distance(i,children.begin())<<endl;
			(*i)->generate_code();
		}
	}
private:
	list<stmt_ast *> children;
};

class return_ast : public stmt_ast {		// TODO
public:
	return_ast(exp_ast *e) {
		expr = e;
	}
	void print(int n) {
		cout<<"(Return "; expr->print(0); cout<<")";
	}
	void generate_code() {
		expr->generate_code();

		code<<"\tstore"<<(l_sym->ret_type==cfloat?"f":"i")
			<<"("<<expr->result<<", ind(ebp, "<<l_sym->return_offset<<"));"<<endl;code_line++;
		code<<"\tloadi(ind(ebp), ebp);"<<endl
			<<"\tpopi(1);"<<endl
			<<"\treturn;"<<endl;code_line+=3;

		if(!expr->isImmediate) regman.free(expr->result);
	}
private:
	exp_ast *expr;
};

// Done
class cast_int_ast : public exp_ast {
public:
	cast_int_ast(exp_ast *e) {
		expr = e;
		setType(typeExp(cint));
	}
	void print(int n) {
		cout<<"(TO_INT "; expr->print(0); cout<<")";
	}
	void generate_code() {
		expr->generate_code();
		string reg = expr->result;
		if(expr->isImmediate) {
			reg = regman.allocate(expr->getType().type);
			code<<"\tmove("<<expr->result<<","<<reg<<");"<<endl;code_line++;
		}
		code<<"\tfloatToint("<<reg<<");"<<endl;code_line++;
		result=reg;
		isImmediate = false;
	}
private:
	exp_ast *expr;
};

// Done
class cast_float_ast : public exp_ast {
public:
	cast_float_ast(exp_ast *e) {
		expr = e;
		setType(typeExp(cfloat));
	}
	void print(int n) {
		cout<<"(TO_FLOAT "; expr->print(0); cout<<")";
	}
	void generate_code() {
		expr->generate_code();
		string reg = expr->result;
		if(expr->isImmediate) {
			reg = regman.allocate(expr->getType().type);
			code<<"\tmove("<<expr->result<<","<<reg<<");"<<endl;code_line++;
		}
		code<<"\tintTofloat("<<reg<<");"<<endl;code_line++;
		result = reg;
		isImmediate = false;
	}
private:
	exp_ast *expr;
};

class arr_ast : public exp_ast {			// TODO
public:
	arr_ast(){}
	arr_ast(arr_ast *e1, exp_ast *e2) {
		expr1=e1; expr2=e2;
		setType(*(e1->getType().t));
	}
	void print(int n) {
		cout<<"(ArrayRef "; expr1->print(0); cout<<" "; expr2->print(0); cout<<")";
	}
	void generate_address() {

	}
	void generate_code() {
		basicType t = getType().type;
		string reg = regman.allocate(t);
		code<<"\tload"<<((t==cint)?"i":"f")
			<<"(ind(ebp,"<<get_offset()<<"),"<<reg<<");"<<endl;code_line++;

		result = reg;
		isImmediate = false;
	}
private:
	int get_offset() {
		return 0; //l_sym->get_array();
	}
	exp_ast *expr1, *expr2;
};

class ass_ast : public stmt_ast {			// TODO
public:
	ass_ast(arr_ast *e1, exp_ast *e2) {
		expr1 = e1; expr2 = e2;
	}
	void print(int n) {
		cout<<"(ASS";cout<<((expr1->getType().type==cint)?"_INT ":"_FLOAT "); expr1->print(0); cout<<" "; expr2->print(0); cout<<")";
	}
	void generate_code() {
		cout<<"ass"<<endl;
		expr2->generate_code();
	}
private:
	exp_ast *expr1,*expr2;
};

class op_ast : public exp_ast {
public:
	op_ast(exp_ast *e1, exp_ast *e2, int op) {
		expr1 = e1; expr2 = e2; oper = op;
		if((op>1000&&op<1007)||op=='<'||op=='>')
			setType(typeExp(cint));
		else if(op=='=')
			setType(e1->getType());
		else
		{
			if(e1->getType().type==cfloat||e2->getType().type==cfloat)
				setType(typeExp(cfloat));
			else
				setType(typeExp(cint));
		}
	}
	void print(int n) {
		cout<<"(";
		switch(oper)
		{
			case 1001:
				cout<<"OR";break;
			case 1002:
				cout<<"AND";break;
			case 1003:
				cout<<"EQ_OP";break;
			case 1004:
				cout<<"NE_OP";break;
			case '<':
				cout<<"LT";break;
			case '>':
				cout<<"GT";break;
			case 1005:
				cout<<"LE_OP";break;
			case 1006:
				cout<<"GE_OP";break;
			case '+':
				cout<<"PLUS";break;
			case '-':
				cout<<"MINUS";break;
			case '*':
				cout<<"MULT";break;
			case '/':
				cout<<"DIV";break;
			case '=':
				cout<<"ASSIGN_EXP";break;
			default:
				cout<<"OP_EXP";
		}
		cout<<((expr1->getType().type==cint)?"_INT ":"_FLOAT "); expr1->print(0); cout<<" "; expr2->print(0); cout<<")";
	}
	void generate_code() {
		cout<<"BinOp "<<oper<<endl;

		if(oper == '=') {
			expr1->generate_address();
			string reg1 = expr1->result;

			expr2->generate_code();
			string reg2 = expr2->result;

			if(!expr1->isImmediate && !expr2->isImmediate) regman.prepare(reg1,reg2);

			code<<"\tstore"<<((expr2->getType().type==cint)?"i":"f")
				<<"("<<reg2<<",ind(ebp,"<<reg1<<"));"<<endl;

			if(!expr2->isImmediate) regman.free(reg2);

			result = reg2;
			isImmediate = expr2->isImmediate;

			return;
		}

		expr1->generate_code();
		string reg1 = expr1->result;

		expr2->generate_code();
		string reg2 = expr2->result;

		if(expr2->isImmediate) {
			reg2 = regman.allocate(expr2->getType().type);
			code<<"\tmove("<<expr2->result<<","<<reg2<<");"<<endl;code_line++;
		}

		if(!expr1->isImmediate) regman.prepare(reg1,reg2);

		if(oper == '+') {
			code<<"\tadd"<<((expr1->getType().type==cint)?"i":"f")
				<<"("<<reg1<<","<<reg2<<");"<<endl;code_line++;
		} else if(oper == '-') {
			code<<"\tmul"<<((expr1->getType().type==cint)?"i":"f")
				<<"(-1,"<<reg2<<");"<<endl
				<<"\tadd"<<((expr1->getType().type==cint)?"i":"f")
				<<"("<<reg1<<","<<reg2<<");"<<endl;code_line++;code_line++;
		} else if(oper == '*') {
			code<<"\tmul"<<((expr1->getType().type==cint)?"i":"f")
				<<"("<<reg1<<","<<reg2<<");"<<endl;code_line++;
		} else if(oper == '/') {
			code<<"\tdiv"<<((expr1->getType().type==cint)?"i":"f")
				<<"("<<reg1<<","<<reg2<<");"<<endl;code_line++;
		}

		if(!expr1->isImmediate) regman.free(reg1);

		result = reg2;
		isImmediate = false;
	}
private:
	exp_ast *expr1,*expr2; int oper;
};

class unop_ast : public exp_ast {
public:
	unop_ast(int op, exp_ast *e) {
		oper = op; expr = e;
		if(op=='-'||op==1000)
			setType(e->getType());
		else
			setType(cint);
	}
	void print(int n) {
		cout<<"(";
		switch(oper)
		{
			case '-':
				cout<<"UMINUS";break;
			case '!':
				cout<<"NOT";break;
			case 1000:
				cout<<"PP";break;
			default:
				cout<<"UNOP_EXP";
		}
		cout<<((expr->getType().type==cint)?"_INT ":"_FLOAT "); expr->print(0); cout<<")";
	}
	void generate_code() {
		expr->generate_code();
		string reg = expr->result;

		if(expr->isImmediate) {
			reg = regman.allocate(expr->getType().type);
			code<<"l"<<++label<<": "
				<<"move("<<reg<<","<<reg<<");"<<endl;
		}
		// if(oper == '-') {
		// 	code<<"l"<<++label<<": "
		// 		<<"mul"<<((expr->getType().type==cint)?"i":"f")
		// 		<<"(-1,"<<reg<<");"<<endl;
		// } else if(oper == '!') {

		// }
	}
private:
	exp_ast *expr; int oper;
};

class func_ast : public exp_ast {
public:
	func_ast(string s,basicType t) {
		name=s;
		setType(typeExp(t));
	}
	func_ast(func_ast *f1,string s,basicType t) {
		expr=f1->expr;
		name=s;
		setType(typeExp(t));
	}
	func_ast(exp_ast *e1) {
		expr.push_back(e1);
	}
	func_ast(func_ast *f1,exp_ast *e1) {
		expr=f1->expr;
		expr.push_back(e1);
	}
	vector<typeExp> get_param_type() {
		vector<typeExp> temp;
		for(int i=0;i<(int)(expr.size());i++)
			temp.push_back(expr[i]->getType());
		return temp;
	}
	vector<exp_ast *> get_list() {
		return expr;
	}
	void print(int n) {
		cout<<"(Function \""<<name<<"\" (";
		if(expr.empty())cout<<")";
		else
		{
			vector<exp_ast *>::iterator it=expr.begin();
			(*it)->print(0);it++;
			for(;it!=expr.end();it++)
			{cout<<" ";(*it)->print(0);}
			cout<<")";
		}
	}
	void cast(basicType t, int i)
	{
		exp_ast *castast;
		if(t == cint) {
			castast = new cast_int_ast(expr[i]);
		} else if(t == cfloat) {
			castast = new cast_float_ast(expr[i]);
		}
		expr[i] = castast;
	}
	void generate_code() {
		if(name=="printf")
		{
			for(int i=0;i<(int)(expr.size());i++)
			{
				expr[i]->generate_code();
				code<<"\tprint_"<<((expr[i]->getType().type==cint)?"int":((expr[i]->getType().type==cfloat)?"float":"string"))
					<<"("<<expr[i]->result<<");"<<endl;code_line++;
			}
		}
	}
private:
	vector<exp_ast *> expr;
	string name;
};

class funcStmt_ast : public stmt_ast {
public:
	funcStmt_ast(func_ast *f1,string s) {
		expr=f1->get_list();
		name=s;
	}
	vector<typeExp> get_param_type() {
		vector<typeExp> temp;
		for(int i=0;i<(int)(expr.size());i++) {
			temp.push_back(expr[i]->getType());
			// expr[i]->getType().print(true);
		}
		return temp;
	}
	void print(int n) {
		cout<<"(Func_Stmt \""<<name<<"\" (";
		if(expr.empty())cout<<")";
		else
		{
			vector<exp_ast *>::iterator it=expr.begin();
			(*it)->print(0);it++;
			for(;it!=expr.end();it++)
			{cout<<" ";(*it)->print(0);}
			cout<<")";
		}
	}
	void cast(basicType t, int i)
	{
		exp_ast *castast;
		if(t == cint) {
			castast = new cast_int_ast(expr[i]);
		} else if(t == cfloat) {
			castast = new cast_float_ast(expr[i]);
		}
		expr[i] = castast;
	}
	void generate_code() {
		if(name=="printf")
		{
			for(int i=0;i<(int)(expr.size());i++)
			{
				expr[i]->generate_code();
				code<<"\tprint_"<<((expr[i]->getType().type==cint)?"int":((expr[i]->getType().type==cfloat)?"float":"string"))
					<<"("<<expr[i]->result<<");"<<endl;code_line++;
			}
		}
	}
private:
	vector<exp_ast *> expr;
	string name;
};

class int_ast : public exp_ast {
public:
	int_ast(string s) {
		c = s;
		setType(typeExp(cint));
	}
	void print(int n) {
		cout<<"(IntConst "<<c<<")";
	}
	void generate_code() {
		cout<<"Int "<<c<<endl;
		result = c;
		isImmediate = true;
	}
private:
	string c;
};

class str_ast : public exp_ast {
public:
	str_ast(string s) {
		c = s;
		setType(typeExp(cstring));
	}
	void print(int n) {
		cout<<"(StringConst "<<c<<")";
	}
	void generate_code() {
		cout<<"String "<<c<<endl;
		result = c;
		isImmediate = true;
	}
private:
	string c;
};

class float_ast : public exp_ast {
public:
	float_ast(string s) {
		c = s;
		setType(typeExp(cfloat));
	}
	void print(int n) {
		cout<<"(FloatConst "<<c<<")";
	}
	void generate_code() {
		cout<<"Float "<<c<<endl;
		result = c;
		isImmediate = true;
	}
private:
	string c;
};

class iden_ast: public arr_ast {
public:
	iden_ast(string s,typeExp t) : arr_ast() {
		c=s;
		setType(t);
	}
	void print(int n) {
		cout<<"(Id \""<<c<<"\")";
	}
	void generate_address() {
		int off = get_offset();
		char offc[10];
		sprintf(offc,"%d",off);
		result = offc;
		isImmediate = true;
	}
	void generate_code() {
		basicType t = getType().type;
		string reg = regman.allocate(t);
		code<<"\tload"<<((t==cint)?"i":"f")
			<<"(ind(ebp,"<<get_offset()<<"),"<<reg<<");"<<endl;

		result = reg;
		isImmediate = false;
	}
private:
	int get_offset() {
		return l_sym->get_offset(c);
	}
	string c;
};

class if_ast : public stmt_ast {
public:
	if_ast(exp_ast *e, stmt_ast *s1, stmt_ast *s2) {
		expr=e; then=s1; els=s2;
	}
	void print(int n) {
		cout<<"(If "; expr->print(0); cout<<endl;
		spaces(n+4); then->print(n+4); cout<<endl;
		spaces(n+4); els->print(n+4); cout<<")";
	}
	void generate_code() {
		expr->generate_code();
		string reg = expr->result;
		if(expr->isImmediate)
		{
			reg = regman.allocate(expr->getType().type);
			code<<"\tmove("<<expr->result<<","<<reg<<");"<<endl;code_line++;
		}
		code<<"\tcmp"<<((expr->getType().type==cint)?"i":"f")<<"(0,"<<reg<<");"<<endl;code_line++;
		if(!expr->isImmediate)
			regman.free(reg);
		code<<"\tje(l"<<++label<<");"<<endl;code_line++;int temp=label;
		then->generate_code();
		code<<"\tj(l"<<++label<<");"<<endl;code_line++;int temp2=label;
		code<<"l"<<temp<<":";
		els->generate_code();
		code<<"l"<<temp2<<":";
	}
private:
	exp_ast *expr; stmt_ast *then, *els;
};

class for_ast : public stmt_ast {
public:
	for_ast(exp_ast *e1, exp_ast *e2, exp_ast *e3, stmt_ast *s) {
		expr1=e1; expr2=e2; expr3=e3; body=s;
	}
	void print(int n) {
		cout<<"(For "; expr1->print(0); cout<<endl;
		spaces(n+5); expr2->print(n+5); cout<<endl;
		spaces(n+5); expr3->print(n+5); cout<<endl;
		spaces(n+5); body->print(n+5); cout<<")";
	}
	void generate_code() {
		expr1->generate_code();
		if(!expr1->isImmediate)
			regman.free(expr1->result);
		int m1,m2,m3;
		code<<"l"<<(m1=++label)<<":";
		expr2->generate_code();
		string reg = expr2->result;
		if(expr2->isImmediate)
		{
			reg = regman.allocate(expr2->getType().type);
			code<<"\tmove("<<expr2->result<<","<<reg<<");"<<endl;code_line++;
		}
		code<<"\tcmp"<<((expr2->getType().type==cint)?"i":"f")<<"(0,"<<reg<<");"<<endl;code_line++;
		if(!expr2->isImmediate)
			regman.free(reg);
		code<<"\tje(l"<<(m2=++label)<<");"<<endl;code_line++;
		body->generate_code();
		expr3->generate_code();
		if(!expr2->isImmediate)
			regman.free(expr2->result);
		code<<"\tj(l"<<m1<<");"<<endl;code_line++;
		code<<"l"<<m2<<":";
	}
private:
	exp_ast *expr1, *expr2, *expr3; stmt_ast *body;
};

class while_ast : public stmt_ast {
public:
	while_ast(exp_ast *e, stmt_ast *s) {
		expr=e; body=s;
	}
	void print(int n) {
		cout<<"(While "; expr->print(0); cout<<endl;
		spaces(n+7); body->print(n+7); cout<<")";
	}
	void generate_code() {
		int m1,m2;
		code<<"l"<<(m1=++label)<<":";
		expr->generate_code();
		string reg = expr->result;
		if(expr->isImmediate)
		{
			reg = regman.allocate(expr->getType().type);
			code<<"\tmove("<<expr->result<<","<<reg<<");"<<endl;code_line++;
		}
		code<<"\tcmp"<<((expr->getType().type==cint)?"i":"f")<<"(0,"<<reg<<");"<<endl;code_line++;
		if(!expr->isImmediate)
			regman.free(reg);
		code<<"\tje(l"<<(m2=++label)<<");"<<endl;code_line++;
		body->generate_code();
		code<<"\tj(l"<<m1<<");"<<endl;code_line++;
		code<<"l"<<m2<<":";
	}
private:
	exp_ast *expr; stmt_ast *body;
};

#endif
