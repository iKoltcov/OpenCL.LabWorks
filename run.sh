rm -rf ./bin/
mkdir -p ./bin/

cc -O3 -Wall -Werror -o ./bin/main main.c -lOpenCL

./bin/main