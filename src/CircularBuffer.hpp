#pragma once

/**************************************************************************

CircularBuffer.hpp

Created by Matthew Clarke

A circular buffer

 **************************************************************************/

#include <vector>

namespace icarus {

	template <class T>
	class CircularBuffer {
	private:
		std::vector<T> buffer;
		size_t nextIndex = 0;

		void incrementIndex() {
			nextIndex++;
			if (nextIndex >= buffer.size()) {
				nextIndex -= buffer.size();
			}
		}

		void decrementIndex() {
			if (nextIndex == 0) {
				nextIndex = buffer.size() - 1;
			}
			else {
				nextIndex--;
			}
		}

	public:
		CircularBuffer() : buffer(5) {

		}

		CircularBuffer(size_t size) : buffer(size) {
			nextIndex = 0;
		}

		T& operator[](size_t index) {
			if (index >= buffer.size()) {
				return buffer[0];
			}
			return buffer[(index + nextIndex + 1) % buffer.size()];
		}

		void push(T element) {
			buffer[nextIndex] = element;
			decrementIndex();
		}

		size_t size() {
			return buffer.size();
		}

	};

}