all: src/snake.c src/main.c
	gcc \
		-Wall -Wextra \
		-O3 -funroll-loops \
		src/main.c \
		-o build/Gluttonous_Snake.exe