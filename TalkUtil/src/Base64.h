#ifndef BASE64_H__
#define BASE64_H__

#include <string>

namespace com {
namespace xiaonei {
namespace talk {
namespace util{


class Base64 {

public:
	/**
	 * Base64-encodes the input according to RFC 3548.
	 * @param input The data to encode.
	 * @return The encoded string.
	 */
	static const std::string encode64(const std::string& input);

	/**
	 * Base64-decodes the input according to RFC 3548.
	 * @param input The encoded data.
	 * @return The decoded data.
	 */
	static const std::string decode64(const std::string& input);

private:
	static const std::string alphabet64;
	static const std::string::size_type table64[];
	static const char pad;
	static const std::string::size_type np;
};
}
}
}
}
#endif // BASE64_H__
