void main() {
	pushi (ebp);
	move (esp,ebp);
	addi (-4,esp);
l1: move(6,eax);
l2: move(5,ebx);
l3: move(4,ecx);
l4: move(3,edx);
l5: pushi(eax);
l6: move(2,eax);
l7: addi(1,eax);
l8: addi(eax,edx);
l9: addi(edx,ecx);
l10: addi(ecx,ebx);
l11: loadi(ind(esp),eax);
popi(1);
l12: addi(ebx,eax);
	addi (4,esp);
	loadi (ind(esp),ebp);
	popi(1);
}

