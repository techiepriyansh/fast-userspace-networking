CC = gcc
CFLAGS = -fPIC -Wall -Wextra -ldl
LDFLAGS = -shared

TARGET = log_libc_net_calls.so
SRC = log_libc_net_calls.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
