// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Components/LedgeClimbComponent.h"

#include "Public/CharacterTestCharacter.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
ULedgeClimbComponent::ULedgeClimbComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULedgeClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ACharacterTestCharacter>(GetOwner());
	check(CharacterOwner);
	
}

bool ULedgeClimbComponent::CanLedgeClimb()
{
	return !bIsLedgeClimbing && !CharacterOwner->IsMoveInputIgnored() && CharacterOwner->GetCharacterMovement()->IsFalling();
}

void ULedgeClimbComponent::StartLedgeClimbing(const FVector& FeetLocation, const FVector& LedgeLocation, const FRotator& LedgeRotation, bool bJumpingFromBelow)
{
	bIsLedgeClimbing = true;

	FRotator CharacterRotation = LedgeRotation;
	CharacterRotation.Yaw += 180;
	CharacterOwner->SetActorRotation(CharacterRotation);

	CharacterOwner->GetCharacterMovement()->StopActiveMovement();
	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	CharacterOwner->SetActorEnableCollision(false);
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->ClientIgnoreLookInput(true);
	PC->ClientIgnoreMoveInput(true);

	BP_OnLedgeClimb(bJumpingFromBelow);
	
	FMotionWarpingTarget Target;
	Target.Location = LedgeLocation;
	Target.Name = WarpTargetNameUp;
	CharacterOwner->GetMotionWarping()->AddOrUpdateWarpTarget(Target);
	
	FMotionWarpingTarget TargetFwd;
	Target.Location = FeetLocation;
	Target.Name = WarpTargetNameForward;
	CharacterOwner->GetMotionWarping()->AddOrUpdateWarpTarget(TargetFwd);
}

void ULedgeClimbComponent::OnLedgeClimbEnded()
{
	//
	bIsLedgeClimbing = false;
	
	CharacterOwner->GetMotionWarping()->RemoveAllWarpTargets();

	CharacterOwner->SetActorEnableCollision(true);
	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	CharacterOwner->GetCharacterMovement()->StopActiveMovement();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->ClientIgnoreLookInput(false);
	PC->ClientIgnoreMoveInput(false);
}

// Called every frame
void ULedgeClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanLedgeClimb() && CharacterOwner->GetLastMovementInputVector().Dot(CharacterOwner->GetActorForwardVector()) >= 0)
	{
		FVector Start = CharacterOwner->GetActorLocation() + FVector::UpVector * DetectionMinHeight;
		FVector End = CharacterOwner->GetActorLocation() + FVector::UpVector * DetectionMaxHeight + CharacterOwner->GetActorForwardVector() * DetectionDistanceForward;
		TArray<FHitResult> Hits;
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);

		if (GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectQueryParams, FCollisionShape::MakeSphere(16), CollisionParams))
		{
			FHitResult LedgeHitResult;
			for (auto& Hit : Hits)
			{
				if (Hit.GetActor()->ActorHasTag(LedgeTag))
				{
					LedgeHitResult = Hit;
					break;
				}
			}

			if (LedgeHitResult.GetActor())
			{
				FHitResult VerticalHitResult;
				if (GetWorld()->LineTraceSingleByObjectType(VerticalHitResult, LedgeHitResult.ImpactPoint + FVector::UpVector * 100, LedgeHitResult.ImpactPoint, ObjectQueryParams, CollisionParams))
				{
					FHitResult HorizontalHitResult;
					if (GetWorld()->LineTraceSingleByObjectType(HorizontalHitResult, VerticalHitResult.ImpactPoint - CharacterOwner->GetActorForwardVector() * 100, VerticalHitResult.ImpactPoint, ObjectQueryParams, CollisionParams))
					{
						FVector LedgeLocation = HorizontalHitResult.ImpactPoint;
						FVector ClimbDirection = LedgeHitResult.GetActor()->GetActorForwardVector() * -1;
						FRotator LedgeRotation = UKismetMathLibrary::MakeRotFromXZ(LedgeHitResult.GetActor()->GetActorForwardVector(), HorizontalHitResult.ImpactNormal);
						FVector FeetLocation = LedgeLocation + CharacterOwner->GetSimpleCollisionRadius() * ClimbDirection;
						bool bJumpingFromBelow = FeetLocation.Z - CharacterOwner->GetActorLocation().Z > MantleFromBelowHeightThreshold;
						StartLedgeClimbing(FeetLocation, LedgeLocation + ClimbDirection * MantleHandAdjustment.X + FVector::UpVector * MantleHandAdjustment.Z, LedgeRotation, bJumpingFromBelow);
					}
				}
			}
		}
	}
	else if (IsLedgeClimbing())
	{
		FRotator CameraRot = CharacterOwner->GetControlRotation();
		FRotator TargetCameraRot = CameraRot;
		TargetCameraRot.Yaw = CharacterOwner->GetActorRotation().Yaw;
		CharacterOwner->GetController()->SetControlRotation(FMath::RInterpTo(CameraRot, TargetCameraRot, DeltaTime, CameraInterpSpeed));
	}
}

