#include "Level_CombinedSteering.h"

#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();

	std::vector<BlendedSteering::WeightedBehavior> weightBehaviors{};

	weightBehaviors.emplace_back(new Seek(), 0.5f);
	weightBehaviors.emplace_back(new Wander(), 0.5f);

	pBlendedSteering = new BlendedSteering(weightBehaviors);

	auto agent = GetWorld()->SpawnActor<ASteeringAgent>
		(
			SteeringAgentClass,
			FVector{ 0,0,90 },
			FRotator::ZeroRotator
		);

	agent->SetSteeringBehavior(pBlendedSteering);

	pAgents.push_back(agent);
}

void ALevel_CombinedSteering::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	
		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();
	
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();
	
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();
	
		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
			// TODO: Handle the debug rendering of your agents here :)
			
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
	
		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

		 ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
		 	pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
		 	[this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[0].Weight = InVal; }, "%.2f");
		
		 ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
		 pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
		 [this](float InVal) { pBlendedSteering->GetWeightedBehaviorsRef()[1].Weight = InVal; }, "%.2f");
	
		//End
		ImGui::End();
	}
#pragma endregion
	// Combined Steering Update
	// TODO: implement handling mouse click input for seek
	// TODO: implement Make sure to also evade the wanderer
	if (CanDebugRender)
	{
		for (auto& pAgent : pAgents)
		{
			DebugDraw(pAgent);
		}
	}

}

void ALevel_CombinedSteering::DebugDraw(const ASteeringAgent* pAgent)
{
	constexpr float shrink{ 3.f };						// the lines where to long

	// colors
	constexpr FColor LinearVelocityColor{ 255, 100, 100 };

	DrawDebugDirectionalArrow
	(
		pAgent->GetWorld(),
		FVector(pAgent->GetPosition(), 0.f),
		FVector(pAgent->GetPosition() + pAgent->GetLinearVelocity(), 0.f),
		1.f,
		LinearVelocityColor
	);
}
