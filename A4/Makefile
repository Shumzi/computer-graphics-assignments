CC = g++
VECMATH_SRCS = $(wildcard vecmath/src/*.cpp)
SHARED_SRCS = $(filter-out main.cpp main_tests.cpp, $(wildcard *.cpp)) $(VECMATH_SRCS)
SRCS = $(SHARED_SRCS) main.cpp
TEST_SRCS = $(SHARED_SRCS) main_tests.cpp
OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d) $(TEST_SRCS:.cpp=.d)
PROG = a4
TEST_PROG = a4_test
CFLAGS = -O2 -Wall -Wextra
INCFLAGS = -Ivecmath/include
DEPFLAGS = -MMD -MP

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

$(TEST_PROG): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $@ $(LINKFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(DEPFLAGS) $< -c -o $@ $(INCFLAGS)

clean:
	rm -f *.bak vecmath/src/*.o vecmath/src/*.d *.o *.d core.* $(PROG) $(TEST_PROG)

-include $(DEPS)
