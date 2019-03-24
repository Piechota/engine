#include "../headers.h"
#include "componentTransform.h"

#define COMP_TYPE SComponentTransform
#define COMP_ID EComponentType::CT_Transform

namespace ComponentTransformManager
{
	#include "componentsImpl.h"
}

#undef COMP_TYPE 
#undef COMP_ID 