all:
	g++ -std=c++17 -Wall -Wextra -o badterm \
	    `pkg-config freetype2 --cflags` \
			badterm.cc \
			textbuffer.cc \
			console.cc \
			window.cc \
			fontrenderer.cc \
			-lX11 -lpthread \
			`pkg-config freetype2 --libs`

test: all
	./badterm


