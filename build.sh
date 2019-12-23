rm -rf ./bin/
mkdir -p ./bin/

g++ -O3 -o ./bin/main main.cpp -framework OpenCL

./bin/main