BINDIR           = bin
LIBDIR           = lib
SRCDIR           = src
OBJDIR           = .build/obj
TARGET           = $(BINDIR)/Project

SRC              = $(shell find $(SRCDIR) -name '*.c' -or -name '*.cpp')
HEADERS          = $(shell find $(SRCDIR) -name '*.h' -or -name '*.hpp')
OBJ              = $(subst $(SRCDIR),$(OBJDIR),$(SRC))
OBJ             := $(OBJ:.cpp=.cpp.o)
OBJ             := $(OBJ:.c=.c.o)

INCLUDE          = -I/usr/X11/include -I/usr/pkg/include -I$(SRCDIR)
CFLAGS           = -g -O2 -Wall -Wextra -pedantic -std=c99
CXXFLAGS         = -g -O2 -Wall -Wextra -pedantic -std=c++11
LDFLAGS          = -lm -lGL -lGLU -lglut -lstdc++
LDPATH           = -L$(LIBDIR)

default: $(TARGET)

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJ)
	$(CXX) $(LDPATH) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(OBJDIR) $(TARGET)

