#pragma once

#define DECLARE_COMPONENT(COMP_TYPE)												\
	extern SComponentHandle AddComponent();											\
	extern void RemoveComponent(SComponentHandle const handle);						\
	extern FORCE_INLINE COMP_TYPE& GetComponent(SComponentHandle const handle);		\
	extern FORCE_INLINE COMP_TYPE& GetComponentNoCheck(UINT const id);							
