all: debug release

debug:
	gcc receiver.c -std=c11 -O0 -g -o receive
	gcc sender.c -std=c11 -O0 -g -o send

release:
	gcc receiver.c -std=c11 -o receiver
	gcc sender.c -std=c11 -o sender
