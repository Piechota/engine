#pragma once
struct SDescriptorHeap : public D3D12_DESCRIPTOR_HEAP_DESC
{
	ID3D12DescriptorHeap* m_pDescriptorHeap;

	void Release()
	{
		if ( m_pDescriptorHeap )
		{
			m_pDescriptorHeap->Release();
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptor( UINT const offset )
	{
		ASSERT( m_pDescriptorHeap );

		D3D12_CPU_DESCRIPTOR_HANDLE descHandle = m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descHandle.ptr += offset * GDescriptorHandleIncrementSize[ Type ];

		return descHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptor( UINT const offset )
	{
		ASSERT( m_pDescriptorHeap );

		D3D12_GPU_DESCRIPTOR_HANDLE descHandle = m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		descHandle.ptr += offset * GDescriptorHandleIncrementSize[ Type ];

		return descHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorScaled( UINT const offset )
	{
		ASSERT( m_pDescriptorHeap );

		D3D12_CPU_DESCRIPTOR_HANDLE descHandle = m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		descHandle.ptr += offset;

		return descHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorScaled( UINT const offset )
	{
		ASSERT( m_pDescriptorHeap );

		D3D12_GPU_DESCRIPTOR_HANDLE descHandle = m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		descHandle.ptr += offset;

		return descHandle;
	}
};
