// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTestAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERTEST_API UCharacterTestAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsDashing;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bJumped;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsWallRunningLeft;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bIsWallRunningRight;

	void SetJumpTrigger();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	float JumpTriggerTime;
};
