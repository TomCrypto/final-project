OUTPUT           = Project
BINDIR           = bin
LIBDIR           = lib
SRCDIR           = src
OBJDIR           = .build/obj
TARGET           = $(BINDIR)/$(OUTPUT)

HEADERS          = $(shell find $(SRCDIR) -name '*.h' -or -name '*.hpp')
SRC              = $(shell find $(SRCDIR) -name '*.c' -or -name '*.cpp')
OBJ              = $(subst $(SRCDIR),$(OBJDIR),$(SRC))
OBJ             := $(OBJ:.cpp=.cpp.o)
OBJ             := $(OBJ:.c=.c.o)

LDPATH           = -L$(LIBDIR)/bin
INCLUDE          = -I$(LIBDIR)/include -I$(SRCDIR)
CFLAGS           = -O2 -Wall -Wextra -pedantic -std=c99
CXXFLAGS         = -O2 -Wall -Wextra -pedantic -std=c++11
LDFLAGS          = -lm -lGL -lGLU -lglut -lstdc++                      \
                 # -lAntTweakBar -lfftw3f

default: $(TARGET)

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJ)
	echo $(OBJ)
	$(CXX) $(LDPATH) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.cpp.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

