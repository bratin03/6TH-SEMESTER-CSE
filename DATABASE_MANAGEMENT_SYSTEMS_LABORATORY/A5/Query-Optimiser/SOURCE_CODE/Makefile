# /**********************************************************
#                     Group Project Information:  
# -----------------------------------------------------------
#                     Group Members:
                    
#                     1. Aritra Chakraborty - 21CS10009
#                     2. Bratin Mondal - 21CS10016
#                     3. Sarika Bishnoi - 21CS10058
#                     4. Anish Datta - 21CS30006
#                     5. Somya Kumar - 21CS30050
# -----------------------------------------------------------
#         Department of Computer Science and Engineering,
#         Indian Institute of Technology Kharagpur.
# ***********************************************************/

run: test_res
	./test_res inp.txt > res.txt

test_res: lex.yy.o y.tab.o test_translator.o test_target_translator.o
	g++ lex.yy.o y.tab.o test_translator.o test_target_translator.o -o test_res

test_target_translator.o: test_target_translator.cxx test_translator.h
	g++ -c test_target_translator.cxx

test_translator.o: test_translator.cxx test_translator.h
	g++ -c test_translator.cxx

lex.yy.o: lex.yy.c
	g++ -c lex.yy.c

y.tab.o: y.tab.c
	g++ -c y.tab.c

lex.yy.c: test.l y.tab.h test_translator.h
	flex test.l

y.tab.c y.tab.h: test.y
	bison -dty test.y

clean:
	rm  test_res\
		test_translator.o\
		test_target_translator.o\
		lex.yy.*\
		y.tab.*