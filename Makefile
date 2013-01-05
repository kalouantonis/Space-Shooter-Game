all: space_fighter.cpp
	g++ -O3 space_fighter.cpp -I. -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -o space_fighter 