.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c
	gcc -o $@ $^

.PHONY: clean
clean: 
	rm -f scheduler.out

.PHONY: run
run: scheduler.out
	./scheduler.out