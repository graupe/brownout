#
# 2015-11-18 15:12
#

SRCS = sct.c
DEPS = $(SRCS:.c=.d)
CFLAGS = -g #-O0
-include $(DEPS)

.PHONY: all clean

all: sct
clean:
	-@rm -f example


sct: $(SRCS)
	$(CC) -std=c99 -O2 -I /usr/X11R6/include -o sct sct.c -L /usr/X11R6/lib -lm -lX11 -lXrandr

