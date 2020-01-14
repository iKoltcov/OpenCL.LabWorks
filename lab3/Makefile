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

main: createDirs
	g++ -Wall -g main.cpp -o ./bin/main $(OPTIONS)

clean:
	rm -rf ./bin/