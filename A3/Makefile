INCFLAGS  = -I vecmath/include
INCFLAGS += -I /usr/include/GL

LINKFLAGS = -L. -lRK4 -lglut -lGL -lGLU
CFLAGS    = -g -Wall -std=c++17
CC        = g++
SRCS      = $(wildcard *.cpp)
SRCS     += $(wildcard vecmath/src/*.cpp)
OBJS      = $(SRCS:.cpp=.o)
PROG      = a3

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(OBJS) -no-pie -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(CXXFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
