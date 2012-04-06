OBJS = main.o DFA.o NFA.o Regex.o nfaBuilder.o BitSet.o
TARGET = x.exe

SRCS = $(OBJS:%.o=%.cpp)
DDS = $(OBJS:%.o=%.dd)

CPPFLAGS = -g -x c++ -std=gnu++11 -Wall

$(TARGET): $(OBJS)
	g++ $^ -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) $(DDS)

%.dd: %.cpp
	rm -f $@ && \
	g++ -MM $(CPPFLAGS) $< > $@.$$$$ && \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@ && \
	rm -f $@.$$$$

include $(DDS)
