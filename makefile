wafsim: wafsim.o ftl.o ssd.o mapping.o gc.o
	gcc -o wafsim wafsim.o ssd.o ftl.o mapping.o gc.o

wafsim.o: wafsim.c ftl.h
	gcc -c wafsim.c ftl.h

ftl.o: ftl.c ftl.h mapping.h
	gcc -c ftl.c ftl.h mapping.h

ssd.o: ssd.c ssd.h
	gcc -c ssd.c ssd.h

mapping.o: mapping.c mapping.h
	gcc -c mapping.c mapping.h

gc.o: gc.c gc.h
	gcc -c gc.c gc.h

clean:
	rm *.o *.gch
