all: asmobs-test

asmobs-test: asmobs
	./asmobs -s asmobs-test.scm

asmobs: asmobs.c asmobs.x
	cc asmobs.c -lguile -o asmobs

asmobs.x: asmobs.c
	touch asmobs.x
	guile-snarf asmobs.c > asmobs.x

clean:
	rm -f asmobs asmobs.x
