OUTPUT           = Project
BINDIR           = bin
LIBDIR           = lib
SRCDIR           = src
OBJDIR           = .build/obj
TARGET           = $(BINDIR)/$(OUTPUT)

HEADERS          = $(shell find $(SRCDIR) -name '*.h' -or -name '*.hpp')
SRCDIRS          = $(shell find $(SRCDIR) -mindepth 1 -type d)
CXX_SRC          = $(shell find $(SRCDIR) -name '*.cpp')
C_SRC            = $(shell find $(SRCDIR) -name '*.c')

OBJDIRS          = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCDIRS)) $(OBJDIR)
CXX_OBJ          = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CXX_SRC))
C_OBJ            = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRC))

LDPATH           = -L$(LIBDIR)/bin/linux
INCLUDE          = -I$(LIBDIR)/include -I$(SRCDIR)
CFLAGS           = -O2 -Wall -Wextra -pedantic -std=c99
CXXFLAGS         = -O2 -Wall -Wextra -pedantic -std=c++11
LDFLAGS          = -lm -lGL -lGLU -lglut -lstdc++
LDFLAGS         += -lfftw3f -lfftw3f_omp -fopenmp
LDFLAGS         += -lfreeimage -lAntTweakBar

all: $(TARGET)

$(OBJDIRS):
	mkdir -p $@

$(TARGET): $(CXX_OBJ) $(C_OBJ) | dependencies
	$(CXX) $(LDPATH) $^ -o $@ $(LDFLAGS)

$(C_OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIRS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(CXX_OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

dependencies:
	cd $(LIBDIR) && make

clean:
	rm -rf $(OBJDIR) $(TARGET)

cleandeps:
	cd $(LIBDIR) && make clean

.PHONY: all dependencies clean cleandeps
