all: water

water:
	g++ A6-Water.cpp -g -lglfw -lGLEW -lOpenGL -o water

clean:
	rm -f water
