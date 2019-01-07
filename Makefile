CXX=g++
RM=rm -f
CXXFLAGS=-O3

SRCS=better.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: better clean

better: $(OBJS)
	$(CXX) $(CXXFLAGS) -o better $(OBJS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) better
