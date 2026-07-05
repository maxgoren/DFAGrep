dfagrep:
	g++ --std=c++17 -c src/parse_re.cpp
	g++ --std=c++17 -c src/compile_dfa.cpp
	g++ --std=c++17 -c src/dfagrep.cpp
	g++ *.o -o dfagrep

install:
	cp dfagrep /usr/local/bin

clean:
	rm *.o
	rm dfagrep
