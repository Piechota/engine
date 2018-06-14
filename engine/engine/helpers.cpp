#include "headers.h"

void CheckResult(HRESULT result)
{
#ifdef _DEBUG
	if (FAILED(result))
	{
		_com_error err(result);
		::OutputDebugString(err.ErrorMessage());
		ASSERT( false );
	}
#endif
}
void CheckResult(HRESULT result, ID3DBlob* errorBlob)
{
#ifdef _DEBUG
	if (FAILED(result))
	{
		_com_error err(result);
		::OutputDebugString(err.ErrorMessage());
		::OutputDebugStringA("\n");
		::OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
		ASSERT( false );
	}
	if (errorBlob && 0 < errorBlob->GetBufferSize())
	{
		::OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
	}
	if ( errorBlob )
	{
		errorBlob->Release();
	}
#endif
}

//https://stackoverflow.com/questions/2111667/compile-time-string-hashing
static constexpr uint32_t crc32RT(const char * str, size_t idx)
{
	if ( idx == size_t( -1 ) )
	{
		return 0xFFFFFFFF;
	}
	return (crc32RT(str,idx-1) >> 8) ^ crc_table[(crc32RT(str,idx-1) ^ str[idx]) & 0x000000FF];
}

uint32_t HashRT( char const* str )
{
	return crc32RT( str, strlen(str) - 1) ^ 0xFFFFFFFF;
}