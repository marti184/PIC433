# to compile, type "make rxuse" or "make txuse"

all:
	make rxuse && make txuse

rxuse: rxuse.o rx.o
	sdcc -mpic14 -p16f628a --use-non-free rxuse.o rx.o

rx.o: rx.c
	sdcc -mpic14 -p16f628a --use-non-free -c rx.c

rxuse.o: rxuse.c
	sdcc -mpic14 -p16f628a --use-non-free -c rxuse.c

txuse: txuse.o tx.o
	sdcc -mpic14 -p12f675 --use-non-free txuse.o tx.o

tx.o: tx.c
	sdcc -mpic14 -p12f675 --use-non-free -c tx.c

txuse.o: txuse.c
	sdcc -mpic14 -p12f675 --use-non-free -c txuse.c

clean:
	rm *.asm *.o *.lst *.cod *.hex
