# You can compile with either gcc or g++
CC = g++
# CC = gcc
CFLAGS = -I. -Wall -lm -ldl -no-pie
# disable the -DNDEBUG flag for the printing the freelist
OPTFLAG = -O2

all: s-deli i-deli s-test1 s-test2 s-test3 s-test4 s-test5 s-test6 s-test7 s-test8 s-test10 s-test11 s-test12

i-deli: deli.cc
	$(CC) -o i-deli thread.o deli.cc libinterrupt.a $(CFLAGS)

s-deli: deli.cc
	$(CC) -o s-deli thread.cc deli.cc libinterrupt.a $(CFLAGS)

s-test1: test1.cc
	$(CC) -o s-test1 thread.cc test1.cc libinterrupt.a $(CFLAGS)

s-test2: test2.cc
	$(CC) -o s-test2 thread.cc test2.cc libinterrupt.a $(CFLAGS)

s-test3: test3.cc
	$(CC) -o s-test3 thread.cc test3.cc libinterrupt.a $(CFLAGS)

s-test4: test4.cc
	$(CC) -o s-test4 thread.cc test4.cc libinterrupt.a $(CFLAGS)

s-test5: test5.cc
	$(CC) -o s-test5 thread.cc test5.cc libinterrupt.a $(CFLAGS)

s-test6: test6.cc
	$(CC) -o s-test6 thread.cc test6.cc libinterrupt.a $(CFLAGS)

s-test7: test7.cc
	$(CC) -o s-test7 thread.cc test7.cc libinterrupt.a $(CFLAGS)

s-test8: test8.cc
	$(CC) -o deli thread.cc test8.cc libinterrupt.a $(CFLAGS)

s-test10: test10.cc
	$(CC) -o s-test10 thread.cc test10.cc libinterrupt.a $(CFLAGS)

s-test11: test11.cc
	$(CC) -o s-test11 thread.cc test11.cc libinterrupt.a $(CFLAGS)

s-test12: test12.cc
	$(CC) -o s-test12 thread.cc test12.cc libinterrupt.a $(CFLAGS)


clean:
	rm -f s-deli


