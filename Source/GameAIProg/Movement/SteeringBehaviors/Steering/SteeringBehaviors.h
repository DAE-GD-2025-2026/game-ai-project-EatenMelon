#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>
#include "Kismet/KismetMathLibrary.h"

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	void SetMaxSpeed(float speed) { MaxSpeed = speed; }

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

	float GetSlowRadius() const;
	float GetTargetRadius() const;

protected:
	FTargetData Target;

	float SlowRadius{ 700.f };
	float TargetRadius{ 100.f };

	float MaxSpeed{ 0.f };
};

// Your own SteeringBehaviors should follow here...

class Seek : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Flee : public Seek
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Arrive : public Seek
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Face : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Pursuit : public ISteeringBehavior
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;

protected:
	FVector2D GetTargetFuturePos(ASteeringAgent& Agent) const;
};

class Evade : public Pursuit
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;
};

class Wander : public Seek
{
public:
	SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent) override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

	float GetWanderRadius() const { return m_Radius; }
	float GetWanderOffset() const { return m_OffsetDistance; }

protected:
	float m_OffsetDistance{ 150.f };
	float m_Radius{ 100.f };
	float m_MaxAngleChange{ FMath::DegreesToRadians(30.f) };
	float m_WanderAngle{ 0.f };
};