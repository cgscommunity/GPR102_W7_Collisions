# GPR102 - Game Engine Programming - Collisions

This Unreal Engine 5 project provides a visual and modular framework for testing various forms of collision detection between two 3D shapes. It includes support for different shape pairings, real-time visual debugging, and live status feedback via a text label.

## Features

- Modular collision system using an enum to switch between modes
- Shape pairings:
    - Sphere to Sphere
    - Box to Sphere
    - Box to Box (AABB)
    - Capsule to Sphere
    - Capsule to Capsule
    - Box to Box (OBB)
- Debug rendering of shapes and collision lines
- UI label to indicate collision status in real time

## How It Works

Two scene components (`ShapePositionA` and `ShapePositionB`) act as transform handles for each shape. Collision checks are performed each frame based on the selected `ECollisionCheckType`. The result updates a `TextRenderComponent` with either "Collision!" or "No Collision" and changes the text color accordingly.

Collision checks are implemented (or stubbed out for extension) in the `CheckCollision()` function in `CollisionResolver.cpp`.

## Usage

1. Attach the `ACollisionResolver` actor to your level.
2. In the Details panel, select a `CollisionCheckType` to test.
3. Adjust shape radii, box extents, or capsule heights in the actor's properties.
4. Move the handles to test overlapping configurations in real-time.

## To Do

- Implement logic for:
    - Sphere-to-Sphere collision
    - Box-to-Sphere collision
    - AABB Box-to-Box collision
    - Capsule-based collisions
    - OBB Box-to-Box collision (partial setup in `AreOBBsOverlapping`)

## Dependencies

- Unreal Engine 5
- `DrawDebug*` functions for visual feedback

## License

© 2025 CG Spectrum. All Rights Reserved.
