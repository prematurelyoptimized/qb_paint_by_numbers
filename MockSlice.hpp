#ifndef INCLUDE_MOCKSLICE
#define INCLUDE_MOCKSLICE

#include"Slice.hpp"

class MockSlice : public Slice {
	private:
		uint8_t *data;
		
	public:
		MockSlice(uint8_t *data_in, std::size_t length_in): Slice(0,length_in), data(data_in) {};
		virtual uint8_t get(std::size_t index) const {return data[index];};
		virtual bool set(std::size_t index, uint8_t value) {
			if(data[index]==value || data[index]==CELL_UNKNOWN) {
				data[index] = value;
				return true;
			} else {
				return false;
			}
		};
};

#endif
