#ifndef _circularbuffer__hpp_
#define _circularbuffer__hpp_

/**************************************************************************

CircularBuffer.hpp

Created by Matthew Clarke

A circular buffer

**************************************************************************/

#include <vector>

namespace i86 {

namespace util {

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
	CircularBuffer() : buffer(5) {}

	CircularBuffer(size_t size) : buffer(size) {}

	T& at(size_t index) {
		if (index >= buffer.size()) {
			throw std::out_of_range("Index " + std::to_string(index) + " out of bounds (size " + std::to_string(buffer.size()) + ")");
		}
		return buffer[(index + nextIndex + 1) % buffer.size()];
	}

	T& operator[](size_t index) {
		return at(index);
	}

	void push(T element) {
		buffer[nextIndex] = element;
		decrementIndex();
	}

	T pop() {
		incrementIndex();
		return buffer[nextIndex];
	}

	size_t size() {
		return buffer.size();
	}

	operator std::vector<T>() const {
		std::vector<T> ret;
		for (size_t i = 0; i < size(); i++) {
			ret.push_back(at(i));
		}
		return ret;
	}

};

}

}

#endif