CC = gcc
CFLAGS = -g -Wall

TARGET = user_thread_demo

OBJS = main.o gthread.o switch_context.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c gthread.h
	$(CC) $(CFLAGS) -c main.c

gthread.o: gthread.c gthread.h
	$(CC) $(CFLAGS) -c gthread.c

switch_context.o: switch_context.S
	$(CC) $(CFLAGS) -c switch_context.S

clean:
	rm -f $(TARGET) $(OBJS)