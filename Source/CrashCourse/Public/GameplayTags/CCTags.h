#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

//创建UE原生标签
namespace CCTags
{
	namespace CCAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ActivateOnGiven);

		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tertiary);
	}
	
	namespace Events
	{
		namespace Enemy
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		}
	}
}
