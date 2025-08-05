#Compiler and Flags
CC=gcc

#wall for warrning and Wextra for exra warrning
#02 is optimization to make our code faster
CFLAGS= -Wall -Wextra -o2

#add libs like -lpthread
LDFLAGS=-lssl -lcrypto

#project
TARGET=http_server
SRCS=main.c parse.c
OBJS=$(SRCS:.c=.o)

#defult
all:$(TARGET)

#linking
$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

#compile into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#for clean up
clean:
	rm -f $(OBJS) $(TARGET)

#runing
run:all
	./$(TARGET)
