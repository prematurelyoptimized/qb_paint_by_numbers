#include"Line.hpp"
#include"Slice.hpp"
#include"defs.hpp"

#include<iostream>
#include<cassert>

#define PROGRESS_MADE (1<<0)
#define PROBLEM_FEASIBLE (1<<1)

int Line::compressFront() {
	int retval = 0;
	// If the first several cells are known to be empty, 
	// just compress the range under consideration
	while(length>0 && getValue(0)==CELL_EMPTY) {
		++offset;
		--length;
	}
	
	// If even the first stretch doesn't fit, the problem
	// is infeasible
	if(stretches[0]>length) {
		return retval;
	}
	
	// If the first cell is known to be full, fill in 
	// that stretch and then compress the range
	if(getValue(0)==CELL_FULL) {
		for(std::size_t j=0;j<stretches[0];++j) {
			if(!setValue(j,CELL_FULL)) {
				return retval;
			}
		}
		offset += stretches[0];
		length -= stretches[0];
		stretches.erase(stretches.begin());
		retval = PROGRESS_MADE;
		if(length>0) {
			if(!setValue(0,CELL_EMPTY)) {
				return retval;
			}
			++offset;
			--length;
		} else {
			return retval | (stretches.empty() ? PROBLEM_FEASIBLE : 0);
		}
	}
	
	// If putting the first stretch first would butt up against
	// another stretch, it can't go first.
	if(!stretches.empty() && length>stretches[0] && getValue(stretches[0])==CELL_FULL) {
		if(!setValue(0,CELL_EMPTY)) {
			return retval;
		}
		++offset;
		--length;
		retval = PROGRESS_MADE;
	}
	
	return retval | PROBLEM_FEASIBLE;
}

int Line::compressBack() {
	int retval = 0;
	// If the last several cells are known to be empty, 
	// just compress the range under consideration
	while(length>0 && getValue(length-1)==CELL_EMPTY) {
		--length;
	}
	
	// If even the last stretch doesn't fit, the problem
	// is infeasible
	if(stretches[stretches.size()-1]>length) {
		return retval;
	}
	
	// If the last cell is known to be full, fill in 
	// that stretch and then compress the range
	if(getValue(length-1)==CELL_FULL) {
		for(std::size_t j=0;j<stretches[stretches.size()-1];++j) {
			if(!setValue(length-1-j,CELL_FULL)) {
				return retval;
			}
		}
		length -= stretches[stretches.size()-1];
		stretches.pop_back();
		retval = PROGRESS_MADE;
		if(length>0) {
			if(!setValue(length-1,CELL_EMPTY)) {
				return retval;
			}
			--length;
		} else {
			return retval | (stretches.empty() ? PROBLEM_FEASIBLE : 0);
		}
	}
	
	// If putting the last stretch last would butt up against
	// another stretch, it can't go last.
	if(!stretches.empty() && length>stretches[stretches.size()-1] && getValue(length-1-stretches[stretches.size()-1])==CELL_FULL) {
		if(!setValue(length-1,CELL_EMPTY)) {
			return retval;
		}
		--length;
		retval = PROGRESS_MADE;
	}
	
	return retval | PROBLEM_FEASIBLE;
}

int Line::packFront(std::size_t stretch, int starting_offset) {
	bool placeable = false;
	int current_offset = starting_offset;
	if(current_offset>0) {
		while(getValue(current_offset-1)==CELL_FULL) {
			++current_offset;
		}
	}
	while(!placeable && current_offset+stretch<=length) {
		if(current_offset+stretch<length && getValue(current_offset+stretch)==CELL_FULL) {
			++current_offset;
			continue;
		}
		placeable = true;
		for(std::size_t j=0;j<stretch;++j) {
			if(getValue(current_offset+j)==CELL_EMPTY) {
				current_offset += j+1;
				placeable = false;
				break;
			}
		}
	}
	if(!placeable) {
		// Some stretch does not fit -> Problem is infeasible
		return -1;
	}
	return current_offset;
}

int Line::packBack(std::size_t stretch, int starting_offset) {
	bool placeable = false;
	int current_offset = starting_offset;
	if(current_offset<length-1) {
		while(getValue(current_offset+1)==CELL_FULL) {
			--current_offset;
		}
	}
	while(!placeable && current_offset-stretch+1>=0) {
		if(current_offset-(int)stretch>=0 && getValue(current_offset-stretch)==CELL_FULL) {
			--current_offset;
			continue;
		}
		placeable = true;
		for(std::size_t j=0;j<stretch;++j) {
			if(getValue(current_offset-j)==CELL_EMPTY) {
				current_offset -= (j+1);
				placeable = false;
				break;
			}
		}
	}
	if(!placeable) {
		// Some stretch does not fit -> Problem is infeasible
		return false;
	}
	return current_offset;
}

std::vector<std::size_t> Line::computeMinima() {
	std::vector<std::size_t> minima;
	minima.resize(stretches.size());
	int current_offset = 0;
	for(std::size_t i=0;i<stretches.size();++i) {
		current_offset = packFront(stretches[i],current_offset);
		if(current_offset < 0) {	// Negative return value means problem is infeasible
			minima.clear();
			return minima;
		}
		minima[i] = current_offset;
		current_offset += stretches[i]+1;
	}
	
	// At this point, everything is packed as far to the front as
	// possible.  However, there may be filled in cells that would
	// be missed by such a packing.  Now, we do a backwards pass
	// to pick those up.
	int current_stretch = stretches.size()-1;
	for(int i=length-1;i>=0;--i) {
		if(current_stretch<0) {
			if(!setValue(i,CELL_EMPTY)) {
				minima.clear();
				return minima;
			}
		}	
		if(i==minima[current_stretch]+stretches[current_stretch]-1) {
			i = minima[current_stretch];
			--current_stretch;
			continue;
		}
		if(getValue(i)==CELL_FULL) {
			int offset = packFront(stretches[current_stretch],i-stretches[current_stretch]+1);
			if(offset != minima[current_stretch]) {
				minima[current_stretch] = offset;
				i = offset;
				--current_stretch;
			}
		}
	}
	
	return minima;
}

std::vector<std::size_t> Line::computeMaxima() {
	std::vector<std::size_t> maxima;
	maxima.resize(stretches.size());
	int current_offset = length-1;
	for(int i=stretches.size()-1;i>=0;--i) {
		current_offset = packBack(stretches[i],current_offset);
		if(current_offset < 0) {	// Negative return value means problem is infeasible
			maxima.clear();
			return maxima;
		}
		maxima[i] = current_offset;
		current_offset -= stretches[i]+1;
	}
		
	// At this point, everything is packed as far to the back as
	// possible.  However, there may be filled in cells that would
	// be missed by such a packing.  Now, we do a forwards pass to
	// pick those up.
	int current_stretch = 0;
	for(int i=0;i<length;++i) {
		if(current_stretch>=(int)stretches.size()) {
			if(!setValue(i,CELL_EMPTY)) {
				maxima.clear();
				return maxima;
			}
		}	
		if(i==maxima[current_stretch]+1-stretches[current_stretch]) {
			i = maxima[current_stretch];
			++current_stretch;
			continue;
		}
		if(getValue(i)==CELL_FULL) {
			int offset = packBack(stretches[current_stretch],i+stretches[current_stretch]-1);
			if(offset != maxima[current_stretch]) {
				maxima[current_stretch] = offset;
				i = offset;
				++current_stretch;
			}
		}
	}
	
	return maxima;
}

bool Line::makeDeductions() {
	// All the work goes here
	
	// If there are no filled cells in this column, fill every
	// cell with empty
	if(stretches.empty()) {
		for(std::size_t i=0;i<length;++i) {
			if(!setValue(i,CELL_EMPTY)) {
				return false;
			}
		}
		return true;
	}
	
	int returnCode;
	do {
		returnCode = compressFront();
	} while((returnCode & PROGRESS_MADE) && (returnCode & PROBLEM_FEASIBLE) && !stretches.empty());
	if(!(returnCode & PROBLEM_FEASIBLE)) {
		return false;
	}
	if(stretches.empty()) {
		return true;
	}
	
	do {
		returnCode = compressBack();
	} while((returnCode & PROGRESS_MADE) && (returnCode & PROBLEM_FEASIBLE) && !stretches.empty());
	if(!(returnCode & PROBLEM_FEASIBLE)) {
		return false;
	}
	if(stretches.empty()) {
		return true;
	}
	
	assert(!stretches.empty());
	
	// Find the first possible index of the first cell for each stretch.
	std::vector<std::size_t> minima = computeMinima();
	if(minima.empty()) {
		return false;
	}
	assert(minima.size()==stretches.size());
	
	// Find the last possible index of the last cell for each stretch
	std::vector<std::size_t> maxima = computeMaxima();
	if(maxima.empty()) {
		return false;
	}
	assert(maxima.size()==stretches.size());
	
	// Compare maxima and minima for each stretch to see if we can
	// fill any cells (also compute the length of the smallest 
	// stretch on the way)
	std::size_t min_length = -1;
	for(std::size_t i=0;i<stretches.size();++i) {
		if(maxima[i]-minima[i]+1 < stretches[i]) {
			// This stretch does not fit -> Problem is infeasible
			return false;
		}
		if(maxima[i]-minima[i]+1 == stretches[i]) {
			// This stretch just fits, so put empties on either end as well
			if(minima[i]>0 && !setValue(minima[i]-1,CELL_EMPTY)) {
				return false;
			}
			if(maxima[i]<length-1 && !setValue(maxima[i]+1,CELL_EMPTY)) {
				return false;
			}
		}
		for(std::size_t j=maxima[i]+1-stretches[i];j<minima[i]+stretches[i];++j) {
			// Fill any cells that must be a part of this stretch
			if(!setValue(j,CELL_FULL)) {
				return false;
			}
		}
		if(i<stretches.size()-1) {
			for(std::size_t j=maxima[i]+1;j<minima[i+1];++j) {
				// Mark as empty all cells that must lie in the gap between 
				// this stretch and the next
				if(!setValue(j,CELL_EMPTY)) {
					return false;
				}
			}
		}
		if(stretches[i]<min_length) {
			min_length = stretches[i];
		}
	}
	
	// If there are any gaps between empties that are shorter than the shortest
	// stretch, then the whole gap must be empty
	std::size_t one_past_last_empty = 0;	// We can consider the cell at index -1 to be empty
	for(std::size_t i=0;i<length;++i) {
		if(getValue(i)==CELL_EMPTY) {
			if(i<one_past_last_empty+min_length) {
				for(std::size_t j=one_past_last_empty;j<i;++j) {
					if(!setValue(j,CELL_EMPTY)) {
						return false;
					}
				}
			}
			one_past_last_empty = i+1;
		}
	}
	
	return true;
}

bitfield Line::getValue(std::size_t index) {
	assert(index<length);
	return data->get(offset+index);
}

bool Line::setValue(std::size_t index, bitfield value) {
	assert(index<length);
	return data->set(offset+index,value);
}
		
bool Line::getEnqueued() const {
	return is_enqueued;
}

void Line::setEnqueued(bool value) {
	is_enqueued = value;
}
