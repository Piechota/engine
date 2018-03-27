#pragma once

typedef unsigned char		Byte;
typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed int          INT;
typedef signed __int64      INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned int        UINT;
typedef unsigned __int64    UINT64;

template<class Key, class Value>
struct TPair
{
	Key m_key;
	Value m_value;

	TPair() {}
	TPair(Key const& key, Value const& value)
		: m_key(key)
		, m_value(value)
	{}
};