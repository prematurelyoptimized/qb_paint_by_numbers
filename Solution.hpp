#ifndef INCLUDE_SOLUTION
#define INCLUDE_SOLUTION

#include<vector>
#include<stack>
#include<cstdint>
#include<string>

#include"Line.hpp"

class Solution {
	private:
		bitfield* data;
		std::vector<Line> columns;
		std::vector<Line> rows;
		std::vector<Line*> hypothetical_columns;
		std::vector<Line*> hypothetical_rows;
		std::stack<Line*> column_queue;
		std::stack<Line*> row_queue;
		bool is_solved = false;
		std::size_t shift = 0;
		
		void initFields(std::size_t num_rows, std::size_t num_columns);
		void enqueueAll();
		bool doLineDeductions();
		
	public:
		static Solution readFromFile(std::string filename);
		Solution();
		Solution(std::size_t num_rows, std::size_t num_columns);
		~Solution();
		
		bitfield getValue(std::size_t row, std::size_t column) const;
		bool setValue(std::size_t row, std::size_t column, bitfield value);
		
		bool solve(std::size_t numSteps=-1);
		bitfield* const getResult();
		size_t const getNumRows();
		size_t const getNumColumns();
		bool const isSolved();
};

#endif
