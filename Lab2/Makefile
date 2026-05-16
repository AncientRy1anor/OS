CC=gcc
CFLAGS=-Wall

all: fcfs sjf pri rr pri_rr

fcfs: driver.o list.o cpu.o fcfs.o
	$(CC) $(CFLAGS) -o fcfs driver.o list.o cpu.o fcfs.o

sjf: driver.o list.o cpu.o sjf.o
	$(CC) $(CFLAGS) -o sjf driver.o list.o cpu.o sjf.o

pri: driver.o list.o cpu.o pri.o
	$(CC) $(CFLAGS) -o pri driver.o list.o cpu.o pri.o

rr: driver.o list.o cpu.o rr.o
	$(CC) $(CFLAGS) -o rr driver.o list.o cpu.o rr.o

pri_rr: driver.o list.o cpu.o pri_rr.o
	$(CC) $(CFLAGS) -o pri_rr driver.o list.o cpu.o pri_rr.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o fcfs sjf pri rr pri_rr
