fake: ../fake.c
	cc ../fake.c -g -o ../fake

run: clean fake
	../fake

val: fake
	valgrind ../fake

clean:
	rm -f ../fake