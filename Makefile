x.exe: main.o DFA.o NFA.o Regex.o
	g++ -g $^ -o $@

%.o: %.cpp
	g++ -g -x c++ -std=gnu++0x -c $<

.PHONY: clean
clean:
	-rm *.o x.exe
