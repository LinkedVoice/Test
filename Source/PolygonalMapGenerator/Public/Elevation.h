/*
* From http://www.redblobgames.com/maps/mapgen2/
* Original work copyright 2017 Red Blob Games <redblobgames@gmail.com>
* Unreal Engine 4 implementation copyright 2018 Jay Stevens <jaystevens42@gmail.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TriangleDualMesh.h"
#include "PolygonalMapGenerator.h"
#include "Elevation.generated.h"

/**
 * 
 */
UCLASS()
class POLYGONALMAPGENERATOR_API UElevation : public UDataAsset
{
	GENERATED_BODY()

protected:
	/**
	* Coast corners are connected to coast sides, which have
	* ocean on one side and land on the other
	*/
	virtual TArray<FTriangleIndex> find_coasts_t(UTriangleDualMesh* Mesh, const TArray<bool>& r_ocean) const;

	bool t_ocean(FTriangleIndex t, UTriangleDualMesh* Mesh, const TArray<bool>& r_ocean) const;
	bool r_lake(FPointIndex r, const TArray<bool>& r_water, const TArray<bool>& r_ocean) const;
	bool s_lake(FSideIndex s, UTriangleDualMesh* Mesh, const TArray<bool>& r_water, const TArray<bool>& r_ocean) const;

	virtual void DistributeElevations(TArray<float> &t_elevation, UTriangleDualMesh* Mesh, const TArray<int32> &t_coastdistance, const TArray<bool>& r_ocean, int32 MinDistance, int32 MaxDistance) const;
	virtual void UpdateCoastDistance(TArray<int32> &t_coastdistance, UTriangleDualMesh* Mesh, FTriangleIndex Triangle, int32 Distance) const;

public:
	/**
	* Elevation is based on breadth first search from the seed points,
	* which are the coastal graph nodes. Since breadth first search also
	* calculates the 'parent' pointers, return those for use as the downslope
	* graph. To handle lakes, which should have all corners at the same elevation,
	* there are two deviations from breadth first search:
	* 1. Instead of pushing to the end of the queue, push to the beginning.
	* 2. Like uniform cost search, check if the new distance is better than
	*    previously calculated distances. It is possible that one lake corner
	*    was reached with distance 2 and another with distance 3, and we need
	*    to revisit that node and make sure it's set to 2.
	*/
	virtual void assign_t_elevation(TArray<float>& t_elevation, TArray<int32>& t_coastdistance, TArray<FSideIndex>& t_downslope_s, UTriangleDualMesh* Mesh, const TArray<bool>& r_ocean, const TArray<bool>& r_water, FRandomStream& DrainageRng) const;

	/**
	* Redistribute elevation values so that lower elevations are more common
	* than higher elevations. Specifically, we want elevation Z to have frequency
	* (1-Z), for all the non-ocean regions.
	*/
	virtual void redistribute_t_elevation(TArray<float>& t_elevation, UTriangleDualMesh* Mesh, const TArray<bool>& r_ocean) const;
	/**
	* Set r elevation to the average of the t elevations. There's a
	* corner case though: it is possible for an ocean region (r) to be
	* surrounded by coastline corners (t), and coastlines are set to 0
	* elevation. This means the region elevation would be 0. To avoid
	* this, I subtract a small amount for ocean regions. */
	virtual void assign_r_elevation(TArray<float>& r_elevation, UTriangleDualMesh* Mesh, const TArray<float>& t_elevation, const TArray<bool>& r_ocean) const;
};
