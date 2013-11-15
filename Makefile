OBJS = DFA.o NFA.o Regex.o nfaBuilder.o BitSet.o HashSet.o lrParser.o

SRCS = $(OBJS:%.o=%.cpp)
OBJS_AND_TARGETS = $(OBJS) main.o pokerMain.o
DDS = $(OBJS_AND_TARGETS:%.o=%.dd)


OPTIMIZATION = -O0
LTO = #-flto
CPPFLAGS = $(OPTIMIZATION) $(LTO) -std=gnu++11 -Wall -Wpedantic -ggdb3

x.exe: main.o $(OBJS)
	g++ $^ $(OPTIMIZATION) $(LTO) -o $@

.PHONY: poker
poker: poker.exe
poker.exe: pokerMain.o $(OBJS)
	g++ $^ $(OPTIMIZATION) $(LTO) -o $@

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

-include $(DDS)
