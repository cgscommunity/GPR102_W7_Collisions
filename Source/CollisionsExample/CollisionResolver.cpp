#include "CollisionResolver.h"
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
			// Use squared distance for efficiency (avoids sqrt)
			float DistSquared = FVector::DistSquared(LocationA, LocationB);
			float CombinedRadius = RadiusA + RadiusB;

			bIsColliding = (DistSquared <= CombinedRadius * CombinedRadius);

			// Draw visual aids
			DrawDebugSphere(GetWorld(), LocationA, RadiusA, 12, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			DrawDebugLine(GetWorld(), LocationA, LocationB, FColor::White, false, -1.0f, 0, 1.0f);
			break;
		}
		case ECollisionCheckType::BoxToBoxAABB:
		{
			// Calculate AABB min/max bounds for both boxes
			FVector MinA = LocationA - BoxExtentA;
			FVector MaxA = LocationA + BoxExtentA;

			FVector MinB = LocationB - BoxExtentB;
			FVector MaxB = LocationB + BoxExtentB;

			// Test overlap along X, Y, Z independently
			bIsColliding =
				(MinA.X <= MaxB.X && MaxA.X >= MinB.X) &&
				(MinA.Y <= MaxB.Y && MaxA.Y >= MinB.Y) &&
				(MinA.Z <= MaxB.Z && MaxA.Z >= MinB.Z);

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugBox(GetWorld(), LocationB, BoxExtentB, FQuat::Identity, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::BoxToBoxOBB:
		{
			// Get box orientations as rotation quaternions
			FQuat RotationA = ShapePositionA->GetComponentQuat();
			FQuat RotationB = ShapePositionB->GetComponentQuat();

			// Use Separating Axis Theorem (SAT) to test for overlap
			bIsColliding = AreOBBsOverlapping(LocationA, BoxExtentA, RotationA,
											  LocationB, BoxExtentB, RotationB);

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, RotationA, FColor::Blue, false, -1.0f);
			DrawDebugBox(GetWorld(), LocationB, BoxExtentB, RotationB, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::CapsuleToSphere:
		{
			// Define capsule line segment (from base to top)
			FVector CapsuleBase = LocationA;
			FVector CapsuleTop = LocationA + FVector::UpVector * HeightA;

			// Find closest point on capsule line segment to sphere center
			FVector ClosestPoint = FMath::ClosestPointOnSegment(LocationB, CapsuleBase, CapsuleTop);

			// Measure distance from sphere center to closest point on capsule
			float DistSquared = FVector::DistSquared(ClosestPoint, LocationB);
			float CombinedRadius = RadiusA + RadiusB;

			bIsColliding = (DistSquared <= CombinedRadius * CombinedRadius);

			DrawDebugCapsule(GetWorld(), LocationA + FVector::UpVector * HeightA * 0.5f, HeightA * 0.5f, RadiusA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::CapsuleToCapsule:
		{
			// Define capsule A and capsule B segments
			FVector StartA = LocationA;
			FVector EndA = LocationA + FVector::UpVector * HeightA;

			FVector StartB = LocationB;
			FVector EndB = LocationB + FVector::UpVector * HeightB;

			// Find closest points between the two line segments
			FVector ClosestA, ClosestB;
			FMath::SegmentDistToSegmentSafe(StartA, EndA, StartB, EndB, ClosestA, ClosestB);

			float SqrDist = FVector::DistSquared(ClosestA, ClosestB);
			float CombinedRadius = RadiusA + RadiusB;

			bIsColliding = (SqrDist <= CombinedRadius * CombinedRadius);

			DrawDebugCapsule(GetWorld(), LocationA + FVector::UpVector * HeightA * 0.5f, HeightA * 0.5f, RadiusA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugCapsule(GetWorld(), LocationB + FVector::UpVector * HeightB * 0.5f, HeightB * 0.5f, RadiusB, FQuat::Identity, FColor::Red, false, -1.0f);
			break;
		}
		case ECollisionCheckType::BoxToSphere:
		{
			// Define AABB for the box
			FVector BoxMin = LocationA - BoxExtentA;
			FVector BoxMax = LocationA + BoxExtentA;

			// Clamp the sphere center to the closest point inside the box bounds
			FVector ClosestPoint = LocationB;
			ClosestPoint.X = FMath::Clamp(LocationB.X, BoxMin.X, BoxMax.X);
			ClosestPoint.Y = FMath::Clamp(LocationB.Y, BoxMin.Y, BoxMax.Y);
			ClosestPoint.Z = FMath::Clamp(LocationB.Z, BoxMin.Z, BoxMax.Z);

			// If the sphere center is inside or very close to the box, they're colliding
			float DistSquared = FVector::DistSquared(ClosestPoint, LocationB);
			bIsColliding = (DistSquared <= RadiusB * RadiusB);

			DrawDebugBox(GetWorld(), LocationA, BoxExtentA, FQuat::Identity, FColor::Blue, false, -1.0f);
			DrawDebugSphere(GetWorld(), LocationB, RadiusB, 12, FColor::Red, false, -1.0f);
			break;
		}
	}
}


bool ACollisionResolver::AreOBBsOverlapping(FVector CenterA, FVector ExtentsA, FQuat RotationA,
                                            FVector CenterB, FVector ExtentsB, FQuat RotationB)
{
    // Convert object rotations into basis matrices to extract their local axes
    // These axes represent the orientation of the box in world space
    FMatrix RotationMatrixA = FRotationMatrix::Make(RotationA);
    FMatrix RotationMatrixB = FRotationMatrix::Make(RotationB);

    // Get the 3 local axes of box A in world space
    FVector AxisA[3] = {
        RotationMatrixA.GetUnitAxis(EAxis::X),
        RotationMatrixA.GetUnitAxis(EAxis::Y),
        RotationMatrixA.GetUnitAxis(EAxis::Z)
    };

    // Get the 3 local axes of box B in world space
    FVector AxisB[3] = {
        RotationMatrixB.GetUnitAxis(EAxis::X),
        RotationMatrixB.GetUnitAxis(EAxis::Y),
        RotationMatrixB.GetUnitAxis(EAxis::Z)
    };

    // Compute the vector from the center of A to the center of B
    FVector Translation = CenterB - CenterA;

    // Re-express the translation vector in A’s local coordinate system
    Translation = FVector(
        FVector::DotProduct(Translation, AxisA[0]),  // How much B is offset from A along A’s X
        FVector::DotProduct(Translation, AxisA[1]),  // Along A’s Y
        FVector::DotProduct(Translation, AxisA[2])   // Along A’s Z
    );

    // First set of 3 tests: separate along each of A's local axes
    for (int i = 0; i < 3; i++)
    {
        float ProjectedExtentA = ExtentsA[i];  // Projection of A onto its own axis = its half-extent
        float ProjectedExtentB =
            FMath::Abs(FVector::DotProduct(AxisB[0], AxisA[i])) * ExtentsB.X +
            FMath::Abs(FVector::DotProduct(AxisB[1], AxisA[i])) * ExtentsB.Y +
            FMath::Abs(FVector::DotProduct(AxisB[2], AxisA[i])) * ExtentsB.Z;

        // If the projected distance between centers is greater than the sum of projected extents,
        // then there's a gap along this axis — therefore, no overlap
        if (FMath::Abs(Translation[i]) > ProjectedExtentA + ProjectedExtentB)
            return false;
    }

    // Next 3 tests: separate along each of B's local axes
    for (int i = 0; i < 3; i++)
    {
        float ProjectedExtentA =
            FMath::Abs(FVector::DotProduct(AxisA[0], AxisB[i])) * ExtentsA.X +
            FMath::Abs(FVector::DotProduct(AxisA[1], AxisB[i])) * ExtentsA.Y +
            FMath::Abs(FVector::DotProduct(AxisA[2], AxisB[i])) * ExtentsA.Z;

        float ProjectedExtentB = ExtentsB[i];  // Box B projected onto its own axis = its half-extent

        // Same logic: if gap between centers along this axis exceeds projected overlap, they don't collide
        if (FMath::Abs(FVector::DotProduct(Translation, AxisB[i])) > ProjectedExtentA + ProjectedExtentB)
            return false;
    }

    // Final 9 tests: cross products of each axis from A with each axis from B
    // These represent the potential separating axes formed by the "edges" of the two boxes
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // Cross product gives us an axis perpendicular to both axes (A[i] and B[j])
            FVector Axis = FVector::CrossProduct(AxisA[i], AxisB[j]);

            // If A[i] and B[j] are parallel, Axis will be (0,0,0), so skip to avoid division by zero
            if (Axis.IsNearlyZero()) continue;

            // Project both box extents onto the cross product axis
            float ProjectedExtentA =
                FMath::Abs(FVector::DotProduct(AxisA[(i + 1) % 3], Axis)) * ExtentsA[(i + 1) % 3] +
                FMath::Abs(FVector::DotProduct(AxisA[(i + 2) % 3], Axis)) * ExtentsA[(i + 2) % 3];

            float ProjectedExtentB =
                FMath::Abs(FVector::DotProduct(AxisB[(j + 1) % 3], Axis)) * ExtentsB[(j + 1) % 3] +
                FMath::Abs(FVector::DotProduct(AxisB[(j + 2) % 3], Axis)) * ExtentsB[(j + 2) % 3];

            // Again, test whether the projected center distance exceeds the overlap range
            if (FMath::Abs(FVector::DotProduct(Translation, Axis)) > ProjectedExtentA + ProjectedExtentB)
                return false;
        }
    }

    // If none of the 15 separating axes had a gap, then the boxes must be overlapping
    return true;
}
