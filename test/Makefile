fake: ../fake.c ../fake.h
	cc ../fake.c -g -o ../fake

run: clean fake				
	../fake 

val: clean fake
	valgrind --track-origins=yes -v ../fake

valmem: clean fake
	valgrind --leak-check=full ../fake

clean:
	rm -f ../fake hworld.o hworld report.txt
	
