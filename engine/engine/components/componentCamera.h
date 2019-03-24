#pragma once

struct SComponentCamera
{
	Matrix4x4 m_projectionMatrix;
	Quaternion m_rotation;
	Vec3 m_position;
};
POD_TYPE( SComponentCamera )

namespace ComponentCameraManager
{
	DECLARE_COMPONENT(SComponentCamera)
	extern void SetMainCamera(SComponentHandle const cameraHandle);

	extern Vec3 GetMainCameraPosition();
	extern Vec3 GetMainCameraForward();
	extern SComponentCamera GetMainCamera();
	extern void MainCameraTick();
	extern void SetMainProjection(Matrix4x4 const& projection);
}
