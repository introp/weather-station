//! @file
//! @author Jon Dubovsky, 28 Oct 2015
//! Byte manipulation utility functions.
#ifndef AVALON_BYTEMANIP_H_
#define AVALON_BYTEMANIP_H_

#include <cstdlib>

//! Make a 32-bit dword from a high and low 16-bit word.
inline std::uint32_t makedword(std::uint16_t hi, std::uint16_t lo) {
	return (static_cast<std::uint32_t>(hi) << 16) | lo;
}

//! Get the high 16-bit word from a 32-bit dword.
inline std::uint16_t hiword(std::uint32_t dw) {
	return dw >> 16;
}

//! Get the low 16-bit word from a 32-bit dword.
inline std::uint16_t loword(std::uint32_t dw) {
	return dw & 0x0000FFFFu;
}



#endif /* AVALON_BYTEMANIP_H_ */
