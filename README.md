# StaticFunctor
A C++ functor that does not rely on dynamic memory allocation. Use me instead of std::function if you need to take control of the memory layout of a functor in advance.

# Platforms
Tested on:

- GCC 4.8 or higher: Bare-metal ARM, mac OS, Windows (mingw)
- clang: mac OS

Other compilers and older versions may work as well, as long as they support C++11.


# License
GNU Lesser General Public License version 3 or later.
