#include <iostream>

#include "./RPN.hpp"

int main(
	int argc,
	const char **argv
)
{
	if (argc != 2) {
		std::cerr
			<< "Usage: "
			<< argv[0]
			<< " <expression>"
			<< std::endl;
		return 1;
	}

	RPN rpn;
	try {
		std::size_t i = 0;
		while (argv[1][i] != '\0') {
			// スペースは必ず無視する仕様とする
			if (!std::isspace(argv[1][i])) {
				rpn.processInput(argv[1][i]);
			}
			++i;
		}

		RPN::VALUE_TYPE result = rpn.getResult();
		// charでもintとして出力する
		std::cout << +result << std::endl;
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
