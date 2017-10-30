all:
	cc -std=c99 -O2 -Wall -Wextra -Werror test.c tlibmp.c -o test
