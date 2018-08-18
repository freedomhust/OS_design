sum : sum.c
	gcc -o sum sum.c `pkg-config --cflags --libs gtk+-2.0`
time : time.c
	gcc -o time time.c `pkg-config --cflags --libs gtk+-2.0`
cycle : cycle.c
	gcc -o cycle cycle.c `pkg-config --cflags --libs gtk+-2.0`
main : main.c
	gcc -o main main.c `pkg-config --cflags --libs gtk+-2.0`
clean :
	rm main sum time cycle