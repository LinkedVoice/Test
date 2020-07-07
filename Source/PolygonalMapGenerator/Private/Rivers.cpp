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

#include "Rivers.h"

TArray<int32> URivers::find_spring_t(UTriangleDualMesh* Mesh, const TArray<int32>& r_water, const TArray<int32>& t_elevation, const TArray<int32>& t_downslope_s) const
{
	unimplemented();
	return TArray<int32>();
}

void URivers::assign_s_flow(TArray<int32>& s_flow, UTriangleDualMesh* Mesh, const TArray<int32>& t_downslope_s, const TArray<int32>& river_t, const TArray<int32>& t_elevation) const
{
	unimplemented();
}
