#pragma once

#include "../core.h"

struct SSphere
{
	Vec3	m_position;
	float	m_radius;
};

struct SBox
{
	Quaternion m_rotation;
	Vec3	m_position;
	Vec3	m_sides;
};

struct SHitInfo
{
	Vec3 m_contactPoint;
};