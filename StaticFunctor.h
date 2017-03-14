/*

 StaticFunctor - a C++ functor that does not rely on dynamic memory allocation
 

 Copyright (C) 2013  Arvid Klang <arvid@klang.us>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#ifndef cranium_Block_h
#define cranium_Block_h

#include <string.h>
#include <stdint.h>
#include <cstddef>

#undef BLOCKS_REENTRANT

template<int, class> class StaticFunctor;

template <int size, typename Return, typename... Param>
class StaticFunctor<size, Return(Param...)> {
private:
	template<class X>
	static Return __invoke(const void* blockdata, Param... arg) {
		return (*reinterpret_cast<const X*>(blockdata))(arg...);
	}

	static Return __call_functionpointer(const void* blockdata, Param... arg) {
		auto* ptr = reinterpret_cast<Return(*)(Param...)>(blockdata);
		return ptr(arg...);
	}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wunused-parameter"
	static Return __empty(const void* blockdata, Param... arg) {
	}
#pragma GCC diagnostic pop

	typedef Return(*TrampolineType)(const void*, Param...);
	
	TrampolineType trampoline;
	uint8_t blockdata[size];

	template<int AnySize, typename AnyClass> friend class StaticFunctor;

public:
	constexpr StaticFunctor() : trampoline(__empty) { }
	
	constexpr inline Return operator()(Param... arg) const {
		return (*trampoline)(blockdata, arg...);
	}

	template<int othersize>
	inline void operator=(StaticFunctor<othersize, Return(Param...)> const &other) {
		static_assert(othersize <= size, "Cannot assign larger block");

#ifdef BLOCKS_REENTRANT
		trampoline = __empty;
#endif
		memcpy(&blockdata, &other.blockdata, othersize);
		trampoline = other.trampoline;
	}

	template<class X>
	inline void operator=(X const &other) {
		static_assert(sizeof(X) <= size, "Block handler is too large to fit!");

#ifdef BLOCKS_REENTRANT
		trampoline = __empty;
#endif
		memcpy(&blockdata, &other, sizeof(X));
		trampoline = __invoke<X>;
	}

	inline void operator=(Return(*fptr)(Param...)) {
		/* even though the signature of the function pointer matches our external interface, 
		 * we still need a trampoline to get rid of the extra void* parameter
		 */
		memcpy(&blockdata, &fptr, sizeof(fptr));
		trampoline = __call_functionpointer;
	}

	inline void operator=(TrampolineType callable) {
		/* trivial case. just use the function pointer */
		trampoline = callable;
	}

	inline void operator=(std::nullptr_t ptr) {
		(void)ptr;
		clear();
	}

	template<class X>
	inline StaticFunctor(X const &other) {
		operator=(other);
	}

	inline void clear() {
		trampoline = __empty;
	}

	inline bool isClear() const {
		return trampoline == __empty;
	}

	inline bool operator ==(std::nullptr_t ptr) const {
		(void)ptr;
		return isClear();
	}

	inline bool operator!=(std::nullptr_t ptr) const {
		(void)ptr;
		return !isClear();
	}
};
		

#endif
