#pragma once

/**************************************************************************

Bus.hpp

Created by Matthew Clarke

Bus implementation for data transfer

 **************************************************************************/

#include "../Type.hpp"

namespace icarus {

	template <class T>
	class Bus {
	private:
		T m_data;
		unsigned int m_dataBitWidth;

	public:
		Bus();

		T readData();
		void putData(T d);

		unsigned int getBusBitWidth();
	};

	typedef Bus<uint16_t> Bus16;
	typedef Bus<uint32_t> Bus32;
	typedef Bus<uint8_t> Bus8;

}