#define CELL_UNKNOWN 0
#define CELL_EMPTY 1
#define CELL_FULL 2

#define SHIFT_HYPOTHETICAL_1 3
#define SHIFT_HYPOTHETICAL_2 5

#define BAD_GUESS_MASK (1<<2)
#define SURE_VALUE_MASK 0x03
#define NO_HYPOTHETICALS_MASK 0x07

#include<cstdint>
typedef uint8_t bitfield;

