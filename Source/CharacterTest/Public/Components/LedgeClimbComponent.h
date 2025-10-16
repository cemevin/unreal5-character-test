﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeClimbComponent.generated.h"
class ACharacterTestCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class CHARACTERTEST_API ULedgeClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	ULedgeClimbComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	bool CanLedgeClimb();

	bool IsLedgeClimbing() const { return bIsLedgeClimbing; }

	void StartLedgeClimbing(const FVector& FeetPosition, const FVector& LedgeLocation, const FRotator& LedgeRotation, bool bJumpingFromBelow);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnLedgeClimb(bool bFromBelow);

	UFUNCTION(BlueprintCallable)
	void OnLedgeClimbEnded();

	UPROPERTY(EditAnywhere)
	float DetectionMinHeight = 75;

	UPROPERTY(EditAnywhere)
	float DetectionMaxHeight = 150;

	UPROPERTY(EditAnywhere)
	float DetectionDistanceForward = 150;

	UPROPERTY(EditAnywhere)
	float MantleFromBelowHeightThreshold = 100;

	UPROPERTY(EditAnywhere)
	float CameraInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere)
	FVector MantleHandAdjustment{4.0f, 0.f, 10.f};

	UPROPERTY(EditAnywhere)
	FName LedgeTag = TEXT("Ledge");

	UPROPERTY(EditAnywhere)
	FName WarpTargetNameUp = TEXT("ClimbUp");

	UPROPERTY(EditAnywhere)
	FName WarpTargetNameForward = TEXT("ClimbForward");
	
	UPROPERTY(BlueprintReadOnly)
	ACharacterTestCharacter* CharacterOwner;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bIsLedgeClimbing = false;
};
