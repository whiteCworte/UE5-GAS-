// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CC_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTags/CCTags.h"

void ACC_PlayerController::SetupInputComponent()
{
	// 先执行父类的初始化，确保 PlayerController 拥有可用的 InputComponent。
	Super::SetupInputComponent();
	
	// 获取当前本地玩家的 Enhanced Input 子系统，用于添加输入映射上下文。
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	// 非本地控制器或子系统尚未创建时，不再继续初始化输入。
	if (!IsValid(InputSubsystem)) return;
	
	// 添加在蓝图默认值中配置的所有输入映射上下文。
	// 映射上下文负责把键盘、鼠标或手柄按键映射到 Input Action。
	for (UInputMappingContext* Context : InputMappingContexts)
	{
		// 第二个参数是优先级；数值越大，发生输入冲突时优先级越高。
		InputSubsystem->AddMappingContext(Context,0);
	}
	
	// 将普通 InputComponent 转换成 Enhanced Input 组件，以便绑定 Input Action。
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	// 如果项目没有使用 Enhanced Input 组件，就无法继续绑定输入事件。
	if (!IsValid(EnhancedInputComponent)) return;
	
	// 按下跳跃键时开始跳跃，松开时停止继续施加跳跃。
	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Started,this,&ThisClass::Jump);
	EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&ThisClass::StopJumping);

	// 移动或视角输入持续发生时，调用对应的处理函数。
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ThisClass::Move);
	EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ThisClass::Look);
	
	// 输入动作被触发时，调用当前控制器中对应的技能函数。
	EnhancedInputComponent->BindAction(PrimaryAction,ETriggerEvent::Started,this,&ThisClass::Primary);
	EnhancedInputComponent->BindAction(SecondaryAction,ETriggerEvent::Triggered,this,&ThisClass::Secondary);
	EnhancedInputComponent->BindAction(TertiaryAction,ETriggerEvent::Triggered,this,&ThisClass::Tertiary);


}




void ACC_PlayerController::Jump()
{
	// 只有当前控制器正在控制 ACharacter 类型的角色时才能调用 Jump。
	if (!IsValid(GetCharacter())) return;
	
	// 调用角色自带的跳跃功能，实际跳跃力度由 CharacterMovement 设置。
	GetCharacter()->Jump();
}

void ACC_PlayerController::StopJumping()
{
	// 防止控制器没有控制有效角色时访问空指针。
	if (!IsValid(GetCharacter())) return;
	
	// 松开跳跃键后终止持续跳跃输入，决定可变跳跃高度。
	GetCharacter()->StopJumping();
}

void ACC_PlayerController::Move(const FInputActionValue& Value)
{
	// 没有控制 Pawn 时不处理移动输入。
	if (!IsValid(GetPawn())) return;
	
	// 读取二维移动输入：X 通常代表左右，Y 通常代表前后。
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// 只使用控制器的水平旋转角度，忽略摄像机的俯仰和翻滚。
	// 因此角色移动方向会以当前摄像机朝向为基准。
	const FRotator YawRotation(0.f,GetControlRotation().Yaw,0.f);

	// 根据水平旋转分别计算世界空间中的前方和右方单位向量。
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	// 将前后输入施加到前方，将左右输入施加到右方。
	// CharacterMovement 会根据这些输入计算最终的移动速度和方向。
	GetPawn()->AddMovementInput(ForwardDirection,MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection,MovementVector.X);
}

void ACC_PlayerController::Look(const FInputActionValue& Value)
{
	// 读取二维视角输入：X 控制水平转向，Y 控制上下观察。
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	// 把输入加入控制器旋转；第三人称弹簧臂会跟随控制器旋转。
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ACC_PlayerController::Primary()
{
	ActivateAbility(CCTags::CCAbilities::Primary);
}

void ACC_PlayerController::Secondary()
{
	ActivateAbility(CCTags::CCAbilities::Secondary);
}

void ACC_PlayerController::Tertiary()
{
	ActivateAbility(CCTags::CCAbilities::Tertiary);
}


void ACC_PlayerController::ActivateAbility(const FGameplayTag& AbilityTag) const
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;
	
	ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

