


#include "AbilitySystem/CC_AbilitySystemComponent.h"

#include "GameplayTags/CCTags.h"


void UCC_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	
	HandleAutoActivateAbility(AbilitySpec);
}

void UCC_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	FScopedAbilityListLock ActiveScopeLock(*this);/*可以理解为：
	创建一个技能列表锁，并告诉它要锁定当前这个 Ability System Component。
	因为 FScopedAbilityListLock 的构造函数需要的是组件引用，而不是组件指针，所以传入 *this。  创建锁后，GAS 会避免在遍历期间立即增删技能，防止数组变化导致引用或迭代器失效。*/
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())//GetActivatableAbilities() 会取得当前 ASC 中所有可激活技能的记录
	{
		HandleAutoActivateAbility(AbilitySpec);//激活所有带有CCTags::CCAbilities::ActivateOnGiven标签的能力
	}
}

void UCC_AbilitySystemComponent::HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if(!IsValid(AbilitySpec.Ability)) return;
	
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())//遍历循环能力的标签,并用Tag存起来，&是不复制，不修改，只读引用
	{
		if (Tag.MatchesTagExact(CCTags::CCAbilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);//TryActivateAbility是ue自带的，尝试激活能力    ".handle"是调用技能的编号，可以有好几种技能
		}
	}
}
