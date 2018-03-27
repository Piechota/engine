#pragma once
struct SDescriptorHeap
{
	ID3D12DescriptorHeap* m_pDescriptorHeap;
	UINT m_descriptorsNum;
	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	D3D12_DESCRIPTOR_HEAP_FLAGS m_flags;

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
		descHandle.ptr += offset * GDescriptorHandleIncrementSize[ m_type ];

		return descHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptor( UINT const offset )
	{
		ASSERT( m_pDescriptorHeap );

		D3D12_GPU_DESCRIPTOR_HANDLE descHandle = m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		descHandle.ptr += offset * GDescriptorHandleIncrementSize[ m_type ];

		return descHandle;
	}
};
