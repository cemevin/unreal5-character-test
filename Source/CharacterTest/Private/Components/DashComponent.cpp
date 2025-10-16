// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Components/DashComponent.h"

#include "Public/CharacterTestAnimInstance.h"
#include "Public/CharacterTestCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ACharacterTestCharacter>(GetOwner());
	check(CharacterOwner);
}

bool UDashComponent::CanDash() const
{
	bool bCooldown = (LastTimeDashed != 0 && GetWorld()->TimeSeconds - LastTimeDashed < DashCooldown);
	return !bCooldown && !UGameplayStatics::GetPlayerController(this, 0)->IsMoveInputIgnored();
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDashing)
	{
		float Alpha = FMath::Min(1, (GetWorld()->TimeSeconds-LastTimeDashed) / DashDuration);
		if (Alpha == 1 || (CharacterOwner->GetVelocity().IsNearlyZero() && Alpha > 0.1f))
		{
			StopDashing();
		}
		else
		{
			float Speed = FMath::InterpEaseInOut(0.f, DashSpeed, Alpha, 3.f);
			FVector Velocity = CharacterOwner->GetActorForwardVector() * Speed;
			FHitResult Hit;
			CharacterOwner->SetActorLocation(CharacterOwner->GetActorLocation() + Velocity * DeltaTime, true, &Hit, ETeleportType::None);

			if (Hit.bBlockingHit)
			{
				StopDashing();
			}
			else
			{
				float FOV = FMath::InterpEaseInOut(CachedFOV, DashFOV, Alpha, 3.f);
				UGameplayStatics::GetPlayerCameraManager(this, 0)->SetFOV(FOV);	
			}
			
		}
	}
}

void UDashComponent::DoDash()
{
	if (!CanDash())
	{
		return;
	}

	bDashing = true;
	LastTimeDashed = GetWorld()->TimeSeconds;

	auto* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FRotator ControlRotation = PlayerController->GetControlRotation();
	ControlRotation.Roll = ControlRotation.Pitch = 0;
	CharacterOwner->SetActorRotation(ControlRotation);

	CachedFOV = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetFOVAngle();

	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	PlayerController->ClientIgnoreMoveInput(true);
	PlayerController->ClientIgnoreLookInput(true);

	if (UCharacterTestAnimInstance* AnimInstance = Cast<UCharacterTestAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->bIsDashing = true;
	}

	OnDashStarted.Broadcast();
}

void UDashComponent::StopDashing()
{
	bDashing = false;
	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	CharacterOwner->GetCharacterMovement()->StopActiveMovement();
	
	UGameplayStatics::GetPlayerCameraManager(this, 0)->SetFOV(CachedFOV);
	
	auto* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	
	PlayerController->ClientIgnoreMoveInput(false);
	PlayerController->ClientIgnoreLookInput(false);
	
	if (UCharacterTestAnimInstance* AnimInstance = Cast<UCharacterTestAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->bIsDashing = false;
	}
	
	OnDashEnded.Broadcast();
}

