#ifndef INCLUDE_ROWSLICE
#define INCLUDE_ROWSLICE

#include"Slice.hpp"

class RowSlice : public Slice {
	private:
		std::size_t row;
		
	public:
		RowSlice(Solution* parent_in, std::size_t row_in, std::size_t length_in): Slice(parent_in, length_in), row(row_in) {};
		virtual uint8_t get(std::size_t index) const {return parent->getValue(row,index);};
		virtual bool set(std::size_t index, uint8_t value) {return parent->setValue(row,index,value);};
};

#endif