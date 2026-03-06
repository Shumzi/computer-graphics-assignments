CC = g++
SRCS = $(wildcard *.cpp)
SRCS += $(wildcard vecmath/src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
PROG = a4
CFLAGS = -O2 -Wall -Wextra
INCFLAGS = -Ivecmath/include
DEPFLAGS = -MMD -MP

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEPFLAGS) $< -c -o $@ $(INCFLAGS)

clean:
	rm -f *.bak vecmath/src/*.o vecmath/src/*.d *.o *.d core.* $(PROG)

-include $(DEPS)
