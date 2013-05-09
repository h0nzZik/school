prog=heap
src=Heap.d main.d
objs=${src:.d=.o}
deps=${src:.d=.o}


dc=ldc2


${prog}: ${objs}
	${dc} ${libs} $^

%.o: %.d
	${dc} -c $^ of=$@

%.dep: %.d

# only a test
test:
	${dc} --run="" ${src}
clean:
	rm -f *.o
