#ifndef _BASE_PACKED_H
#define _BASE_PACKED_H

#ifdef _MSC_VER
#define PACKED_BEGIN __pragma(pack(push, 1))
#define PACKED_END __pragma(pack(pop))
#define PACKED
#else
#define PACKED_BEGIN
#define PACKED_END
#define PACKED __attribute__((packed))
#endif

#endif  // _BASE_PACKED_H
