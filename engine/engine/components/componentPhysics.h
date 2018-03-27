#pragma once
struct SComponentPhysics
{
};

POD_TYPE( SComponentPhysics )

class CComponentPhysicsManager : public TComponentContainer< SComponentPhysics, EComponentType::CT_Physics>
{
private:

public:
};
