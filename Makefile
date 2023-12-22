

run: main.c aes.c
	gcc main.c aes.c -o aes
	./aes

attaque: exploit
	./exploit

exploit: exploit.c aes.c
	gcc exploit.c aes.c -o exploit

clean:
	rm -f aes exploit