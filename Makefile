CC 			= gcc
CFLAGS 	= -std=gnu99 -ggdb -Wall -Werror
OBJS 		= *.o
TARGET 	= prog
CLEAN 	= rm -rf
OBJS 		= main.o snapshot.o
SOURCES	= main.c snapshot.c

$(TARGET): $(OBJS)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
			strip $(TARGET)

clean:
		$(CLEAN) $(OBJS)
