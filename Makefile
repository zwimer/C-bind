CC := gcc -O0 -g -Wall -Wextra

# test: bind
# 	$(CC) test.c bind.so -o test.out
main: bind
	$(CC) main.c bind.so -o main.out
bind:
	$(CC) -shared -fPIC bind.c bind_map.c bind_utilities.c -o bind.so
