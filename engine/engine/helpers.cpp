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

