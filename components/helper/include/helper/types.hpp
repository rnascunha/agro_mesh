#ifndef HELPER_TYPES_HPP__
#define HELPER_TYPES_HPP__

#include <cstdlib>
#include <type_traits>

template<typename T>
constexpr T swap_byte_order(T t){
	static_assert(std::is_trivial<T>::value, "Type T must be trivial");

	union u{
		T ut;
		char ct[sizeof(T)];

		u(){}
	} un, to;
	un.ut = t;
	to.ut = 0;
	std::size_t s = sizeof(T);

	for(int i = 0; i < s; i++) to.ct[s - i - 1] = un.ct[i];

	return to.ut;
}

#endif /*HELPER_TYPES_HPP__*/
