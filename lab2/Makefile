OS := $(shell uname)
OPTIONS:= 

ifeq ($(OS),Darwin)
	OPTIONS += -framework OpenCL
else
	OPTIONS += -l OpenCL
endif

all: main

createDirs:
	mkdir -p ./bin

main: createDirs main.c
	gcc -Wall -g main.c -o ./bin/main $(OPTIONS)

clean:
	rm -rf ./bin/