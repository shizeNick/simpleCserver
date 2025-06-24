H = TikTakToe.h

prog: main.o TikTakToe.o
	gcc -o prog main.o TikTakToe.o

main.o: main.c $(H)
	gcc -c main.c 

TikTakToe.o: TikTakToe.c $(H)
	gcc -c TikTakToe.c

clean:
	rm -f *.o prog
