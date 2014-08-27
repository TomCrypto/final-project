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

LDPATH           = -L$(LIBDIR)/bin
INCLUDE          = -I$(LIBDIR)/include -I$(SRCDIR)
CFLAGS           = -g -O2 -Wall -Wextra -pedantic -std=c99
CXXFLAGS         = -g -O2 -Wall -Wextra -pedantic -std=c++11
LDFLAGS          = -lm -lGL -lGLU -lglut -lstdc++ # -lAntTweakBar -lfftw3f

default: $(TARGET)

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJ)
	$(CXX) $(LDPATH) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf $(OBJDIR) $(TARGET)

