#ifndef __UTILCXX_HASH_MAP_STRING__
#define __UTILCXX_HASH_MAP_STRING__
namespace __gnu_cxx {

template <>
struct hash<std::string> {
	std::size_t operator()(const std::string& h) const {
		return __stl_hash_string(h.c_str());
	};
};

}

#endif
