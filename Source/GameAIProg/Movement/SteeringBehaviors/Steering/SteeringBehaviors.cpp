#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

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

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	//FVector example{ Target.Position, 0.f };
	//Agent.GetWorld()

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
