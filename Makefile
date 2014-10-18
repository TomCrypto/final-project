OUTPUT           = Project
BINDIR           = bin
LIBDIR           = lib
SRCDIR           = src
RESDIR           = res
OBJDIR           = .build/obj
TARGET           = $(BINDIR)/$(OUTPUT)

LDPATH_EXTRA     = -L/usr/pkg/lib -Wl,-R/usr/pkg/lib
INCLUDE_EXTRA    = -I/usr/pkg/include

HEADERS          = $(shell find $(SRCDIR) -name '*.h' -or -name '*.hpp')
SRCDIRS          = $(shell find $(SRCDIR) -mindepth 1 -type d)
CXX_SRC          = $(shell find $(SRCDIR) -name '*.cpp')
C_SRC            = $(shell find $(SRCDIR) -name '*.c')
RESOURCES        = $(shell find $(RESDIR) -type f)

OBJDIRS          = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCDIRS)) $(OBJDIR)
CXX_OBJ          = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CXX_SRC))
C_OBJ            = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(C_SRC))
RES              = $(patsubst $(RESDIR)/%,$(BINDIR)/%,$(RESOURCES))

LDPATH           = -L$(LIBDIR)/bin/linux $(LDPATH_EXTRA)
INCLUDE          = -I$(LIBDIR)/include -I$(SRCDIR) $(INCLUDE_EXTRA)
CFLAGS           = -O3 -march=native -Wall -Wextra -pedantic -std=c99
CFLAGS          += -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS         = -O3 -march=native -Wall -Wextra -pedantic -std=c++11
CXXFLAGS        += -Wno-unused-parameter -Wno-unused-variable
CXXFLAGS        += -Wno-reorder -DGLM_FORCE_RADIANS
CXXFLAGS        += -D_ELPP_DISABLE_DEFAULT_CRASH_HANDLING
CXXFLAGS        += -D_ELPP_NO_DEFAULT_LOG_FILE
LDFLAGS          = -lm -lGLEW -lGLU -lGL -lglut -lstdc++
LDFLAGS         += -lfreeimage -lAntTweakBar -lX11
LDFLAGS         += -lfftw3f -lfftw3f_omp -fopenmp

all: $(TARGET)

$(OBJDIRS):
	mkdir -p $@

$(BINDIR)/%: $(RESDIR)/%
	mkdir -p $(dir $@) && cp $< $@

$(TARGET): $(CXX_OBJ) $(C_OBJ) | $(RES)
	$(CXX) $(LDPATH) $^ -o $@ $(LDFLAGS)

$(C_OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(OBJDIRS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(CXX_OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJDIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

deps:
	cd $(LIBDIR) && make

clean:
	rm -rf $(OBJDIR) $(BINDIR)/*

cleandeps:
	cd $(LIBDIR) && make clean

.PHONY: all deps clean cleandeps
