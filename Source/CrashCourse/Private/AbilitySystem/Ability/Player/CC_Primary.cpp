// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CCTags.h"



TArray<AActor*> UCC_Primary::HitBoxOverlapTest()//进行重叠测试和收集所有命中的actor
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());//用于获取当前实际执行该技能的 Actor。
	
	
	//FCollisionQueryParams：控制忽略哪些 Actor、是否返回物理材质等查询选项。
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);//忽略角色本身，避免与自己碰撞
	
	FCollisionResponseParams ResponseParams;//这次检测只关心 Pawn，忽略其他类型的物体。ERC==Engine Collision Response(ignore/overlap/block)。
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn,ECR_Block);
	
	//画Debug球
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);
	
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + Forward + FVector(0.f,0.f,HitBoxElevationOffset);

	GetWorld()->OverlapMultiByChannel(OverlapResults,HitBoxLocation,FQuat::Identity,ECC_Visibility,Sphere,QueryParams,ResponseParams);
	
	
	TArray <AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}
	
	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(OverlapResults,HitBoxLocation);
	}
	
	return ActorsHit;
}

void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)//向所有actor发送游戏事件
{
	for (AActor* HitActor : ActorsHit)
	{
		
		FGameplayEventData Payload;
		Payload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor,CCTags::Events::Enemy::HitReact,Payload);
	}
	
}

void UCC_Primary::DrawHitBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults,const FVector& HitBoxLocation) const
{
	DrawDebugSphere(GetWorld(),HitBoxLocation,HitBoxRadius,16,FColor::Red,false,3.f);
		
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(GetWorld(),DebugLocation,30.f,10,FColor::Green,false,3.f);
		}
	}	 
}
