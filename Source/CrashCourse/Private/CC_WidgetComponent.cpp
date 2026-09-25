// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CC_WidgetComponent.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/CC_BaseCharacter.h"
#include "UI/CC_AttributeWidget.h"


void UCC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	//调用初始化能力系统数据函数,为了得到下面三个 CrashCharacter,AttributeSet,AbilitySystemComponent值
	InitAbilitySystemData();
	
	if (!IsASCInitialized())//检查能力系统组件和属性集是否有效，如果无效就会绑定到角色的委托上,有效时才会广播
	{
		CrashCharacter->OnASCInitialized.AddDynamic(this,&ThisClass::OnASCInitialized);
		return;
	}
	
	InitializeAttributeDelegates();

}

void UCC_WidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<ACC_BaseCharacter>(GetOwner());	
	AttributeSet = Cast<UCC_AttributeSet>(CrashCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());
}

bool UCC_WidgetComponent::IsASCInitialized() const//检测传入指针ASC和AS是否有效
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UCC_WidgetComponent::InitializeAttributeDelegates()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		AttributeSet->OnAttributesInitialized.AddDynamic(this,&ThisClass::BindToAttributeChanges);
	}
	else
	{
		BindToAttributeChanges();
	}
}



void UCC_WidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)//回调ASC和AS,并且Cast保存对象,检查cast过后的对象是否为空指针,若无效直接return，有效转到InitializeAttributeDelegates();
{
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(ASC);//将父类 ASC 安全转换为项目自定义 ASC；类型不匹配或 ASC 为空时返回 nullptr
	AttributeSet = Cast<UCC_AttributeSet>(AS);//将父类 AS 安全转换为项目自定义 AttributeSet；类型不匹配或 AS 为空时返回 nullptr
	
	if (!IsASCInitialized()) return;
	InitializeAttributeDelegates();
}

void UCC_WidgetComponent::BindToAttributeChanges()
{
	for (const TTuple<FGameplayAttribute,FGameplayAttribute>& Pair : AttributeMap)
	{
		BindWidgetToAttributeChanges(GetUserWidgetObject(),Pair);//检查拥有的小部件对象
		
		GetUserWidgetObject()->WidgetTree->ForEachWidget([this,&Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	}
}

void UCC_WidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject,const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return;//只关注cc Attribute Widget
	if (!AttributeWidget->MatchesAttributes(Pair)) return;//只关注匹配的属性
		
	AttributeWidget->OnAttributeChange(Pair,AttributeSet.Get());//初始值
		
	//this 是指向AttributeSet的指针
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair,AttributeSet.Get());//改变的属性值

	});
}


