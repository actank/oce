#ifndef OCE_BASE_BASICTYPES_H__
#define OCE_BASE_BASICTYPES_H__

#ifdef __GNUC__
#include <stdint.h>
#else // NOT GCC
#include <boost/cstdint.hpp>
#endif

namespace xce {

#ifdef BOOST_CSTDINT_HPP

typedef boost::int8_t  int8;
typedef boost::uint8_t  uint8;

typedef boost::int16_t  int16;
typedef boost::uint16_t  uint16;

typedef boost::int32_t  int32;
typedef boost::uint32_t uint32;

typedef boost::int64_t  int64;
typedef boost::uint64_t uint64;

#else // GCC

typedef ::int8_t  int8;
typedef ::uint8_t  uint8;

typedef ::int16_t  int16;
typedef ::uint16_t  uint16;

typedef ::int32_t  int32;
typedef ::uint32_t uint32;

typedef ::int64_t  int64;
typedef ::uint64_t uint64;

#endif

}

using xce::int8;
using xce::uint8;

using xce::int16;
using xce::uint16;

using xce::int32;
using xce::uint32;

using xce::int64;
using xce::uint64;


// A macro to disallow the evil copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DISALLOW_EVIL_CONSTRUCTORS(TypeName)

#ifndef UNUSED
#define UNUSED(x) Unused(static_cast<const void *>(&x))
#define UNUSED2(x,y) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y))
#define UNUSED3(x,y,z) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z))
#define UNUSED4(x,y,z,a) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z)); Unused(static_cast<const void *>(&a))
#define UNUSED5(x,y,z,a,b) Unused(static_cast<const void *>(&x)); Unused(static_cast<const void *>(&y)); Unused(static_cast<const void *>(&z)); Unused(static_cast<const void *>(&a)); Unused(static_cast<const void *>(&b))
inline void Unused(const void *) { }
#endif // UNUSED

#endif // OCE_BASE_BASICTYPES_H__
