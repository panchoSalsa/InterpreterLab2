CC=gcc
CXX=g++
CPPFLAGS=-g -Wall
LDFLAGS=-g
LDLIBS=

# add any addition source files here (space seperated)
SRCS=INTERPRETER.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all: INTERPRETER

INTERPRETER: $(OBJS)
	$(CXX) $(LDFLAGS) -o INTERPRETER $(OBJS) $(LDLIBS)

# dependency rules go here, if needed

clean:
	$(RM) $(OBJS) INTERPRETER
