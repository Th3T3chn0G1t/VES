CXX ?= clang++

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(wildcard src/include/*.hpp)

EXEC = VES.out

DEPENDENCIES = lua raylib sol2 fmt yaml-cpp
CFLAGS = $(shell pkg-config --cflags $(DEPENDENCIES))
LFLAGS = $(shell pkg-config --libs $(DEPENDENCIES))

run: $(EXEC)
	./$<

$(EXEC): $(OBJECTS)
	$(CXX) $(LFLAGS) -fuse-ld=lld -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) -c -std=c++17 $(CFLAGS) -DSOL_ALL_SAFETIES_ON -glldb -O0 -o $@ $<
	clang-format -i -style=file $<

clean:
	-rm $(OBJECTS)
	-rm $(EXEC)
