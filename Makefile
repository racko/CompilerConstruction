OBJS = DFA.o NFA.o Regex.o nfaBuilder.o BitSet.o HashSet.o

SRCS = $(OBJS:%.o=%.cpp)
DDS = $(OBJS:%.o=%.dd)

DEBUG = -g
FAST = -O3 #-flto

CPPFLAGS = $(FAST) -std=gnu++11 -Wall

x.exe: main.o $(OBJS)
	g++ $^ -o $@ $(FAST)

.PHONY: poker
poker: poker.exe
poker.exe: pokerMain.o $(OBJS)
	g++ $^ -o $@ $(FAST)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

.PHONY: clean
clean:
	rm -f x.exe poker.exe main.o pokerMain.o $(OBJS) $(DDS)

%.dd: %.cpp
	rm -f $@ && \
	g++ -MM $(CPPFLAGS) $< > $@.$$$$ && \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@ && \
	rm -f $@.$$$$

include $(DDS)
