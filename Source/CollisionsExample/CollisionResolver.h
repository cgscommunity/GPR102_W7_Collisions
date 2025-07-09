// © 2025 CG Spectrum. All Rights Reserved.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionResolver.generated.h"

// Forward Declarations
class UTextRenderComponent;

UENUM(BlueprintType)
enum class ECollisionCheckType : uint8
{
	SphereToSphere UMETA(DisplayName = "Sphere to Sphere"),
	BoxToSphere UMETA(DisplayName = "Box to Sphere"),
	BoxToBoxAABB UMETA(DisplayName = "Box to Box (AABB)"),
	CapsuleToSphere UMETA(DisplayName = "Capsule to Sphere"),
	CapsuleToCapsule UMETA(DisplayName = "Capsule to Capsule"),
	BoxToBoxOBB UMETA(DisplayName = "Box to Box (OBB)"),
};


UCLASS()
class COLLISIONSEXAMPLE_API ACollisionResolver : public AActor
{
	GENERATED_BODY()
	
public:	
	ACollisionResolver();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CheckCollision();

	bool AreOBBsOverlapping(FVector CenterA, FVector ExtentsA, FQuat RotationA,
							FVector CenterB, FVector ExtentsB, FQuat RotationB);
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	ECollisionCheckType CollisionCheckType = ECollisionCheckType::SphereToSphere;

	// Radius for the Sphere/Capsule A	
	UPROPERTY(EditAnywhere, Category = "ShapeA")
	float RadiusA = 50.0f;

	// Height of Capsule A
	UPROPERTY(EditAnywhere, Category = "ShapeA")
	float HeightA = 300.0f;

	// Radius for the Sphere/Capsule B
	UPROPERTY(EditAnywhere, Category = "ShapeB")
	float RadiusB = 50.0f;

	// Height of Capsule B
	UPROPERTY(EditAnywhere, Category = "ShapeB")
	float HeightB = 300.0f;

	// Extent for Box A (in half-size terms)
	UPROPERTY(EditAnywhere, Category = "ShapeA")
	FVector BoxExtentA = FVector(50.0f);

	// Extent for Box B (in half-size terms)
	UPROPERTY(EditAnywhere, Category = "ShapeB")
	FVector BoxExtentB = FVector(50.0f);

	// Scene Components for Gizmo-based Positioning
	UPROPERTY(VisibleAnywhere)
	USceneComponent* ShapePositionA;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* ShapePositionB;

	// Text Render for Collision Status
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* TextRender;

	bool bIsColliding;
};
