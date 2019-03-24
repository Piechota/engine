#pragma once

struct SComponentTransform
{
	Quaternion m_rotation;
	Vec3 m_position;
	Vec3 m_scale;
};
POD_TYPE( SComponentTransform )

namespace ComponentTransformManager
{
	DECLARE_COMPONENT(SComponentTransform)
}
