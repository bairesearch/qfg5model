/*******************************************************************************
 *
 * File Name: qfg5modelMeshImport.hpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: 3D mesh input functions
 *
 *******************************************************************************/

#define METHOD_1 (0)
#define METHOD_2 (1)
#define METHOD_3 (2)
#define NUMBER_OF_METHODS (3)

void importHAKfile(System::String^ fileNameHakenberg, array<float>^ % newsubmeshVerticesArray, array<float>^ % newsubmeshFaceArray);
void formatNew3DmeshData(System::String^ submeshName, array<float>^ % submeshVerticesArray, array<float>^ % submeshUVcoordsArray, array<float>^ % submeshFaceArray, array<int>^ % submeshLightingEntriesArray, array<float>^ % newsubmeshVerticesArray, array<float>^ % newsubmeshUVcoordsArray, array<float>^ % newsubmeshFaceArray, array<int>^ % newsubmeshLightingEntriesArray);

