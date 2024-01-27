SRC := writer.c
TARGET = writer
OBJS := $(SRC:.c=.o)

# Cross compile: make CROSS_COMPILE=aarch64-none-linux-gnu-
CC=$(CROSS_COMPILE)gcc

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	-rm -f *.o $(TARGET)
