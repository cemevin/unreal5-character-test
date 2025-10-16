// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/CharacterTestAnimInstance.h"

void UCharacterTestAnimInstance::SetJumpTrigger()
{
	bJumped = true;
	JumpTriggerTime = GetWorld()->TimeSeconds;
}

void UCharacterTestAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bJumped && GetWorld()->TimeSeconds - JumpTriggerTime > DeltaSeconds * 3)
	{
		bJumped = false;
	}
}
