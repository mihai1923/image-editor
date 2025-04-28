# Țucă Mihai-Laurențiu 314CA

build: image_editor

image_editor: image_editor.c
	gcc -Wall -Wextra -std=c99 image_editor.c -o image_editor -lm

pack:
	zip -FSr 314CA_TucaMihai-Laurentiu_Tema3.zip README Makefile *.c *.h

clean:
	rm -f image_editor

.PHONY: pack clean
