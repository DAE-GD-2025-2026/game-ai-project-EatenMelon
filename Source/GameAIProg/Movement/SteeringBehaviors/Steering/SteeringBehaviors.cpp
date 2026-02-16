#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

#include <string>

float ISteeringBehavior::GetSlowRadius() const
{
	return SlowRadius;
}

float ISteeringBehavior::GetTargetRadius() const
{
	return TargetRadius;
}

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = (Target.Position - Agent.GetPosition()).GetSafeNormal();

	return Steering;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{ Seek::CalculateSteering(DeltaT, Agent) };

	Steering.LinearVelocity = -Steering.LinearVelocity;

	return Steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{ Seek::CalculateSteering(DeltaT, Agent) };

	Agent.SetMaxLinearSpeed(MaxSpeed);

	const float slowRadiusSq = SlowRadius * SlowRadius;
	const float targetRadiusSq = TargetRadius * TargetRadius;
	const float distanceSq = FVector2D::DistSquared(Agent.GetPosition(), Target.Position);

	if (distanceSq <= targetRadiusSq)
	{
		Agent.SetMaxLinearSpeed(0.f);
	}
	else if (distanceSq <= slowRadiusSq)
	{
		float scale = (distanceSq - targetRadiusSq) / (slowRadiusSq - targetRadiusSq);
		Agent.SetMaxLinearSpeed(MaxSpeed * FMath::Clamp(scale, 0.05f, 1.f));
	}

	return Steering;
}

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	// Direction to target (normalized!)
	FVector2D ToTarget = Target.Position - Agent.GetPosition();
	FVector2D DesiredDir = ToTarget.GetSafeNormal();

	// Current forward
	FVector Forward = Agent.GetActorForwardVector();

	// Signed angle between them
	float CrossZ = FVector::CrossProduct(Forward, FVector(DesiredDir, 0.f)).Z;
	float Dot = FVector::DotProduct(Forward, FVector(DesiredDir, 0.f));

	float AngleRad = FMath::Atan2(CrossZ, Dot);
	float AngleDeg = FMath::RadiansToDegrees(AngleRad);

	// Convert to angular velocity
	float AngularVelocity = AngleDeg / DeltaT;

	// Clamp to max rotation speed
	AngularVelocity = FMath::Clamp(
		AngularVelocity,
		-Agent.GetMaxAngularSpeed(),
		Agent.GetMaxAngularSpeed()
	);

	// Stop jitter when almost aligned
	if (FMath::Abs(AngleDeg) < 1.f)
	{
		AngularVelocity = 0.f;
	}

	Steering.AngularVelocity = AngularVelocity;

	return Steering;
}
