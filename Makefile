CC = gcc
CFLAGS = -Wall -Werror -Wextra -g -DCTEST_ENABLE
OBJS = image.o block.o testfs.o
LIB = libvvsfs.a
EXEC = testfs

all: $(EXEC)

$(EXEC): $(LIB)
	$(CC) $(CFLAGS) -o $(EXEC) testfs.c $(LIB)

$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)

%.o: %.c #makes all objects
	$(CC) $(CFLAGS) -c $< -o $@

test: $(EXEC)
	./$(EXEC)

clean:
	rm -f *.o

pristine: clean
	rm -f $(EXEC) $(LIB)