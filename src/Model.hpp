#pragma once

#include "Core/DataTypes.hpp"
#include "Vars.hpp"

void RegisterModel
(
	ModelData& modelData,
	byte8* modelFile,
	byte8* textureFile
);

void RegisterShadow
(
	ModelData& modelData,
	ShadowData& shadowData,
	byte8* shadowFile
);

void RegisterPhysics
(
	byte8* physicsFile,
	PhysicsData* physicsData,
	PhysicsMetadata** physicsMetadata
);

void ResetModel(ModelData& modelData);