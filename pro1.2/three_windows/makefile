LDFLAGS = `pkg-config --cflags --libs gtk+-2.0`

sum : sum.c
	gcc -o sum sum.c $(LDFLAGS)
time : time.c
	gcc -o time time.c $(LDFLAGS)
cycle : cycle.c
	gcc -o cycle cycle.c $(LDFLAGS)
main : main.c
	gcc -o main main.c $(LDFLAGS)
all : sum time cycle main
clean :
	rm main sum time cycle

.PHONY: clean all 

# CC = gcc
# CFLAGS = 
# LDFLAGS = `pkg-config gtk+-2.0 --cflags --libs`

# SRCS = main.c cycle.c sum.c time.c
# TARGETS = build

# all:$(TARGETS)

# # ^ 表示所有目标文件
# $(TARGETS):$(SRCS)
# 	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

# clean:
# 	-rm -rf $(TARGETS) *~ .*swp

# .PHONY: clean all

# CC = gcc 
# LDFLAGS = `pkg-config --cflags --libs gtk+-2.0`

# files = sum.o time.o cycle.o main.o

# all: $(files)

# $(files): %.o: %.c 
# 	$(CC) -c $(LDFLAGS) $< -o $@


# clean :
# 	rm *.o 

# .PHONY: clean all 