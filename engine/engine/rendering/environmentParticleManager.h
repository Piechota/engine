#pragma once

class CEnvironmentParticleManager
{
private:
	CShaderRes	m_particleShaderInit;

	ID3D12Resource* m_particlesGPU;

	ID3D12CommandAllocator*		m_particleCA;
	ID3D12GraphicsCommandList*	m_particleCL;

	ID3D12RootSignature*		m_particleRS;

	SCommonRenderData m_renderData;
	Matrix4x4 m_boxMatrix;
	Matrix4x4 m_viewToWorld;
	Matrix4x4 m_viewToScreen;
	Vec3 m_projectPositionOffset;
	Vec3 m_boxCenterOffset;
	Vec2 m_fade;
	UINT m_boxesNum[2];
	UINT m_particlesNum;
	float m_boxesSize;
	float m_size;
	float m_positionOffset;

private:
	void AllocateBuffers();
	void InitParticles(UINT const initParticleNum, UINT const boxesNum, float const boxesSize);
	FORCE_INLINE void InitProjectionMatrix( Vec3 const forward, UINT const boxesNum );

public:
	CEnvironmentParticleManager();

	void Init( UINT const initParticleNum, UINT const boxesNum, float const boxesSize );
	void FillRenderData();

	Matrix4x4 GetViewToWorld() const { return m_viewToWorld; }
	Matrix4x4 GetViewToScreen() const { return m_viewToScreen; }
	Vec3 GetProjectPositionOffset() const { return m_projectPositionOffset; }
	float GetPositionOffset() const { return m_positionOffset; }

	void Release();

	D3D12_GPU_VIRTUAL_ADDRESS GetParticlesBufferAddress() const;
};

extern CEnvironmentParticleManager GEnvironmentParticleManager;