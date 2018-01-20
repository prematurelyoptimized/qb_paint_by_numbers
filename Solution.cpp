#include<fstream>
#include<sstream>
#include<cstring>
#include<iostream>
#include<cassert>

#include"Solution.hpp"
#include"Line.hpp"
#include"RowSlice.hpp"
#include"ColumnSlice.hpp"
#include"defs.hpp"

void enqueue(Line& line, std::stack<Line*>& queue) {
	//std::cout << "Enqueuing " << &line << '\n';
	if(~line.getEnqueued()) {
		queue.push(&line);
		line.setEnqueued(true);
	}
}

bool dequeue(std::stack<Line*>& queue) {
	Line* line = queue.top();
	//std::cout << "Dequeuing " << line << '\n';
	bool retval = line->makeDeductions();
	queue.pop();
	line->setEnqueued(false);
	return retval;
}

Solution::Solution() {
	// Do nothing
}

Solution::Solution(std::size_t num_rows, std::size_t num_columns) {
	initFields(num_rows, num_columns);
	std::vector<std::size_t> dummy;
	for(int i=0;i<num_rows;++i) {
		rows.emplace_back(new RowSlice(this,i,num_columns),dummy);
		hypothetical_rows.push_back(NULL);
	}
	for(int i=0;i<num_columns;++i) {
		columns.emplace_back(new ColumnSlice(this,i,num_rows),dummy);
		hypothetical_columns.push_back(NULL);
	}

	enqueueAll();
}

Solution::~Solution() {
	if(data) {
		delete data;
	}
}

void Solution::initFields(std::size_t num_rows, std::size_t num_columns) {
	data = new bitfield[num_rows*num_columns];
	std::fill(data,data+num_rows*num_columns,CELL_UNKNOWN);
	columns.reserve(num_columns);
	rows.reserve(num_rows);
	hypothetical_columns.reserve(num_columns);
	hypothetical_rows.reserve(num_rows);
}

void Solution::enqueueAll() {	
	for(int i=0;i<rows.size();++i) {
		enqueue(rows[i],row_queue);
	}
	for(int i=0;i<columns.size();++i) {
		enqueue(columns[i],column_queue);
	}
}
		
bitfield Solution::getValue(std::size_t row, std::size_t column) const {
	bitfield b = data[row*columns.size() + column];
	return (b | b>>shift) & SURE_VALUE_MASK;
}

bool Solution::setValue(std::size_t row, std::size_t column, bitfield value) {
	bitfield b = getValue(row,column);
	if(b != value) {
		if(b != CELL_UNKNOWN) {
			return false;
		}
		data[row*columns.size() + column] |= (value << shift);
		if(shift>0) {
			if(hypothetical_rows[row]==NULL) {
				hypothetical_rows[row] = new Line(rows[row]);
			}
			if(hypothetical_columns[column]==NULL) {
				hypothetical_columns[column] = new Line(columns[column]);
			}
			enqueue(*hypothetical_rows[row],row_queue);
			enqueue(*hypothetical_columns[column],column_queue);
		} else {
			enqueue(rows[row],row_queue);
			enqueue(columns[column],column_queue);
		}
	}
	return true;
}

bool Solution::doLineDeductions() {
	bool feasible = true;
	if(!row_queue.empty()) {
		feasible = dequeue(row_queue);
	} else if(!column_queue.empty()) {
		feasible = dequeue(column_queue);
	}
	return feasible;
}

bool Solution::solve(std::size_t numSteps) {
	bool feasible = true;
	for(int i=0;i<numSteps;++i) {
		for(;i<numSteps && (!row_queue.empty() || !column_queue.empty());++i) {
			feasible = doLineDeductions();
			if(!feasible) {
				return false;
			}
		}
		if(i >= numSteps) {
			break;
		}
		
		// Check if we are done
		bool done = true;
		for(size_t row = 0;row<rows.size();++row) {
			done &= rows[row].isComplete();
		}
		for(size_t column = 0;column<columns.size();++column) {
			done &= columns[column].isComplete();
		}
		if(done) {
			break;
		}
		
		// Select a square to guess
		std::size_t guess_row = -1;
		std::size_t guess_column = -1;
		for(size_t i = 0;i<rows.size();++i) {
			for(size_t j = 0;j<columns.size();++j) {
				if(getValue(i,j) == CELL_UNKNOWN) {
					guess_row = i;
					guess_column = j;
					break;
				}
			}
			if(guess_row != -1) break;
		}
		if(guess_row == -1 || guess_column == -1) {
			// No cells are unknown, yet we failed the done check.
			assert(false);
		}
		
		// Make hypothetical deductions assuming that the square is filled
		shift = SHIFT_HYPOTHETICAL_1;
		setValue(guess_row, guess_column, CELL_FULL);
		while(feasible && !row_queue.empty() || !column_queue.empty()) {
			feasible = doLineDeductions();
		}
		
		// If the result is infeasible, erase hypothetical deductions, set the square as empty, and continue
		if(!feasible) {
			for(size_t row=0;row<rows.size();++row) {
				if(hypothetical_rows[row]!=NULL) {
					for(size_t column=0;column<columns.size();++column) {
						std::size_t index = row*columns.size() + column;
						data[index] &= NO_HYPOTHETICALS_MASK;
					}
					delete hypothetical_rows[row];
					hypothetical_rows[row] = NULL;
				}
			}
			for(size_t column=0;column<columns.size();++column) {
				if(hypothetical_columns[column]!=NULL) {
					delete hypothetical_columns[column];
					hypothetical_columns[column] = NULL;
				}
			}
			shift = 0;
			setValue(guess_row,guess_column,CELL_EMPTY);
			continue;
		}
		
		// If the puzzle is solved, copy the hypothetical deductions into truth and return true
		done = true;
		for(size_t row = 0;row<rows.size();++row) {
			done &= hypothetical_rows[row] == NULL ? rows[row].isComplete() : hypothetical_rows[row]->isComplete();
		}
		for(size_t column = 0;column<columns.size();++column) {
			done &= hypothetical_columns[column] == NULL ? columns[column].isComplete() : hypothetical_columns[column]->isComplete();
		}
		if(done) {
			for(size_t row = 0;row<rows.size();++row) {
				if(hypothetical_rows[row] != NULL) {
					for(size_t column=0;column<columns.size();++column) {
						size_t index = row*columns.size() + column;
						data[index] |= (data[index] >> shift);
						data[index] &= NO_HYPOTHETICALS_MASK;
					}
					delete hypothetical_rows[row];
					hypothetical_rows[row] = NULL;
				}
			}
			for(size_t column = 0;column<columns.size();++column) {
				if(hypothetical_columns[column] != NULL) {
					delete hypothetical_columns[column];
					hypothetical_columns[column] = NULL;
				}
			}
			return true;
		}
		
		// Make hypothetical deductions assuming that the square is empty
		shift = SHIFT_HYPOTHETICAL_2;
		setValue(guess_row, guess_column, CELL_EMPTY);
		while(feasible && !row_queue.empty() || !column_queue.empty()) {
			feasible = doLineDeductions();
		}
		
		// If the result is infeasible, copy the other hypothetical deductions into truth and guess again
		if(!feasible) {
			for(size_t row = 0;row<rows.size();++row) {
				if(hypothetical_rows[row] != NULL) {
					for(size_t column=0;column<columns.size();++column) {
						size_t index = row*columns.size() + column;
						data[index] |= (data[index] >> SHIFT_HYPOTHETICAL_1);
						data[index] &= NO_HYPOTHETICALS_MASK;
					}
					delete hypothetical_rows[row];
					hypothetical_rows[row] = NULL;
				}
			}
			for(size_t column = 0;column<columns.size();++column) {
				if(hypothetical_columns[column] != NULL) {
					delete hypothetical_columns[column];
					hypothetical_columns[column] = NULL;
				}
			}
			continue;
		}
		
		// If the puzzle is solved, copy the hypothetical deductions into truth and return true
		done = true;
		for(size_t row = 0;row<rows.size();++row) {
			done &= hypothetical_rows[row] == NULL ? rows[row].isComplete() : hypothetical_rows[row]->isComplete();
		}
		for(size_t column = 0;column<columns.size();++column) {
			done &= hypothetical_columns[column] == NULL ? columns[column].isComplete() : hypothetical_columns[column]->isComplete();
		}
		if(done) {
			for(size_t row = 0;row<rows.size();++row) {
				if(hypothetical_rows[row] != NULL) {
					for(size_t column=0;column<columns.size();++column) {
						size_t index = row*columns.size() + column;
						data[index] |= (data[index] >> shift);
						data[index] &= NO_HYPOTHETICALS_MASK;
					}
					delete hypothetical_rows[row];
					hypothetical_rows[row] = NULL;
				}
			}
			for(size_t column = 0;column<columns.size();++column) {
				if(hypothetical_columns[column] != NULL) {
					delete hypothetical_columns[column];
					hypothetical_columns[column] = NULL;
				}
			}
			return true;
		}
		
		// Erase hypothetical deductions, copying equal ones into truth, and guess again
		for(size_t row = 0;row<rows.size();++row) {
			if(hypothetical_rows[row] != NULL) {
				for(size_t column=0;column<columns.size();++column) {
					size_t index = row*columns.size() + column;
					if((data[index] >> SHIFT_HYPOTHETICAL_1) & NO_HYPOTHETICALS_MASK == (data[index] >> SHIFT_HYPOTHETICAL_2) & NO_HYPOTHETICALS_MASK) {
						data[index] |= (data[index] >> SHIFT_HYPOTHETICAL_1);
					}
					data[index] &= NO_HYPOTHETICALS_MASK;
				}
				delete hypothetical_rows[row];
				hypothetical_rows[row] = NULL;
			}
		}
		for(size_t column = 0;column<columns.size();++column) {
			if(hypothetical_columns[column] != NULL) {
				delete hypothetical_columns[column];
				hypothetical_columns[column] = NULL;
			}
		}
	}
	return feasible;
}

bitfield* const Solution::getResult() {
	return data;
}

size_t const Solution::getNumRows() {
	return rows.size();
}

size_t const Solution::getNumColumns() {
	return columns.size();
}

bool const Solution::isSolved() {
	return is_solved;
}

std::vector<std::size_t> readInts(std::string& buffer) {
	std::istringstream bufferStream(buffer);
	size_t n;
	std::vector<std::size_t> retval;
	while(bufferStream >> n) {
		retval.push_back(n);
	}
	return retval;
}

Solution Solution::readFromFile(std::string filename) {
	std::ifstream ifs;
	ifs.open(filename.c_str(),std::ifstream::in);
	
	std::size_t numRows;
	std::size_t numColumns;
	
	std::string buffer;
	getline(ifs,buffer);
	std::istringstream(buffer) >> numRows >> numColumns;
	
	Solution retval;
	retval.initFields(numRows,numColumns);
	
	for(int i=0;i<numRows;++i) {
		getline(ifs,buffer);
		std::vector<std::size_t> stretches = readInts(buffer);
		retval.rows.emplace_back(new RowSlice(&retval,i,numColumns),stretches);
	}
	for(int i=0;i<numColumns;++i) {
		getline(ifs,buffer);
		std::vector<std::size_t> stretches = readInts(buffer);
		retval.columns.emplace_back(new ColumnSlice(&retval,i,numRows),stretches);
	}
	
	retval.enqueueAll();
	
	return retval;
}

