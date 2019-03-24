#pragma once

struct SComponentRigidbody
{
	void *bodyPtr;
};
POD_TYPE(SComponentRigidbody)

namespace ComponentRigidbodyManager
{
	DECLARE_COMPONENT(SComponentRigidbody)

	extern void RegisterPhysicsComponents(SComponentHandle const transformHandle, SComponentHandle const rigidbodyHandle);
	extern void SyncTransformsWithPhysics();
}
