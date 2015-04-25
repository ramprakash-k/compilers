void main() {
	pushi (ebp);
	move (esp,ebp);
	addi (-4,esp);
	storei(5,ind(ebp,-4));
	move(5,eax);
	intTofloat(eax);
	move(10.0,ebx);
	addf(eax,ebx);
	loadi(ind(ebp,-4),eax);
	intTofloat(eax);
	addf(ebx,eax);
	loadi(ind(ebp,-4),ebx);
	intTofloat(ebx);
	addf(eax,ebx);
	loadi(ind(ebp,-4),eax);
	intTofloat(eax);
	addf(ebx,eax);
	loadi(ind(ebp,-4),ebx);
	intTofloat(ebx);
	addf(eax,ebx);
	floatToint(ebx);
	addi (4,esp);
	loadi (ind(esp),ebp);
	popi(1);
}

