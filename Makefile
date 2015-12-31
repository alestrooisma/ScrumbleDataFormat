CFLAGS = -Werror -Wall -Wextra

objects = main.o SdfParser.o

test: $(objects)
	$(CC) -o test $(objects)

main.o: SdfParser.h
SdfParser.o: SdfParser.h

.PHONY : clean
clean :
	-rm test $(objects)
