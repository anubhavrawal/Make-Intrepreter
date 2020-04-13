fake: fake.c fake.h
	cc fake.c -g -o fake

clean:
	rm -f fake