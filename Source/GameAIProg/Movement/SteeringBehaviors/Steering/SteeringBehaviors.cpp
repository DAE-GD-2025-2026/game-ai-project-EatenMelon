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
	Agent.SetIsAutoOrienting(true);

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

	// act based of the ranges
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
	Agent.SetIsAutoOrienting(false);

	SteeringOutput Steering{};

	// Direction to target
	const FVector2D DesiredDir = (Target.Position - Agent.GetPosition()).GetSafeNormal();
	const FVector Forward = Agent.GetActorForwardVector();

	// Signed angle between them
	const float CrossZ = FVector::CrossProduct(Forward, FVector(DesiredDir, 0.f)).Z;
	const float Dot = FVector::DotProduct(Forward, FVector(DesiredDir, 0.f));

	const float AngleRad = FMath::Atan2(CrossZ, Dot);
	const float AngleDeg = FMath::RadiansToDegrees(AngleRad);

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

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	constexpr float margin{ 15.f };

	Target.Position = GetTargetFuturePos(Agent);

	// using facing steering for angular velocity
	Face faceOutput{};
	faceOutput.SetTarget(Target);
	
	SteeringOutput Steering{ faceOutput.CalculateSteering(DeltaT, Agent) };

	const float distToTarget = (Target.Position - Agent.GetPosition()).SizeSquared();

	// avoids the spinning
	if (distToTarget < margin * margin)
	{
		Steering.LinearVelocity = FVector2D::Zero();
		Steering.AngularVelocity = 0.f;

		return Steering;
	}

	const FVector forward = Agent.GetActorForwardVector();
	Steering.LinearVelocity = FVector2D(forward.X, forward.Y);

	return Steering;
}

FVector2D Pursuit::GetTargetFuturePos(ASteeringAgent& Agent) const
{
	const float distToTarget = (Target.Position - Agent.GetPosition()).Size();
	const float time = distToTarget / Agent.GetMaxLinearSpeed();

	FVector2D newTarget = Target.Position + Target.LinearVelocity * time;
	return newTarget;
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	FVector2D awayDir = (Agent.GetPosition() - Target.Position).GetSafeNormal();

	Face faceOutput;

	Target.Position = Agent.GetPosition() + awayDir;
	faceOutput.SetTarget(Target);

	SteeringOutput Steering = faceOutput.CalculateSteering(DeltaT, Agent);

	const FVector forward = Agent.GetActorForwardVector();
	Steering.LinearVelocity = FVector2D(forward.X, forward.Y);

	return Steering;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	// get a random angle within a range
	const float minAngle{ m_WanderAngle - m_MaxAngleChange };
	const float maxAngle{ m_WanderAngle + m_MaxAngleChange };

	float randomAngle = FMath::FRandRange(0.f, PI * 2);
	randomAngle = FMath::Clamp(randomAngle, minAngle, maxAngle);

	// use the angle for the new target position
	FVector2D targetPos = FVector2D(cosf(randomAngle), sinf(randomAngle));

	const FVector forward = Agent.GetActorForwardVector();

	targetPos *= m_Radius;
	targetPos += FVector2D(forward.X, forward.Y) * m_OffsetDistance;

	// seek towards the new target position
	Target.Position = targetPos;

	SteeringOutput Steering{ Seek::CalculateSteering(DeltaT, Agent) };

	m_WanderAngle = randomAngle;

	return Steering;
}
