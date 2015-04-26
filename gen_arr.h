void arr_ast::generate_arr_address() {
	if(expr1->isIden)
		((iden_ast*)expr1)->generate_address();
	else expr1->generate_arr_address();
	string reg1 = expr1->result;
	expr2->generate_code();
	string reg2 = expr2->result;
	if(expr2->isImmediate) {
		reg2 = regman.allocate(expr2->getType().type);
		code<<"\tmove("<<expr2->result<<","<<reg2<<");"<<endl;
	}
	code<<"\tmuli("<<expr1->getType().t->size()<<","<<reg2<<");"<<endl;
	if(!expr1->isImmediate)
		regman.prepare(reg1,reg2);
	code<<"\taddi("<<reg1<<","<<reg2<<");"<<endl;
	if(!expr1->isImmediate)
		regman.free(reg1);
	result = reg2;
	isImmediate = false;
}
void arr_ast::generate_code() {
	if(isIden)
		((iden_ast*)this)->generate_iden();
	else
	{
		generate_arr_address();
		string reg = result;
		code<<"\tload"<<((expr1->getType().type==cint)?"i":"f")
			<<"(ind(ebp,"<<reg<<"),"<<reg<<");"<<endl;
		result = reg;
		isImmediate = false;
	}
}
