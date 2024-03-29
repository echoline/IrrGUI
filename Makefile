CXXFLAGS := -I/usr/include/irrlicht/ -g
LDFLAGS := -lIrrlicht -lavcodec -lavutil -lavformat -lswscale -lopenal -lalut -g
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

irrgui: $(OBJECTS)
	g++ -o irrgui $(OBJECTS) $(LDFLAGS)

clean:
	rm -f *.o irrgui
