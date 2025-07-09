// © 2025 CG Spectrum. All Rights Reserved.

// This file's header
#include "CollisionResolver.h"

// Other includes
#include "Components/TextRenderComponent.h"

// Sets default values
ACollisionResolver::ACollisionResolver()
{
	// Enable ticking every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create transform handles for each shape
	ShapePositionA = CreateDefaultSubobject<USceneComponent>(TEXT("ShapePositionA"));
	ShapePositionA->SetupAttachment(RootComponent);

	ShapePositionB = CreateDefaultSubobject<USceneComponent>(TEXT("ShapePositionB"));
	ShapePositionB->SetupAttachment(RootComponent);

	// Create a text label to show collision status
	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRender"));
	TextRender->SetupAttachment(RootComponent);
	TextRender->SetHorizontalAlignment(EHTA_Center);
	TextRender->SetText(FText::FromString("No Collision"));
}

// Called when the game starts or when spawned
void ACollisionResolver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACollisionResolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckCollision();

	// Update visual feedback depending on whether shapes are colliding
	if (bIsColliding)
	{
		TextRender->SetText(FText::FromString("Collision!"));
		TextRender->SetTextRenderColor(FColor::Red);
	}
	else
	{
		TextRender->SetText(FText::FromString("No Collision"));
		TextRender->SetTextRenderColor(FColor::Green);
	}
}

void ACollisionResolver::CheckCollision()
{
	FVector LocationA = ShapePositionA->GetComponentLocation();
	FVector LocationB = ShapePositionB->GetComponentLocation();

	switch (CollisionCheckType)
	{
		case ECollisionCheckType::SphereToSphere:
		{
			bIsColliding = false; // TODO: SphereToSphere

			// Draw visual aids
			DrawDebugSphere(GetWorld(), LocationA, RadiusA, 12, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			DrawDebugLine(GetWorld(), LocationA, LocationB, FColor::White, false, -1.0f, 0, 1.0f);
			break;
		}
		case ECollisionCheckType::BoxToSphere:
		{
			bIsColliding = false; // TODO: BoxToSphere

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::BoxToBoxAABB:
		{			
			bIsColliding = false; // TODO: BoxToBoxAABB

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugBox(GetWorld(), LocationB, BoxExtentB, FQuat::Identity, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::CapsuleToSphere:
		{
			bIsColliding = false; // TODO: CapsuleToSphere

			DrawDebugCapsule(GetWorld(), LocationA + FVector::UpVector * HeightA * 0.5f, HeightA * 0.5f, RadiusA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::CapsuleToCapsule:
		{
			bIsColliding = false; // TODO: CapsuleToCapsule

			DrawDebugCapsule(GetWorld(), LocationA + FVector::UpVector * HeightA * 0.5f, HeightA * 0.5f, RadiusA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugCapsule(GetWorld(), LocationB + FVector::UpVector * HeightB * 0.5f, HeightB * 0.5f, RadiusB, FQuat::Identity, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::BoxToBoxOBB:
		{
			// Get box orientations as rotation quaternions
			FQuat RotationA = ShapePositionA->GetComponentQuat();
			FQuat RotationB = ShapePositionB->GetComponentQuat();

			// TODO: BoxToBoxOBB
			bIsColliding = AreOBBsOverlapping(LocationA, BoxExtentA, RotationA,
											  LocationB, BoxExtentB, RotationB);

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, RotationA, FColor::Blue, false, -1.0f);
			DrawDebugBox(GetWorld(), LocationB, BoxExtentB, RotationB, FColor::Red, false, -1.0f);
			break;
		}
	}
}


bool ACollisionResolver::AreOBBsOverlapping(FVector CenterA, FVector ExtentsA, FQuat RotationA,
                                            FVector CenterB, FVector ExtentsB, FQuat RotationB)
{
	// TODO: BoxToBoxOBB
	return false;
}
