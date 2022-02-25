TARGET = vr

CC = gcc

CFLAGS = -c -Wall -Werror=format-security
SRC = vr.c
OBJ = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<
clean:
	rm -f *.o *.obj
