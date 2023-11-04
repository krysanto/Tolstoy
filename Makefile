all: tolstoy

.outputFolder:
	mkdir -p out

tolstoy: .outputFolder
	g++ -std=c++20 -lstdc++ -lm tolstoy.cpp -Wall -Wextra -Werror -o out/tolstoy
	./out/tolstoy
