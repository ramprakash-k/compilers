all: lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++ --std=c++0x lex.cc parse.cc main.cc -o compile;
	./compile <test.in;
	make -f codegen

lex.cc: lex.l Scanner.ih 
	rm -f Scanner* Parser* lex.cc *~ a.out parse.cc compile
	flexc++ lex.l;
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y;
	./sedscript
#	bisonc++   --construction -V parse.y;

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -f Scanner* Parser* lex.cc *~ a.out parse.cc compile

bin: lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	rm -f Scanner* Parser* lex.cc *~ parse.cc
