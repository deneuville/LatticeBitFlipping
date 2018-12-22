CXX=g++
RM=rm -f
CXXFLAGS=-O3

SRCS=lbf.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: lbf clean

lbf: $(OBJS)
	$(CXX) $(CXXFLAGS) -o lbf $(OBJS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) lbf
