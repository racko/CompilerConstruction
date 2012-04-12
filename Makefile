OBJS = DFA.o NFA.o Regex.o nfaBuilder.o BitSet.o HashSet.o

SRCS = $(OBJS:%.o=%.cpp)
OBJS_AND_TARGETS = $(OBJS) main.o pokerMain.o
DDS = $(OBJS_AND_TARGETS:%.o=%.dd)

CPPFLAGS = -O3 -flto -std=gnu++11 -Wall

x.exe: main.o $(OBJS)
	g++ $^ -O3 -flto -o $@

.PHONY: poker
poker: poker.exe
poker.exe: pokerMain.o $(OBJS)
	g++ $^ -O3 -flto -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

.PHONY: clean
clean:
	rm -f x.exe poker.exe $(OBJS_AND_TARGETS) $(DDS)

%.dd: %.cpp
	rm -f $@ && \
	g++ -MM $(CPPFLAGS) $< > $@.$$$$ && \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@ && \
	rm -f $@.$$$$

include $(DDS)
