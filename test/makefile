
test.exe:test.cpp
	g++ -g -std=c++11 $^ -o $@ -lpthread

.PHONY:clean
clean:
	rm -rf test.exe
	rm -rf ./test_logs/*