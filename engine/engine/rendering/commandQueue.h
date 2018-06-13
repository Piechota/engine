#pragma once

class CCommandQueue
{
private:
	ID3D12CommandQueue*		m_pCommandQueue;
	ID3D12Fence*			m_fence;
	HANDLE					m_fenceEvent;
	UINT					m_fenceValue;

public:
	void Init( ID3D12Device* pDevice, LPCWSTR name, D3D12_COMMAND_LIST_TYPE const type )
	{
		D3D12_COMMAND_QUEUE_DESC descCQ = {};
		descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		descCQ.Type = type;
		CheckResult(pDevice->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_pCommandQueue)));
		m_pCommandQueue->SetName(name);

		m_fenceValue = 0;
		CheckResult(pDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		++m_fenceValue;
		m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	}

	void ExecuteCommandLists( UINT const NumCommandLists, ID3D12CommandList *const *ppCommandLists )
	{
		m_pCommandQueue->ExecuteCommandLists( NumCommandLists, ppCommandLists );
	}

	void Signal()
	{
		m_pCommandQueue->Signal( m_fence, m_fenceValue );
	}

	void WaitGPU( CCommandQueue& other )
	{
		other.Signal();
		m_pCommandQueue->Wait( other.m_fence, other.m_fenceValue );
		++other.m_fenceValue;
	}

	void WaitCPU()
	{
		CheckResult(m_pCommandQueue->Signal(m_fence, m_fenceValue));
		CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
		++m_fenceValue;
	}

	ID3D12CommandQueue* GetCommandQueue()
	{
		return m_pCommandQueue;
	}

	void Release()
	{
		UINT64 const endFenceValue = m_fence->GetCompletedValue() + 1;
		CheckResult(m_pCommandQueue->Signal(m_fence, endFenceValue));
		CheckResult(m_fence->SetEventOnCompletion(endFenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);

		m_pCommandQueue->Release();
		m_fence->Release();
	}
};
