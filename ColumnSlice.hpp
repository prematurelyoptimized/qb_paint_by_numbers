#ifndef INCLUDE_COLUMNSLICE
#define INCLUDE_COLUMNSLICE

#include"Slice.hpp"

class ColumnSlice : public Slice {
	private:
		std::size_t column;
		
	public:
		ColumnSlice(Solution* parent_in, std::size_t column_in, std::size_t length_in): Slice(parent_in,length_in), column(column_in) {};
		virtual uint8_t get(std::size_t index) const {return parent->getValue(index,column);};
		virtual bool set(std::size_t index, uint8_t value) {return parent->setValue(index,column,value);};
};

#endif