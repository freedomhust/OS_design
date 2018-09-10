CC = gcc
CFLAGS = -Wall
LDFLAGS = `pkg-config gtk+-2.0 --cflags --libs`

SRCS = about.c resource.c filesystem.c mainwindow.c processes.c refresh.c sysinfo.c
TARGETS = monitor

all:$(TARGETS)

# ^ 表示所有目标文件
$(TARGETS):$(SRCS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

clean:
	-rm -rf $(TARGETS) *~ .*swp

.PHONY: clean all
