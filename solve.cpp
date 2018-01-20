#include<iostream>

#include"Solution.hpp"
#include"defs.hpp"

void display(uint8_t* result, size_t num_rows, size_t num_columns) {	
	for(std::size_t i=0;i<num_rows;++i) {
		std::cout << "  ";
		for(std::size_t j=0;j<num_columns;++j) {
			if(result[i*num_columns+j]==CELL_EMPTY) {
				std::cout << ' ';
			} else if(result[i*num_columns+j]==CELL_FULL) {
				std::cout << 'X';
			} else if(result[i*num_columns+j]==CELL_UNKNOWN) {
				std::cout << '.';
			} else {
				std::cout << '?';
			}
		}
		std::cout << '\n';
	}
}

int main(int argc, char** argv) {
	if(argc!=2) {
		std::cout << "Usage:\n";
		std::cout << "\tsolve [filename]\n";
		exit(0);
	}
	
	Solution test = Solution::readFromFile(argv[1]);
	bool success = test.solve();
	if(!success) {
		std::cout << "CONTRADICTION\n";
	}
	std::cout << '\n';
	display(test.getResult(), test.getNumRows(), test.getNumColumns());
	std::cout << '\n';
}
