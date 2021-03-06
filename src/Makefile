CC = gcc
RM = rm -f
VAL = valgrind
TD = cutest

CFLAGS = -O0 -g -Wall -Wextra -Wconversion -Wundef -Wshadow \
	-Waggregate-return -Wunreachable-code

MAIN = assembler.c
CFILES = alias.c error.c parser.c isa_instr.c isa_operand.c \
	isa_format.c label.c microcode.c sop2.c sopk.c sop1.c sopc.c \
	sopp.c smrd.c vop2.c vop1.c vopc.c vop3a.c vop3b.c ds.c mubuf.c \
	mtbuf.c mimg.c

TESTFILES = $(TD)/unit_tests.c $(TD)/CuTest.c $(TD)/SOP2_test.c

GCNASM = ../run/gcnasm
TESTDIR = ../test
UNITTESTS = ../run/testit

all: default

default:
	$(CC) $(CFILES) $(MAIN) $(CFLAGS) -o $(GCNASM)

tests:
	$(CC) $(CFILES) $(TESTFILES) -o $(UNITTESTS)

clean:
	$(RM) $(GCNASM) $(UNITTESTS)

memtest:
	$(VAL) --tool=memcheck --leak-check=full $(GCNASM) 				\
		$(TESTDIR)/basic_test.isa output.bin
	$(RM) output.bin
