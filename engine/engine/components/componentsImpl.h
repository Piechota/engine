#pragma once

TArray<COMP_TYPE> m_components;
TArray<UINT32> m_freeComponents;

SComponentHandle AddComponent()
{
	SComponentHandle handle;
	handle.m_type = COMP_ID;
	handle.m_index = m_components.Size();
	if (m_freeComponents.Size() == 0)
	{
		m_components.Add();
	}
	else
	{
		handle.m_index = m_freeComponents[m_freeComponents.Size() - 1];
		m_freeComponents.EraseBack();
		IsPOD< COMP_TYPE >::Create(&m_components[handle.m_index]);
	}

	return handle;
}

void RemoveComponent(SComponentHandle const handle)
{
	ASSERT(handle.m_type == COMP_ID);
	ASSERT(handle.m_index < m_components.Size());
	m_freeComponents.Add(handle.m_index);
	if (!IsPOD< COMP_TYPE >::value)
	{
		m_components[handle.m_index].~COMP_TYPE();
	}
}

FORCE_INLINE COMP_TYPE& GetComponent(SComponentHandle const handle)
{
	ASSERT(handle.m_type == COMP_ID);
	return m_components[handle.m_index];
}

FORCE_INLINE COMP_TYPE& GetComponentNoCheck(UINT const id)
{
	return m_components[id];
}
