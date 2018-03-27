#include "../headers.h"
#include "../input.h"
#include "../timer.h"

void CComponentCameraManager::MainCameraTick()
{
	static Vec2i const midPos(GWidth / 2, GHeight / 2);
	SComponentCamera& mainCamera = m_components[ m_graphicsCameraID ];

	if ( GInputManager.IsKeyDown( EKeys::K_RIGHT_MOUSE_BUTTON ) )
	{
		if ( GInputManager.KeyDownLastFrame( EKeys::K_RIGHT_MOUSE_BUTTON ) )
		{
			GInputManager.SetMousePosition( midPos );
		}

		Vec2i mouseDelta;
		GInputManager.GetMousePosition( mouseDelta );
		mouseDelta -= midPos;

		float const factor = .4f * MathConsts::DegToRad;
		mainCamera.m_rotation = Quaternion::FromAngleAxis( factor * float( mouseDelta.x ), Vec3::UP.data ) * mainCamera.m_rotation;
		mainCamera.m_rotation = mainCamera.m_rotation * Quaternion::FromAngleAxis(factor * float( mouseDelta.y ), Vec3::RIGHT.data );

		GInputManager.SetMousePosition( midPos );
	}

	Vec3 dir(0.f, 0.f, 0.f);

	if (GInputManager.IsKeyDown('W'))
	{
		dir += mainCamera.m_rotation * Vec3::FORWARD;
	}
	if (GInputManager.IsKeyDown('S'))
	{
		dir += mainCamera.m_rotation * Vec3::BACKWARD;
	}

	if (GInputManager.IsKeyDown('A'))
	{
		dir += mainCamera.m_rotation * Vec3::LEFT;
	}
	if (GInputManager.IsKeyDown('D'))
	{
		dir += mainCamera.m_rotation * Vec3::RIGHT;
	}

	if ( 0.f < dir.GetMagnitudeSq())
	{
		dir.Normalize();
		float const speed = GInputManager.IsKeyDown(K_SPACE) ? 100.f : 2.f;
		mainCamera.m_position += dir * GTimer.Delta() * speed;
	}
}

void CComponentCameraManager::SetMainProjection( Matrix4x4 const& projection )
{
	SComponentCamera& mainCamera = m_components[ m_graphicsCameraID ];
	mainCamera.m_projectionMatrix = projection;
}

Vec3 CComponentCameraManager::GetMainCameraPosition() const
{
	SComponentCamera const& mainCamera = m_components[ m_graphicsCameraID ];
	return mainCamera.m_position;
}

Vec3 CComponentCameraManager::GetMainCameraForward() const
{
	SComponentCamera const& mainCamera = m_components[ m_graphicsCameraID ];
	return mainCamera.m_rotation * Vec3::FORWARD;
}