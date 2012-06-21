CXXFLAGS := -I/usr/include/irrlicht/ -g
LDFLAGS := -lIrrlicht -g
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

gui: $(OBJECTS)
	g++ -o gui $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o gui
