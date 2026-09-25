// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Enemy/CC_HitReact.h"

void UCC_HitReact::CacheHitDirectionVectors(AActor* Instigator)
{
	AvatorForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	
	const FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	
	const FVector InstigatorLocation = Instigator->GetActorLocation();
	
	ToInstigator = InstigatorLocation - AvatarLocation;
	ToInstigator.Normalize();
}
