all:
	clang++ -Wc++11-extensions -std=c++14 -g ./src/*.cpp -o ./bin/run.out

clang:
	clang++ -Wc++11-extensions -std=c++14 ./src/*.cpp -o ./bin/run.out

gcc:
	g++ -std=c++14 -fdiagnostics-color src/*.cpp -o bin/run.out

analyze:
	clang++ --analyze -Xanalyzer -analyzer-output=text -Wc++11-extensions -std=c++14 ./src/*.cpp

format:
	clang-format ./src/*.cpp ./src/inc/*.hpp -i
