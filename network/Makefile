objects=main.o utils.o
CFLAGS=-I/home/yyz/tiny210/data/fake_root/include -std=c99
LDFLAGS=-luv -L/home/yyz/tiny210/data/fake_root/lib 
CC=arm-linux-gcc

control : $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o control $(objects)

$(objects): %.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

.PHONY : clean
clean :
	-rm -rf control $(objects)