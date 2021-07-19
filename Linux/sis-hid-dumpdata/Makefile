CC := gcc
exe := sis-hid-dumpdata
obj := sis-hid-dumpdata.o

all:$(obj)
	$(CC) -o $(exe) $(obj)

%.o:%.c
	$(CC) -c $^ -o $@

.PHONY: clean
clean:
	rm *.o $(exe)
