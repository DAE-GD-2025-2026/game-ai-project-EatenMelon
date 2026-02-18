// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"



// Sets default values
ASteeringAgent::ASteeringAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();
}

void ASteeringAgent::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ASteeringAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SteeringBehavior)
	{
		SteeringOutput output = SteeringBehavior->CalculateSteering(DeltaTime, *this);

		AddMovementInput(FVector{output.LinearVelocity, 0.f});

		// TODO Implement angular velocity handling
		
		//AddActorWorldRotation(
		//	FRotator(0.f, output.AngularVelocity * DeltaTime, 0.f)
		//);

		if (!IsAutoOrienting())
		{
			//if(AAIController* AIController = Cast<AAIController*>(GetController()))
			{
				float deltaYaw = FMath::Clamp(output.AngularVelocity, -1.f, 1.f) * GetMaxAngularSpeed() * DeltaTime;

				const FRotator CurrentRotation{ GetActorForwardVector().ToOrientationRotator() };
				const FRotator DeltaRotation{ 0.f, deltaYaw, 0.f };
				const FRotator DesiredRotation{ CurrentRotation.Yaw + DeltaRotation.Yaw };

				if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw))
				{
					FaceRotation(DesiredRotation);
				}
			}
		}
	}
}

// Called to bind functionality to input
void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior)
{
	SteeringBehavior = NewSteeringBehavior;
}

