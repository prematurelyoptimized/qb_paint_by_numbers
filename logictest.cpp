#include<iostream>

#include"defs.hpp"
#include"Solution.hpp"
#include"MockSlice.hpp"

int main(void) {
	uint8_t data[] = {CELL_FULL,		// 0
					  CELL_FULL,		// 1
					  CELL_FULL,
					  CELL_FULL,
					  CELL_FULL,
					  CELL_FULL,		// 5
					  CELL_EMPTY,
					  CELL_EMPTY,
					  CELL_EMPTY,
					  CELL_EMPTY,
					  CELL_EMPTY,		// 10
					  CELL_EMPTY,
					  CELL_EMPTY,
					  CELL_EMPTY,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,		// 15
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,		// 20
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,		// 25
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_EMPTY,
					  CELL_EMPTY,		// 30
					  CELL_FULL,
					  CELL_FULL,
					  CELL_FULL,
					  CELL_FULL,
					  CELL_FULL,		// 35
					  CELL_EMPTY,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_EMPTY,
					  CELL_EMPTY,		// 40
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,		// 45
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN,
					  CELL_UNKNOWN};	// 49
	std::size_t length = 50;
	std::vector<std::size_t> stretches;
	stretches.push_back(6);
	stretches.push_back(2);
	stretches.push_back(3);
	stretches.push_back(5);
	stretches.push_back(6);
	
	Slice* s = new MockSlice(data,length);
	Line l(s,stretches);
	l.makeDeductions();
}
