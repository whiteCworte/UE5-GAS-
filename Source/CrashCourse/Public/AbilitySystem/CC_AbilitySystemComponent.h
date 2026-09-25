// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CC_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;// override不是必须的，但强烈建议写。它相当于让编译器帮你确认：“我确实覆盖了父类函数，而不是不小心新建了一个相似函数。”
	virtual void OnRep_ActivateAbilities() override;
	
private:
	void HandleAutoActivateAbility(const FGameplayAbilitySpec& AbilitySpec);//FGameplayAbilitySpec 是 GAS（Gameplay Ability System）中的一个结构体，表示“某个能力被授予给 AbilitySystemComponent 后的运行记录”。




};
