// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/Components/WallRunComponent.h"

#include "Public/CharacterTestCharacter.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/CharacterTestAnimInstance.h"

// Sets default values for this component's properties
UWallRunComponent::UWallRunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWallRunComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ACharacterTestCharacter>(GetOwner());
	check(CharacterOwner);

	CharacterOwner->OnJumpAttempted.AddUniqueDynamic(this, &UWallRunComponent::OnJumpAttempted);
	
}

bool UWallRunComponent::CanWallRun()
{
	return !bIsWallRunning && !CharacterOwner->IsMoveInputIgnored() && CharacterOwner->GetCharacterMovement()->IsFalling();
}

void UWallRunComponent::StartWallRunning(const FHitResult& HitResult)
{
	if (LastTimeWallRun != 0 && HitResult.GetActor() == WallRunHitResult.GetActor() && GetWorld()->TimeSeconds - LastTimeWallRun < WallRunTimeout)
	{
		return;
	}
	
	bIsWallRunning = true;
	WallRunHitResult = HitResult;
	LastTimeWallRunStarted = GetWorld()->TimeSeconds;

	bIsWallRunningRight =  WallRunHitResult.ImpactNormal.Dot(CharacterOwner->GetActorRightVector()) > 0;
	

	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	CharacterOwner->GetCharacterMovement()->StopActiveMovement();
	
	FRotator Rot = UKismetMathLibrary::MakeRotFromYZ(WallRunHitResult.ImpactNormal * (bIsWallRunningRight ? 1 : -1), FVector::UpVector);
	CharacterOwner->SetActorRotation(Rot);
	
	CharacterOwner->SetActorLocation(WallRunHitResult.ImpactPoint + CharacterOwner->GetActorRightVector() * (bIsWallRunningRight ? 1 : -1) * CharacterOwner->GetSimpleCollisionRadius());

	if (UCharacterTestAnimInstance* AnimInstance = Cast<UCharacterTestAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->bIsWallRunningRight = bIsWallRunningRight;
		AnimInstance->bIsWallRunningLeft = !bIsWallRunningRight;
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->ClientIgnoreLookInput(true);
	PC->ClientIgnoreMoveInput(true);

}


void UWallRunComponent::StopWallRun()
{
	//
	bIsWallRunning = false;
	LastTimeWallRun = GetWorld()->GetTimeSeconds();

	CharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	if (UCharacterTestAnimInstance* AnimInstance = Cast<UCharacterTestAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance()))
	{
		AnimInstance->bIsWallRunningRight = false;
		AnimInstance->bIsWallRunningLeft = false;
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	PC->ClientIgnoreLookInput(false);
	PC->ClientIgnoreMoveInput(false);
}

void UWallRunComponent::OnJumpAttempted()
{
	if (IsWallRunning())
	{
		bool bWasWallRunningRight = bIsWallRunningRight;
		StopWallRun();

		FVector Launch;
		if (bOverrideLaunchVelocity)
		{
			Launch = LaunchVelocity;
		}
		else
		{
			Launch = {CharacterOwner->GetCharacterMovement()->MaxWalkSpeed, 0, CharacterOwner->GetCharacterMovement()->JumpZVelocity};
		}

		Launch = CharacterOwner->GetActorQuat().RotateVector(Launch);
		Launch = Launch.RotateAngleAxis(LaunchAngle * (bWasWallRunningRight ? 1 : -1), FVector::UpVector);

		CharacterOwner->SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(Launch.GetSafeNormal2D(), FVector::UpVector));
		
		CharacterOwner->LaunchCharacter(Launch, true, true);
	}
}

// Called every frame
void UWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FrameCounter++;

	if (CanWallRun() && CharacterOwner->GetLastMovementInputVector().Dot(CharacterOwner->GetActorForwardVector()) >= 0)
	{
		FVector Start = CharacterOwner->GetActorLocation() + FVector::UpVector * WallRunHeightOffset;
		FVector End = Start + CharacterOwner->GetActorForwardVector() * (CharacterOwner->GetSimpleCollisionRadius() + WallRunOffset);
		FVector EndLeft = Start + CharacterOwner->GetActorRightVector() * -1 * (CharacterOwner->GetSimpleCollisionRadius() + WallRunOffset);
		FVector EndRight = Start + CharacterOwner->GetActorRightVector() * 1 * (CharacterOwner->GetSimpleCollisionRadius() + WallRunOffset);
		FHitResult Hit;
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(CharacterOwner);

		// hit trace forward, left, right, do one per frame to not bottleneck the performance
		if (FrameCounter % 3 == 0 && GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, CollisionParams) && Hit.GetActor()->ActorHasTag(WallRunTag))
		{
			StartWallRunning(Hit);
		}
		else if (FrameCounter % 3 == 1 && GetWorld()->LineTraceSingleByObjectType(Hit, Start, EndLeft, ObjectQueryParams, CollisionParams) && Hit.GetActor()->ActorHasTag(WallRunTag))
		{
			StartWallRunning(Hit);
		}
		else if (FrameCounter % 3 == 2 && GetWorld()->LineTraceSingleByObjectType(Hit, Start, EndRight, ObjectQueryParams, CollisionParams) && Hit.GetActor()->ActorHasTag(WallRunTag))
		{
			StartWallRunning(Hit);
		}
	}
	else if (IsWallRunning())
	{
		FHitResult Hit;

		// lerp Z height to match the wall running section (if needed - makes it feel more streamlined but more game-y)
		FVector DeltaVec = (CharacterOwner->GetActorForwardVector() * (bOverrideWalkSpeed ? WallRunSpeed : CharacterOwner->GetCharacterMovement()->MaxWalkSpeed) * DeltaTime);
		FVector FinalPos = CharacterOwner->GetActorLocation() + DeltaVec;
		
		if (bMatchZHeightWithWallRunSection)
		{
			float Z = FMath::FInterpTo(CharacterOwner->GetActorLocation().Z, WallRunHitResult.GetActor()->GetActorLocation().Z, DeltaTime, 10);
			FinalPos.Z = Z;
		}
		
		CharacterOwner->SetActorLocation(FinalPos, true, &Hit, ETeleportType::TeleportPhysics);
		CharacterOwner->GetCharacterMovement()->Velocity.Z = 0;
		
		FRotator CameraRot = CharacterOwner->GetControlRotation();
		FRotator TargetCameraRot = CameraRot;
		TargetCameraRot.Yaw = CharacterOwner->GetActorRotation().Yaw;
		CharacterOwner->GetController()->SetControlRotation(FMath::RInterpTo(CameraRot, TargetCameraRot, DeltaTime, CameraInterpSpeed));

		if (Hit.bBlockingHit)
		{
			StopWallRun();
		}
		else
		{
			// update wall run
			FHitResult Result;
			FVector Start = CharacterOwner->GetActorLocation();
			FVector End = CharacterOwner->GetActorLocation() + CharacterOwner->GetActorRightVector() * (bIsWallRunningRight ? -1 : 1) * CharacterOwner->GetSimpleCollisionRadius() * 2;
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(CharacterOwner);
			bool bHit = (GetWorld()->LineTraceSingleByObjectType(Result, Start, End, ObjectQueryParams, CollisionParams));

			if (!bHit || Result.GetActor() != WallRunHitResult.GetActor())
			{
				StopWallRun();
			}
		}
	}
}

