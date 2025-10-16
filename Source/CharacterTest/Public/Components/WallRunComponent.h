// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunComponent.generated.h"


class ACharacterTestCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class CHARACTERTEST_API UWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UWallRunComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	bool CanWallRun();

	bool IsWallRunning() const { return bIsWallRunning; }

	void StartWallRunning(const FHitResult& HitResult);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnWallRun();

	void StopWallRun();

	UFUNCTION()
	void OnJumpAttempted();

	UPROPERTY(EditAnywhere)
	FName WallRunTag = TEXT("WallRun");
	
	UPROPERTY(BlueprintReadOnly)
	ACharacterTestCharacter* CharacterOwner;

	UPROPERTY(EditAnywhere)
	bool bOverrideWalkSpeed = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bOverrideWalkSpeed"))
	float WallRunSpeed = 300;

	UPROPERTY(EditAnywhere)
	float WallRunOffset = 35;

	UPROPERTY(EditAnywhere)
	float WallRunHeightOffset = 35;

	UPROPERTY(EditAnywhere)
	float WallRunTimeout = 0.5f;

	UPROPERTY(EditAnywhere)
	float CameraInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere)
	float LaunchAngle = 45.f;

	UPROPERTY(EditAnywhere)
	bool bMatchZHeightWithWallRunSection = true;

	UPROPERTY(EditAnywhere)
	bool bOverrideLaunchVelocity = true;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bOverrideLaunchVelocity"))
	FVector LaunchVelocity{600,0,300};

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bIsWallRunning = false;
	bool bIsWallRunningRight;
	FHitResult WallRunHitResult;
	float LastTimeWallRun;
	float LastTimeWallRunStarted;
	int FrameCounter;
	
};
