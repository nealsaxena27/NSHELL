all:
	@rm -f nsh
	gcc -o nsh nsh.c

clean:
	@rm -f nsh
