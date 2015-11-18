objects = main.o SdfParser.o

test: $(objects)
	cc -o test $(objects)

main.o: SdfParser.h
SdfParser.o: SdfParser.h

.PHONY : clean
clean :
	-rm test $(objects)