// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CC_PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CC_PlayerState.h"



ACC_PlayerCharacter::ACC_PlayerCharacter()
{
	// 当前角色没有需要每帧执行的逻辑，因此关闭 Tick 以减少不必要的性能开销。
	PrimaryActorTick.bCanEverTick = false;
	
	// 设置角色胶囊体的碰撞尺寸：半径 42 cm，半高 96 cm。
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	
	// 不让角色身体直接跟随控制器旋转。
	// 玩家转动第三人称镜头时，角色不会因此原地旋转。
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 让角色自动面向当前移动方向。
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 设置角色转向速度。这里只使用 Yaw（水平旋转），速度为每秒 540 度。
	GetCharacterMovement()->RotationRate = FRotator(0.f,540.f,0.f);

	// 设置起跳时向上的初始速度。
	GetCharacterMovement()->JumpZVelocity = 500.f;

	// 设置空中控制能力，让玩家在跳跃或下落时仍能有限地调整方向。
	GetCharacterMovement()->AirControl = 0.35f;

	// 设置角色在地面上的最大移动速度，单位为厘米/秒。
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	// 设置使用手柄摇杆时的最小模拟移动速度。
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;

	// 设置停止地面移动时的刹车减速度。数值越大，停得越快。
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 设置角色处于空中时的刹车减速度。
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	
	// 创建第三人称摄像机弹簧臂，并将它连接到角色根组件（通常是胶囊体）。
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());

	// 设置摄像机与角色之间的目标距离为 600 cm。
	CameraBoom->TargetArmLength = 600.f;

	// 让弹簧臂跟随玩家控制器旋转，实现第三人称视角控制。
	CameraBoom->bUsePawnControlRotation = true;
	
	// 创建跟随摄像机，并将它安装到弹簧臂末端。
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);

	// 摄像机本身不再单独使用控制器旋转，因为旋转已由弹簧臂负责。
	FollowCamera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* ACC_PlayerCharacter::GetAbilitySystemComponent() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState)) return nullptr;
	
	return CCPlayerState->GetAbilitySystemComponent();
}

void ACC_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(),this);
	GiveStartupAbilities();
}

void ACC_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!IsValid(GetAbilitySystemComponent())) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(),this);
}




