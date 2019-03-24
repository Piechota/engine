#pragma once

FORCE_INLINE btVector3 Vec3TobtVector3(Vec3 const& v)
{
	return btVector3(v.x, v.y, v.z);
}

FORCE_INLINE Vec3 btVector3ToVec3(btVector3 const& v)
{
	return Vec3(v.x(), v.y(), v.z());
}

FORCE_INLINE btQuaternion QuaternionTobtQuaternion(Quaternion const& q)
{
	return btQuaternion(q.x, q.y, q.z, q.w);
}

FORCE_INLINE Quaternion btQuaternionToQuaternion(btQuaternion const& q)
{
	return Quaternion( q.x(), q.y(), q.z(), q.w() );
}
