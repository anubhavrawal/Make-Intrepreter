fake: fake.c fake.h
	cc fake.c -g -o fake

tmp:
	cc tmp2.c -o tmp

clean:
	rm -f fake test/hworld.o test/hworld test/report.txt