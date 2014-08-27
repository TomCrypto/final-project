CC = g++
CFLAGS = -g -Wall -Wextra -pedantic 
IPATH = -I/usr/X11/include -I/usr/pkg/include -Isrc
LPATH = -L/usr/X11/lib -L/usr/pkg/lib
LDPATH = -Wl,-R/usr/pkg/lib 
RM = rm
all: Ass2

Ass2: .build/obj/G308_Skeleton.o .build/obj/main.o
	$(CC) -o bin/$@ $^ -lm -lGL -lGLU -lglut $(LPATH)

.build/obj/%.o: src/%.cpp
	mkdir -p .build/obj
	$(CC) $(CFLAGS) -c -o $@ $^ $(IPATH)
	
clean:
	$(RM) -f *.o *.gch Ass2
