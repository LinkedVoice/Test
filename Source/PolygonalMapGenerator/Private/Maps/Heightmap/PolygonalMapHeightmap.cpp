// Original Work Copyright (c) 2010 Amit J Patel, Modified Work Copyright (c) 2016 Jay M Stevens

#include "PolygonalMapGeneratorPrivatePCH.h"
#include "Maps/MapDataHelper.h"
#include "Biomes/BiomeManager.h"
#include "Moisture/MoistureDistributor.h"
#include "Maps/Heightmap/HeightmapPointTask.h"
#include "PolygonalMapHeightmap.h"

void UPolygonalMapHeightmap::CreateHeightmap(UPolygonMap* PolygonMap, UBiomeManager* BiomeMngr, UMoistureDistributor* MoistureDist, const int32 Size, const EHeightmapGenerationType HeightmapGenerationOptions, const FIslandGeneratorDelegate OnComplete)
{
	if (PolygonMap == NULL)
	{
		return;
	}
	BiomeManager = BiomeMngr;
	MoistureDistributor = MoistureDist;
	HeightmapSize = Size;
	HeightmapData.Empty();
	OnGenerationComplete = OnComplete;

	// Interpolate between the actual points
	CreateHeightmapTimer = FPlatformTime::Seconds();

	if (HeightmapGenerationOptions == EHeightmapGenerationType::Background)
	{
		FIslandGeneratorDelegate generatePoints;
		generatePoints.BindDynamic(this, &UPolygonalMapHeightmap::CheckMapPointsDone);
		FHeightmapPointGenerator::GenerateHeightmapPoints(HeightmapSize, NumberOfPointsToAverage, this, PolygonMap, BiomeManager, generatePoints);
	}
	else if (HeightmapGenerationOptions == EHeightmapGenerationType::Foreground)
	{
		FHeightmapPointGenerator::MapScale = (float)PolygonMap->GetGraphSize() / (float)HeightmapSize;
		float squaredHeightmap = (float)HeightmapSize * (float)HeightmapSize;
		float current = 0.0f;
		for (int32 x = 0; x < HeightmapSize; x++)
		{
			for (int32 y = 0; y < HeightmapSize; y++)
			{
				HeightmapData.Add(FHeightmapPointTask::MakeMapPoint(FVector2D(x, y), PolygonMap, BiomeManager, EPointSelectionMode::InterpolatedWithPolygonBiome));
				current++;
				FHeightmapPointGenerator::CompletionPercent = current / squaredHeightmap;
			}
		}
		DoHeightmapPostProcess();
	}
	else
	{
		unimplemented();
	}
}

void UPolygonalMapHeightmap::CheckMapPointsDone()
{
	HeightmapData = FHeightmapPointGenerator::HeightmapData;
	DoHeightmapPostProcess();
}

void UPolygonalMapHeightmap::DoHeightmapPostProcess()
{
	UE_LOG(LogWorldGen, Log, TEXT("%d map points created in %f seconds."), HeightmapSize * HeightmapSize, FPlatformTime::Seconds() - CreateHeightmapTimer);

	/*// Normalize between 0 and 1
	// I had assumed these values were already normalized, but apparently not
	float maxHeightmapSize = -1.0f;
	CreateHeightmapTimer = FPlatformTime::Seconds();
	for (int i = 0; i < HeightmapData.Num(); i++)
	{
		if (HeightmapData[i].Elevation > maxHeightmapSize)
		{
			maxHeightmapSize = HeightmapData[i].Elevation;
		}
	}
	for (int i = 0; i < HeightmapData.Num(); i++)
	{
		HeightmapData[i].Elevation /= maxHeightmapSize;
	}
	UE_LOG(LogWorldGen, Log, TEXT("Points normalized in %f seconds."), FPlatformTime::Seconds() - CreateHeightmapTimer);*/

	// Create the biomes
	CreateHeightmapTimer = FPlatformTime::Seconds();
	for (int i = 0; i < HeightmapData.Num(); i++)
	{
		HeightmapData[i].Biome = BiomeManager->DetermineBiome(HeightmapData[i]);
	}
	UE_LOG(LogWorldGen, Log, TEXT("Biomes determined in %f seconds."), FPlatformTime::Seconds() - CreateHeightmapTimer);

	// Add the rivers
	/*CreateHeightmapTimer = FPlatformTime::Seconds();
	for (int i = 0; i < MoistureDistributor->Rivers.Num(); i++)
	{
		MoistureDistributor->Rivers[i]->MoveRiverToHeightmap(this);
	}
	UE_LOG(LogWorldGen, Log, TEXT("Rivers placed in %f seconds."), FPlatformTime::Seconds() - CreateHeightmapTimer);*/

	if (OnGenerationComplete.IsBound())
	{
		OnGenerationComplete.Execute();
		OnGenerationComplete.Unbind();
	}
}

float UPolygonalMapHeightmap::GetCompletionPercent() const
{
	return FHeightmapPointGenerator::CompletionPercent;
}


TArray<FMapData> UPolygonalMapHeightmap::GetMapData()
{
	return HeightmapData;
}

FMapData UPolygonalMapHeightmap::GetMapPoint(int32 x, int32 y)
{
	int32 index = x + (y * HeightmapSize);
	if (index < 0 || HeightmapData.Num() <= index)
	{
		UE_LOG(LogWorldGen, Warning, TEXT("Tried to get a pixel at %d, %d, but no pixel was found."), x, y);
		return FMapData();
	}
	else
	{
		return HeightmapData[index];
	}
}

void UPolygonalMapHeightmap::SetMapPoint(int32 X, int32 Y, FMapData MapData)
{
	int32 index = X + (Y * HeightmapSize);
	if (index < 0 || HeightmapData.Num() <= index)
	{
		UE_LOG(LogWorldGen, Warning, TEXT("Tried to set a pixel at %d, %d, but no pixel was found."), X, Y);
		return;
	}
	else
	{
		HeightmapData[index] = MapData;
	}
}