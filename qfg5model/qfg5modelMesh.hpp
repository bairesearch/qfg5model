/*******************************************************************************
 *
 * File Name: qfg5modelMesh.hpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: 3D mesh i/o functions
 *
 *******************************************************************************/

void importOrExportMeshFile(array<unsigned char>^ % mdlMeshData, array<unsigned char>^ % mdlMeshDataNew, int numberOfSubmeshes, int mdlMeshDataAddress, System::String^ inputFileNameBase, System::String^ outputFileNameBase, bool importMesh);