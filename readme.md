# JSON for C++  
* Simple C++ port of my [C JSON implementation](https://github.com/FoFabien/JSON).  
* WIP, might change in the future.  
  
# Usage  
Simply `#include "json.hpp"`.  
Check `main.cpp` for a minimalist example.  
  
# Unicode  
Unicode is disabled by default.  
You must define `JSON_STR_UNICODE` to enable it, and change the type of string and char used to your Unicode-compatible types (see the typedefs at the top of the header). You can even change the other types in a similar fashion, if you need to.  
The `_parse_hexa`, `_parse_key` functions and `<<` operator might need slight changes depending on how standard compliant your unicode string type is.  