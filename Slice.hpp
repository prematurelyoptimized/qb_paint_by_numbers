#ifndef INCLUDE_SLICE
#define INCLUDE_SLICE

#include<cstdint>
#include"defs.hpp"

class Solution;

class Slice {
	protected:
		Solution* parent;
		std::size_t length;
		Slice(Solution* parent_in, std::size_t length_in): parent(parent_in), length(length_in) {};
		
	public:
		virtual bitfield get(std::size_t index) const {};
		virtual bool set(std::size_t index, bitfield value) {};
		std::size_t getLength() const {return length;};
};

#endif
