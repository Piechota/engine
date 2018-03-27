#pragma once
struct SGlobalBuffer
{
	Matrix4x4 m_enviroProjection;
	Matrix4x4 m_worldToScreen;
	Vec4 m_viewToWorld[3];
	Vec4 m_perspectiveValues;
	Vec3 m_cameraPositionWS;
	float m_deltaTime;
	float m_time;
};
