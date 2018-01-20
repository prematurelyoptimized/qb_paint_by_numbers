#ifndef INCLUDE_LINE
#define INCLUDE_LINE

#include<vector>
#include<cstdint>

#include"Slice.hpp"

class Line {
	private:
		Slice* data;
		std::size_t offset;
		std::size_t length;
		std::vector<std::size_t> stretches;
		
		bool is_enqueued;
		bitfield getValue(std::size_t index);
		bool setValue(std::size_t index, bitfield value);
		
		int compressFront();
		int compressBack();
		int packFront(std::size_t stretch, int starting_offset);
		int packBack(std::size_t stretch, int starting_offset);
		std::vector<std::size_t> computeMinima();
		std::vector<std::size_t> computeMaxima();
		
	public:
		Line(Slice* data_in, const std::vector<std::size_t>& stretches_in) : 
			data(data_in), stretches(stretches_in), offset(0), length(data_in->getLength()), is_enqueued(false) {};
		Line(const Line& other) :
			data(other.data), stretches(other.stretches), offset(other.offset), length(other.length), is_enqueued(false) {};
		virtual ~Line() {delete data;};
		
		bool makeDeductions();
		bool getEnqueued() const;
		void setEnqueued(bool value);
		bool isComplete() {return length==0;};
};

#endif
