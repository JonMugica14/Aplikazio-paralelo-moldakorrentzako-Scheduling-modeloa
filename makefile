all:
	gcc	-Wall	-o	exec	src/main.c	src/Scheduler.c	src/Cores.c	-I	include