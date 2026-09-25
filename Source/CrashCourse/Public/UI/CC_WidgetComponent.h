// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "CC_WidgetComponent.generated.h"

class ACC_BaseCharacter;
class UCC_AbilitySystemComponent;
class UCC_AttributeSet;
class UAbilitySystemComponent;



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

protected:
	virtual void BeginPlay() override;
	
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute,FGameplayAttribute> AttributeMap;
private:
	//UCC和ACC区别:
	TWeakObjectPtr<ACC_BaseCharacter> CrashCharacter;//TWeakObjectPtr<T>弱引用，不阻止目标对象销毁  |   TObjectPtr<T>强引用，通常用于当前对象“拥有”或必须保持存活的 UObject
	TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCC_AttributeSet> AttributeSet;
	
	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegates();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject,const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	
	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC,UAttributeSet* AS);

	UFUNCTION()
	void BindToAttributeChanges();
};
