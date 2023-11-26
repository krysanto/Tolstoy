all: tolstoy unit_tests

.outputFolder:
	mkdir -p out

tolstoy: .outputFolder
	g++ -std=c++20 -lm tolstoy.cpp -Wall -Wextra -Werror -o out/tolstoy
	./out/tolstoy

unit_tests: .outputFolder
	g++ -std=c++20 -lm unit_tests.cpp -Wall -Wextra -Werror -o out/unit_tests
	./out/unit_tests
