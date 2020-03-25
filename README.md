# Vitaly's C++ Bits
A collection of small classes that I happen to keep copying and pasting from one
project to another.

## Usage
Building this little library by itself is not really feasible, and instead to
add a component to your own project, you just do:

```
add_subdirectory(FULLPATH/VcppBits/StringUtils StringUtils) # dependency of KeyFile
add_subdirectory(FULLPATH/VcppBits/KeyFile KeyFile) # second arg is arbitrary 
target_link_libraries(YOURPROJECTNAME VcppBits-StringUtils)
```

And in your code:
```
#include <VcppBits/StringUtils/StringUtils.hpp>
```

See SampleProject for full example. Note that dependencies are to be satisfied
by component user.

## Components

`KeyFile` -- simple ini-like file parser, with ability to properly handle
multiple identically-named '[section]'s

`MathUtils` -- more like a placeholder... Just here in case I need more custom
math functions in other components in the future

`Settings` -- simple class to help keep your application's (potentially
multiple) settings, stored in `KeyFile` (will be interesting to have other ways
to serialize data). Each setting tries to handle upper/lower limits for
numerics, and there also is convenient `STRING_ONE_OF` type.

`SimpleVector` -- very naive and straightforward implementation of vector-like
data structure. Can also work as a view on any piece of memory. There is also a
way to call your own callback each time vector grows, might be helpful for
optimize out some memory allocation out of your code, with no need of any other
tooling...

`StateManager` -- simple hierarchical state stack manager. Supposed to be
simple, and only manage the hierarchy - each of your `State` classes decides
what actions to take when it gets loaded, being pushed down on the stack, being
popped at the top again, or unloaded completely.

`StringUtils` -- same as `MathUtils`.

## Directory structure
Convention is to follow
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html so that
you can do #include <VcppBits/StringUtils/StringUtils.hpp> -- clearly stating
the origin of included file.
