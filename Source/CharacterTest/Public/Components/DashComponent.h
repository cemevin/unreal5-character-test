// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


class ACharacterTestCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHARACTERTEST_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UDashComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	bool CanDash() const;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	float DashSpeed = 500;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	float DashCooldown = 3;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	float DashDuration = 1;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	float DashFOV = 130;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	bool bDashTowardsControllerRotation = true;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash")
	bool bLerpCameraRotationToPlayer = false;
	
	UPROPERTY(EditAnywhere, Category="Movement|Dash", meta=(EditCondition="bLerpCameraRotationToPlayer"))
	float CameraInterpSpeed = 10;

public:


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	void DoDash();
	void StopDashing();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashStarted);
	UPROPERTY(BlueprintAssignable, Category="Movement|Dash")
	FOnDashStarted OnDashStarted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashEnded);
	UPROPERTY(BlueprintAssignable, Category="Movement|Dash")
	FOnDashStarted OnDashEnded;
	
private:

	float LastTimeDashed = 0;
	bool bDashing = false;
	float CachedFOV;

	ACharacterTestCharacter* CharacterOwner;
};
