#include<iostream>

#include"Solution.hpp"
#include"defs.hpp"

void display(uint8_t* result, size_t num_rows, size_t num_columns) {	
	for(std::size_t i=0;i<num_rows;++i) {
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

void test1(void) {
	Solution test = Solution::readFromFile("test1.pzl");
	std::cout << test.solve() << '\n';
	uint8_t* result = test.getResult();
	display(result, test.getNumRows(), test.getNumColumns());
}

void test2(void) {
	Solution test = Solution::readFromFile("test1.pzl");
	while(!test.isSolved()) {
		test.solve(1);
		uint8_t* result = test.getResult();
		display(result, test.getNumRows(), test.getNumColumns());
		std::cout << '\n';
	}
}

void test3(void) {
	Solution test = Solution::readFromFile("test2.pzl");
	std::cout << test.solve() << '\n';
	uint8_t* result = test.getResult();
	display(result, test.getNumRows(), test.getNumColumns());
}

int main(void) {
	test1();
	test3();
}
