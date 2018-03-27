#pragma once

struct SComponentCamera
{
	Matrix4x4 m_projectionMatrix;
	Quaternion m_rotation;
	Vec3 m_position;
};
POD_TYPE( SComponentCamera )

class CComponentCameraManager : public TComponentContainer< SComponentCamera, EComponentType::CT_Camera>
{
private:
	UINT m_graphicsCameraID;

public:
	void SetMainCamera( UINT const cameraID )
	{
		m_graphicsCameraID = cameraID;
	}

	Vec3 GetMainCameraPosition() const;
	Vec3 GetMainCameraForward() const;
	SComponentCamera GetMainCamera() const { return m_components[ m_graphicsCameraID ]; }
	void MainCameraTick();
	void SetMainProjection( Matrix4x4 const& projection );
};