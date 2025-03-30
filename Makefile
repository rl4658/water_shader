all: water

water:
	g++ -std=c++11 A6-Water.cpp -g -lglfw -lGLEW -lOpenGL -o water

clean:
	rm -f water
