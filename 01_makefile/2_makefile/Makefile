.PHONY: all_static static_lib link_static clear_static all_shared shared_lib link_shared move_default_location use_with_default_location clear_shared

CC := gcc
CFLAGS := -I./inc

all_static:
	$(CC) -c main.c -o obj_static/main.o $(CFLAGS)
	$(CC) -c src/hello.c -o obj_static/hello.o $(CFLAGS)
	$(CC) -c src/hi.c -o obj_static/hi.o $(CFLAGS)
static_lib: 
	ar rcs lib/static_lib/libstatic.a obj_static/hello.o obj_static/hi.o
link_static:
	$(CC) obj_static/main.o -Llib/static_lib -lstatic -o bin/exam_static
clear_static: 
	rm -rf obj_static/* bin/*
	rm -rf lib/static_lib/* 

all_shared:
	$(CC) -c main.c -o obj_shared/main.0 $(CFLAGS)
	$(CC) -c -fPIC src/hello.c -o obj_shared/hello.o $(CFLAGS)
	$(CC) -c -fPIC srs/hi.c -o obj_shared/hi.o $(CFLAGS)
shared_lib:
	$(CC) -shared obj_shared/hello.o obj_shared/hi.o -o lib/shared_lib/libshared.so
link_shared:
	$(CC) obj_shared/main.o -Llib/shared_lib -lshared -o bin/exam_shared
move_default_location:
	sudo mv lib/shared_lib/libshared.so /usr/lib
	sudo chmod 775 /usr/lib/libshared.so
use_with_default_location:
	bin/exam_shared
clear_shared:
	rm -rf obj_shared/* bin/*
	rm -rf lib/shared_lib/*
