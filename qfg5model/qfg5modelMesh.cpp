/*******************************************************************************
 *
 * File Name: qfg5modelMesh.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: 3D mesh i/o functions
 *
 *******************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "qfg5modelMeshImport.hpp"
#include "qfg5modelShared.hpp"

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

//void importOrExportMeshFile(array<unsigned char>^ % mdlMeshData, array<unsigned char>^ % mdlMeshDataNew, int numberOfSubmeshes, int mdlMeshDataAddress, System::String^ inputFileNameBase, String^ outputFileNameBase, bool importMesh);
	//import/export functions:
	static void insertByteArray(array<unsigned char>^ byteArrayToUpdate, array<unsigned char>^ byteArrayToInsert, int index);

void importOrExportMeshFile(array<unsigned char>^ % mdlMeshData, array<unsigned char>^ % mdlMeshDataNew, int numberOfSubmeshes, int mdlMeshDataAddress, System::String^ inputFileNameBase, System::String^ outputFileNameBase, bool importMesh)
{
	/*
	Algorithm:
	1. Parse mdl3ddata header + submesh data
		a. get all submesh addresses from mdl3ddata header
		b. get submesh data
	2. For every face import [INCOMPLETE] or export HAK file
	*/

	array<array<unsigned char>^>^ newsubmeshArray = gcnew array<array<unsigned char>^>(numberOfSubmeshes);	//for import only

	//1a. get all submesh addresses from mdl3ddata header
	array<int>^ submeshAddressArray = gcnew array<int>(numberOfSubmeshes);
	array<System::String ^>^ submeshNameArray = gcnew array<System::String ^>(numberOfSubmeshes);

	int submeshAddressReferenceAddress = 0;
	for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
	{
		array<unsigned char>^ submeshAddressTempByteArray = { mdlMeshData[submeshAddressReferenceAddress], mdlMeshData[submeshAddressReferenceAddress+1], mdlMeshData[submeshAddressReferenceAddress+2], mdlMeshData[submeshAddressReferenceAddress+3] };
		int submeshAddressTemp = BitConverter::ToUInt32(submeshAddressTempByteArray, 0);
		submeshAddressArray[submeshIndex] = submeshAddressTemp;

		submeshAddressReferenceAddress = submeshAddressReferenceAddress + BYTES_PER_INT;
	}

	//1b. get submesh data
	for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
	{
		int submeshAddressRelative = submeshAddressArray[submeshIndex] - mdlMeshDataAddress;

		#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
		Console::WriteLine("submeshAddressRelative = {0}", submeshAddressRelative);
		Console::WriteLine("submeshAddressArray[submeshIndex] = {0}", submeshAddressArray[submeshIndex]);
		Console::WriteLine("mdlMeshDataAddress = {0}", mdlMeshDataAddress);
		#endif

		String^ submeshName = "";
		for(int i=0; i<SUBMESH_NAME_LENGTH; i++)
		{
			submeshName = submeshName + Char(mdlMeshData[submeshAddressRelative+i]);
		}
		//remove appending bad characters from submesh name
		int lastIndexOfRealCharacterInName = 0;
		bool finishedReadingRealCharactersInName = false;
		for(int i=0; i<submeshName->Length; i++)
		{
			if(!finishedReadingRealCharactersInName)
			{
				if((int(submeshName[i]) > 48) && (int(submeshName[i]) < 122))
				{
					lastIndexOfRealCharacterInName++;
				}
				else
				{
					finishedReadingRealCharactersInName = true;
				}
			}
		}
		submeshName = submeshName->Substring(0, lastIndexOfRealCharacterInName);
		Console::WriteLine("submeshName = {0}", submeshName);

		submeshNameArray[submeshIndex] = submeshName;

		StreamWriter^ fileObjectLDR;
		StreamWriter^ fileObjectHakenberg;
		//Note submeshes stored in MDL can have the same name
		String^ submeshIndexString = convertIntToManagedString(submeshIndex);
		String^ fileNameLDR = outputFileNameBase + "-" + submeshIndexString + "-" + submeshName + LDR_EXTENSION;
		String^ fileNameHakenberg = outputFileNameBase + "-" + submeshIndexString + "-" + submeshName + HAK_EXTENSION;
		if(!importMesh)
		{
			fileObjectLDR = gcnew StreamWriter(File::Open(fileNameLDR, FileMode::Create));
			fileObjectHakenberg = gcnew StreamWriter(File::Open(fileNameHakenberg, FileMode::Create));
		}
		#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
		Console::WriteLine("submeshNameArray[submeshIndex] = {0}", submeshNameArray[submeshIndex]);
		#endif

		array<unsigned char>^ tempByteArray1 = { mdlMeshData[submeshAddressRelative+0x60], mdlMeshData[submeshAddressRelative+0x61], mdlMeshData[submeshAddressRelative+0x62], mdlMeshData[submeshAddressRelative+0x63] };
		int numberOfVertices = BitConverter::ToUInt32(tempByteArray1, 0);
		array<unsigned char>^ tempByteArray2 = { mdlMeshData[submeshAddressRelative+0x64],  mdlMeshData[submeshAddressRelative+0x65], mdlMeshData[submeshAddressRelative+0x66], mdlMeshData[submeshAddressRelative+0x67] };
		int numberOfUVcoords = BitConverter::ToUInt32(tempByteArray2, 0);
		array<unsigned char>^ tempByteArray3 = { mdlMeshData[submeshAddressRelative+0x68], mdlMeshData[submeshAddressRelative+0x69], mdlMeshData[submeshAddressRelative+0x6A], mdlMeshData[submeshAddressRelative+0x6B] };
		int numberOfFaces = BitConverter::ToUInt32(tempByteArray3, 0);
		int numberOfLightingEntries = numberOfVertices;

		//#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
		Console::WriteLine("numberOfVertices = {0}", numberOfVertices);
		Console::WriteLine("numberOfUVcoords = {0}", numberOfUVcoords);
		Console::WriteLine("numberOfFaces = {0}", numberOfFaces);
		Console::WriteLine("numberOfLightingEntries = {0}", numberOfLightingEntries);
		//#endif
		int estimatedSubmeshSize = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + numberOfVertices*SUBMESH_DATAPOINTS_PER_VERTEX*BYTES_PER_INT + numberOfUVcoords*SUBMESH_DATAPOINTS_PER_UV_COORDS*BYTES_PER_INT + numberOfFaces*SUBMESH_DATAPOINTS_PER_FACE*BYTES_PER_INT + numberOfLightingEntries*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY*BYTES_PER_INT;
		//Console::WriteLine("estmated submesh size = {0}", estimatedSubmeshSize);


		if(!importMesh)
		{
			String^ fileHAKsectionHeader = "% vertices";
			fileObjectHakenberg->WriteLine(fileHAKsectionHeader);
			fileHAKsectionHeader = "v 3 ";
			fileHAKsectionHeader = fileHAKsectionHeader + convertIntToManagedString(numberOfVertices);
			fileObjectHakenberg->WriteLine(fileHAKsectionHeader);
		}
		String^ fileHAKLine = "";

		int submeshVerticesArraySize = numberOfVertices*SUBMESH_DATAPOINTS_PER_VERTEX;	//FloatX,FloatY,FloatZ
		int submeshUVcoordsArraySize = numberOfUVcoords*SUBMESH_DATAPOINTS_PER_UV_COORDS;	//FloatU,FloatV
		int submeshFaceArraySize = numberOfFaces*10;	//4ByteVertex#1,4ByteVertex#2,4ByteVertex#3,4ByteUVMap#1,4ByteUVMap#2,4ByteUVMap#3,4ByteUnknown,4ByteFACENormalMappingX,4ByteFACENormalMappingY,4ByteFACENormalMappingZ
		int submeshLightingEntriesArraySize = numberOfLightingEntries*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY;	//Float*4 or Int*4?
		Console::WriteLine("submeshVerticesArraySizeBytes = {0}", submeshVerticesArraySize*BYTES_PER_INT);
		Console::WriteLine("submeshUVcoordsArraySizeBytes = {0}", submeshUVcoordsArraySize*BYTES_PER_INT);
		Console::WriteLine("submeshFaceArraySizeBytes = {0}", submeshFaceArraySize*BYTES_PER_INT);
		Console::WriteLine("submeshLightingEntriesArraySizeBytes = {0}", submeshLightingEntriesArraySize*BYTES_PER_INT);
		array<float>^ submeshVerticesArray = gcnew array<float>(submeshVerticesArraySize);
		array<float>^ submeshUVcoordsArray = gcnew array<float>(submeshUVcoordsArraySize);
		array<float>^ submeshFaceArray = gcnew array<float>(numberOfFaces*SUBMESH_DATAPOINTS_PER_FACE);
		array<int>^ submeshLightingEntriesArray = gcnew array<int>(numberOfLightingEntries*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY);
		int submeshAddressRelativeVertices = submeshAddressRelative + SUBMESH_HEADER_VERTICES_LIST_ADDRESS;
		int submeshAddressRelativeUVcoords = submeshAddressRelative + SUBMESH_HEADER_VERTICES_LIST_ADDRESS + submeshVerticesArraySize*BYTES_PER_INT;
		int submeshAddressRelativeFaces = submeshAddressRelative + SUBMESH_HEADER_VERTICES_LIST_ADDRESS + submeshVerticesArraySize*BYTES_PER_INT + submeshUVcoordsArraySize*BYTES_PER_INT;
		int submeshAddressRelativeLightingEntries = submeshAddressRelative + SUBMESH_HEADER_VERTICES_LIST_ADDRESS + submeshVerticesArraySize*BYTES_PER_INT + submeshUVcoordsArraySize*BYTES_PER_INT + submeshFaceArraySize*BYTES_PER_INT;	//added 26 Feb 2013

		//Console::WriteLine("submeshVerticesArraySize = {0}", submeshVerticesArraySize);

		for(int i=0; i<submeshVerticesArraySize; i++)
		{
			int currentAddress = submeshAddressRelativeVertices+(i*BYTES_PER_INT);
			array<unsigned char>^ tempByteArray = { mdlMeshData[currentAddress+0], mdlMeshData[currentAddress+1], mdlMeshData[currentAddress+2], mdlMeshData[currentAddress+3] };
			submeshVerticesArray[i] = bytesToFloat(tempByteArray);
			#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
			Console::WriteLine("submeshVerticesArray[i] = {0}", submeshVerticesArray[i]);
			#endif

			if(!importMesh)
			{
				String^ tempString = convertFloatToManagedString(submeshVerticesArray[i]);
				fileHAKLine = fileHAKLine + tempString + " ";
				if(i%SUBMESH_DATAPOINTS_PER_VERTEX == SUBMESH_DATAPOINTS_PER_VERTEX-1)
				{
					#ifdef MDL_DEVEL_OUTPUT_3D_MESH_LDR_POINT_MAP
					String^ fileLDRLine = "1 0 " + fileHAKLine + " 1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 point.dat";
					fileObjectLDR->WriteLine(fileLDRLine);
					#endif
					fileObjectHakenberg->WriteLine(fileHAKLine);
					fileHAKLine = "";
				}
			}
		}
		#ifdef MDL_DEVEL_EXPORT_UV_COORD_ARRAY_TO_HAK_AND_LDR_FILE
		fileHAKLine = "";
		if(!importMesh)
		{
			String^ fileHAKsectionHeader = "% UVcoords";
			fileObjectHakenberg->WriteLine(fileHAKsectionHeader);

			String^ fileLDRLine = "0 start UV coords";
			fileObjectLDR->WriteLine(fileLDRLine);
		}
		#endif
		for(int i=0; i<submeshUVcoordsArraySize; i++)
		{
			int currentAddress = submeshAddressRelativeUVcoords+(i*BYTES_PER_INT);
			array<unsigned char>^ tempByteArray = { mdlMeshData[currentAddress+0], mdlMeshData[currentAddress+1], mdlMeshData[currentAddress+2], mdlMeshData[currentAddress+3] };
			submeshUVcoordsArray[i] = bytesToFloat(tempByteArray);
			#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
			Console::WriteLine("submeshUVcoordsArray[i] = {0}", submeshUVcoordsArray[i]);
			#endif

			#ifdef MDL_DEVEL_EXPORT_UV_COORD_ARRAY_TO_HAK_AND_LDR_FILE
			if(!importMesh)
			{
				float UVvalue = submeshUVcoordsArray[i];
				#ifdef MDL_DEVEL_EXPORT_UV_COORD_ARRAY_TO_HAK_FILE_NORMALISE_WRT_FLAT_TEXTURE_512x256_DOESNT_WORK
				if(i%SUBMESH_DATAPOINTS_PER_UV_COORDS == 0)
				{
					UVvalue = 512.0F*submeshUVcoordsArray[i];
				}
				else
				{
					UVvalue = 256.0F*submeshUVcoordsArray[i];
				}
				if((UVvalue > SOME_LARGE_DISTANCE) || (UVvalue < -(SOME_LARGE_DISTANCE)))
				{//compensate for errors in UV value...
					UVvalue = 0.0;
				}
				//Console::WriteLine("UVvalue = {0}", UVvalue);
				#endif
				String^ tempString = convertFloatToManagedString(UVvalue);
				fileHAKLine = fileHAKLine + tempString + " ";
				if(i%SUBMESH_DATAPOINTS_PER_UV_COORDS == SUBMESH_DATAPOINTS_PER_UV_COORDS-1)
				{
					String^ fileLDRLine = "1 0 " + fileHAKLine + " 0.0" + " 0.01 0.0 0.0 0.0 0.01 0.0 0.0 0.0 0.01 point.dat";
					fileObjectLDR->WriteLine(fileLDRLine);

					fileObjectHakenberg->WriteLine(fileHAKLine);
					fileHAKLine = "";
				}
			}
			#endif
		}
		#ifdef MDL_DEVEL_EXPORT_UV_COORD_ARRAY_TO_HAK_AND_LDR_FILE
		fileHAKLine = "";
		if(!importMesh)
		{
			String^ fileLDRLine = "0 end UV coords";
			fileObjectLDR->WriteLine(fileLDRLine);
		}
		#endif

		if(!importMesh)
		{
			String^ fileHAKsectionHeader = "% triangles";
			fileObjectHakenberg->WriteLine(fileHAKsectionHeader);
			fileHAKsectionHeader = "f 2 ";
			fileHAKsectionHeader = fileHAKsectionHeader + convertIntToManagedString(submeshFaceArraySize/SUBMESH_DATAPOINTS_PER_FACE);
			fileObjectHakenberg->WriteLine(fileHAKsectionHeader);
		}
		fileHAKLine = "";
		String^ fileLDRLine = "";

		#ifdef MDL_DEVEL_DETERMINE_WHICH_UV_COORDINATES_ARE_BEING_REFERENCED_BY_FACES
		//debug only: determine which UV coordinates are being referenced by faces (a minority), and which are not (the majority)
		int minUVcoordIndex = int(SOME_LARGE_DISTANCE);
		int maxUVcoordIndex = 0;
		#endif

		for(int i=0; i<submeshFaceArraySize; i++)
		{
			int currentAddress = submeshAddressRelativeFaces+(i*BYTES_PER_INT);
			array<unsigned char>^ tempByteArray = { mdlMeshData[currentAddress+0], mdlMeshData[currentAddress+1], mdlMeshData[currentAddress+2], mdlMeshData[currentAddress+3] };

			int faceInternalDataPointIndex = i%SUBMESH_DATAPOINTS_PER_FACE;
			#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
			Console::WriteLine("faceInternalDataPointIndex = {0}", faceInternalDataPointIndex);
			#endif

			if(faceInternalDataPointIndex == 0)
			{
				fileHAKLine = "";
				fileLDRLine = "3 0 ";	//LDR lines require indication of polygon type and color (tri/3 + default/0)
			}
			if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD)
			{
				//get Vertex index
				int vertexIndex = BitConverter::ToUInt32(tempByteArray, 0);
				submeshFaceArray[i] = float(vertexIndex);
				#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
				Console::WriteLine("vertexIndex = {0}", vertexIndex);
				#endif
				if(!importMesh)
				{
					String^ tempString = convertIntToManagedString(vertexIndex);
					fileHAKLine = fileHAKLine + tempString + " ";
				}

				for(int j=0; j<SUBMESH_DATAPOINTS_PER_VERTEX; j++)
				{
					#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
					Console::WriteLine("submeshFaceArray[i] => submeshVerticesArray[vertexIndex*SUBMESH_DATAPOINTS_PER_VERTEX+j] = {0}", submeshVerticesArray[vertexIndex*SUBMESH_DATAPOINTS_PER_VERTEX+j]);
					#endif
					if(!importMesh)
					{
						String^ tempString = convertFloatToManagedString(submeshVerticesArray[vertexIndex*SUBMESH_DATAPOINTS_PER_VERTEX+j]);
						fileLDRLine = fileLDRLine + tempString + " ";
					}
				}
			}
			else if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
			{
				//get UVcoord index
				int UVcoordsIndex = BitConverter::ToUInt32(tempByteArray, 0);
				submeshFaceArray[i] = float(UVcoordsIndex);

				#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
				Console::WriteLine("UVcoordsIndex = {0}", UVcoordsIndex);
				for(int j=0; j<SUBMESH_DATAPOINTS_PER_UV_COORDS; j++)
				{
					Console::WriteLine("submeshFaceArray[i] => submeshUVcoordsArray[UVcoordsIndex*SUBMESH_DATAPOINTS_PER_UV_COORDS+j] = {0}", submeshUVcoordsArray[UVcoordsIndex*SUBMESH_DATAPOINTS_PER_UV_COORDS+j]);
				}
				#endif

				#ifdef MDL_DEVEL_DETERMINE_WHICH_UV_COORDINATES_ARE_BEING_REFERENCED_BY_FACES
				Console::WriteLine("UVcoordsIndex = {0}", UVcoordsIndex);
				if(UVcoordsIndex < minUVcoordIndex)
				{
					minUVcoordIndex = UVcoordsIndex;
				}
				if(UVcoordsIndex > maxUVcoordIndex)
				{
					maxUVcoordIndex = UVcoordsIndex;
				}
				#endif

			}
			else if(faceInternalDataPointIndex == SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
			{
				#ifdef MDL_DEVEL_IMPORT_DESTROY_ALL_UV_NORMALS_AND_LIGHTING_DATA
				submeshFaceArray[i] = 0.0F;
				#else
				//get UNKNOWN/undefined value
				int unknownInt = BitConverter::ToUInt32(tempByteArray, 0);
				submeshFaceArray[i] = float(unknownInt);
				//submeshFaceArray[i] =  bytesToFloat(tempByteArray);	//OLD
				#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
				Console::WriteLine("submeshFaceArray[i] = UNKNOWN = {0}", submeshFaceArray[i]);
				#endif
				#endif
			}
			else
			{//normal data
				submeshFaceArray[i] =  bytesToFloat(tempByteArray);
				#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
				Console::WriteLine("submeshFaceArray[i] = {0}", submeshFaceArray[i]);
				#endif
			}

			if(!importMesh)
			{
				if(faceInternalDataPointIndex == SUBMESH_DATAPOINTS_PER_FACE-1)
				{
					fileObjectHakenberg->WriteLine(fileHAKLine);	//HAK record vertex indicies for face
					#ifndef MDL_DEVEL_EXPORT_UV_COORD_ARRAY_TO_HAK_AND_LDR_FILE
					#ifndef MDL_DEVEL_OUTPUT_3D_MESH_LDR_POINT_MAP
					fileObjectLDR->WriteLine(fileLDRLine);	//LDR record verticies for face
					#endif
					#endif
				}
			}
		}

		#ifdef MDL_DEVEL_DETERMINE_WHICH_UV_COORDINATES_ARE_BEING_REFERENCED_BY_FACES
		Console::WriteLine("minUVcoordIndex = {0}", minUVcoordIndex);
		Console::WriteLine("maxUVcoordIndex = {0}", maxUVcoordIndex);
		#endif

		for(int i=0; i<submeshLightingEntriesArraySize; i++)
		{
			int currentAddress = submeshAddressRelativeLightingEntries+(i*BYTES_PER_INT);
			array<unsigned char>^ tempByteArray = { mdlMeshData[currentAddress+0], mdlMeshData[currentAddress+1], mdlMeshData[currentAddress+2], mdlMeshData[currentAddress+3] };
			submeshLightingEntriesArray[i] = BitConverter::ToUInt32(tempByteArray, 0);	//bytesToFloat(tempByteArray);	//unsure if this is supposed to be integer or floating point form...
			#ifdef MDL_DEVEL_PRINT_3D_MESH_DATA
			Console::WriteLine("submeshLightingEntriesArray[i] = {0}", submeshLightingEntriesArray[i]);
			#endif
		}

		if(!importMesh)
		{
			fileObjectLDR->Close();
			fileObjectHakenberg->Close();
		}
		if(importMesh)
		{
			Console::WriteLine("start importHAKfile");
			//for importMesh only:
			array<float>^ newsubmeshVerticesArray;
			array<float>^ newsubmeshUVcoordsArray;
			array<float>^ newsubmeshFaceArray;
			array<int>^ newsubmeshLightingEntriesArray;
			if(importMesh)
			{
				importHAKfile(fileNameHakenberg, newsubmeshVerticesArray, newsubmeshFaceArray);
			}

			Console::WriteLine("start formatNew3DmeshData");
			formatNew3DmeshData(submeshName, submeshVerticesArray, submeshUVcoordsArray, submeshFaceArray, submeshLightingEntriesArray, newsubmeshVerticesArray, newsubmeshUVcoordsArray, newsubmeshFaceArray, newsubmeshLightingEntriesArray);
			Console::WriteLine("done formatNew3DmeshData");


			int newsubmeshVerticesArraySize = newsubmeshVerticesArray->Length;
			int newsubmeshUVcoordsArraySize = newsubmeshUVcoordsArray->Length;
			int newsubmeshFaceArraySize = newsubmeshFaceArray->Length;
			int newsubmeshLightingEntriesArraySize = newsubmeshLightingEntriesArray->Length;
			int newnumberOfVertices = newsubmeshVerticesArraySize/SUBMESH_DATAPOINTS_PER_VERTEX;
			int newnumberOfUVcoords = newsubmeshUVcoordsArraySize/SUBMESH_DATAPOINTS_PER_UV_COORDS;
			int newnumberOfFaces = newsubmeshFaceArraySize/SUBMESH_DATAPOINTS_PER_FACE;
			int newnumberOfLightingEntries = newsubmeshLightingEntriesArraySize/SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY;

			//Console::WriteLine("submeshLightingEntriesArraySize = {0}", submeshLightingEntriesArraySize);
			//Console::WriteLine("newsubmeshLightingEntriesArraySize = {0}", newsubmeshLightingEntriesArraySize);

			Console::WriteLine("newnumberOfVertices = {0}", newnumberOfVertices);
			Console::WriteLine("newnumberOfUVcoords = {0}", newnumberOfUVcoords);
			Console::WriteLine("newnumberOfFaces = {0}", newnumberOfFaces);
			Console::WriteLine("newnumberOfLightingEntries = {0}", newnumberOfLightingEntries);

			//added 25 Feb (there appears to be some additional data corresponding to each submesh)
			int newsubmeshSize = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + newsubmeshVerticesArraySize*BYTES_PER_INT + newsubmeshUVcoordsArraySize*BYTES_PER_INT + newsubmeshFaceArraySize*BYTES_PER_INT + newsubmeshLightingEntriesArraySize*BYTES_PER_INT;
			int submeshSize = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + submeshVerticesArraySize*BYTES_PER_INT + submeshUVcoordsArraySize*BYTES_PER_INT + submeshFaceArraySize*BYTES_PER_INT + submeshLightingEntriesArraySize*BYTES_PER_INT;
			Console::WriteLine("newsubmeshSize = {0}", newsubmeshSize);
			Console::WriteLine("submeshSize = {0}", submeshSize);

			array<unsigned char>^ newsubmesh = gcnew array<unsigned char>(newsubmeshSize);
			newsubmeshArray[submeshIndex] = newsubmesh;

			int newsubmeshCurrentIndex = 0;

			for(int i=0; i<SUBMESH_HEADER_NUMBER_OF_VERTICES_ADDRESS; i++)
			{
				newsubmesh[i] = mdlMeshData[submeshAddressRelative+i];	//copy the exact same name as the original sub mesh
				newsubmeshCurrentIndex++;
			}
			Console::WriteLine("newnumberOfVertices = {0}", newnumberOfVertices);
			array<unsigned char>^ newsubmeshNumberOfVerticesByteArray = BitConverter::GetBytes(newnumberOfVertices);
			insertByteArray(newsubmesh, newsubmeshNumberOfVerticesByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			array<unsigned char>^ newsubmeshNumberOfUVcoordsByteArray = BitConverter::GetBytes(newnumberOfUVcoords);
			insertByteArray(newsubmesh, newsubmeshNumberOfUVcoordsByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			array<unsigned char>^ newsubmeshNumberOfFacesByteArray = BitConverter::GetBytes(newnumberOfFaces);
			insertByteArray(newsubmesh, newsubmeshNumberOfFacesByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;

			int register7C = SUBMESH_HEADER_VERTICES_LIST_ADDRESS;	//this register is set to the size of the submesh header
			array<unsigned char>^ newsubmeshRegister7CByteArray = BitConverter::GetBytes(register7C);
			insertByteArray(newsubmesh, newsubmeshRegister7CByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			int registerR1 = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + newsubmeshVerticesArraySize*BYTES_PER_INT;	//important second reference to the number of vertices
			array<unsigned char>^ newsubmeshRegisterR1ByteArray = BitConverter::GetBytes(registerR1);
			insertByteArray(newsubmesh, newsubmeshRegisterR1ByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;

			int registerR2 = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + newsubmeshVerticesArraySize*BYTES_PER_INT + newsubmeshUVcoordsArraySize*BYTES_PER_INT;	//important second reference to the number of vertices
			array<unsigned char>^ newsubmeshRegisterR2ByteArray = BitConverter::GetBytes(registerR2);
			insertByteArray(newsubmesh, newsubmeshRegisterR2ByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			int registerR3 = SUBMESH_HEADER_VERTICES_LIST_ADDRESS + newsubmeshVerticesArraySize*BYTES_PER_INT + newsubmeshUVcoordsArraySize*BYTES_PER_INT + newsubmeshFaceArraySize*BYTES_PER_INT;	//important second reference to the number of vertices
			array<unsigned char>^ newsubmeshRegisterR3ByteArray = BitConverter::GetBytes(registerR3);
			insertByteArray(newsubmesh, newsubmeshRegisterR3ByteArray, newsubmeshCurrentIndex);
			newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;

			for(int i=0; i<newsubmeshVerticesArraySize; i++)
			{
				array<unsigned char>^ tempByteArray = BitConverter::GetBytes(newsubmeshVerticesArray[i]);
				insertByteArray(newsubmesh, tempByteArray, newsubmeshCurrentIndex);
				newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			}
			for(int i=0; i<newsubmeshUVcoordsArraySize; i++)
			{
				array<unsigned char>^ tempByteArray = BitConverter::GetBytes(newsubmeshUVcoordsArray[i]);
				insertByteArray(newsubmesh, tempByteArray, newsubmeshCurrentIndex);
				newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			}
			for(int i=0; i<newsubmeshFaceArraySize; i++)
			{
				array<unsigned char>^ tempByteArray;
				int faceInternalDataPointIndex = i%SUBMESH_DATAPOINTS_PER_FACE;
				if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD)
				{
					tempByteArray = BitConverter::GetBytes(int(newsubmeshFaceArray[i]));
				}
				else if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
				{
					tempByteArray = BitConverter::GetBytes(int(newsubmeshFaceArray[i]));
				}
				else if(faceInternalDataPointIndex == SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
				{
					tempByteArray = BitConverter::GetBytes(int(newsubmeshFaceArray[i]));
				}
				else
				{
					tempByteArray = BitConverter::GetBytes(newsubmeshFaceArray[i]);
				}
				insertByteArray(newsubmesh, tempByteArray, newsubmeshCurrentIndex);
				newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			}
			for(int i=0; i<newsubmeshLightingEntriesArraySize; i++)
			{
				array<unsigned char>^ tempByteArray = BitConverter::GetBytes(newsubmeshLightingEntriesArray[i]);
				insertByteArray(newsubmesh, tempByteArray, newsubmeshCurrentIndex);
				newsubmeshCurrentIndex = newsubmeshCurrentIndex + BYTES_PER_INT;
			}

			Console::WriteLine("newsubmesh->Length = {0}", newsubmesh->Length);
		}
	}

	//create mdlMeshDataNew
	if(importMesh)
	{
		Console::WriteLine("create mdlMeshDataNew");

		//1. initialise mdlMeshDataNew
		int totalSizeOfSubmeshArrays = 0;
		for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
		{
			array<unsigned char>^ currentnewsubmesh = newsubmeshArray[submeshIndex];
			totalSizeOfSubmeshArrays = totalSizeOfSubmeshArrays + currentnewsubmesh->Length;
		}
		Console::WriteLine("totalSizeOfSubmeshArrays = {0}", totalSizeOfSubmeshArrays);

		//debug:
		int totalSizeOfSubmeshArraysDebug = submeshAddressArray[numberOfSubmeshes-1] - submeshAddressArray[0] + newsubmeshArray[numberOfSubmeshes-1]->Length;
		Console::WriteLine("numberOfSubmeshes = {0}", numberOfSubmeshes);
		Console::WriteLine("submeshAddressArray[1]-submeshAddressArray[0] = {0}", submeshAddressArray[1]-submeshAddressArray[0]);
		Console::WriteLine("newsubmeshArray[0]->Length = {0}", newsubmeshArray[0]->Length);
		Console::WriteLine("totalSizeOfSubmeshArraysDebug = {0}", totalSizeOfSubmeshArraysDebug);

		int mdlMeshDataHeaderSize = numberOfSubmeshes*BYTES_PER_INT;	//list of 4 byte submesh addresses
		int mdlMeshDataNewSize = mdlMeshDataHeaderSize + totalSizeOfSubmeshArrays;
		Console::WriteLine("numberOfSubmeshes = {0}", numberOfSubmeshes);
		Console::WriteLine("mdlMeshDataHeaderSize = {0}", mdlMeshDataHeaderSize);
		Console::WriteLine("totalSizeOfSubmeshArrays = {0}", totalSizeOfSubmeshArrays);
		Console::WriteLine("mdlMeshDataNewSize = {0}", mdlMeshDataNewSize);

		mdlMeshDataNew = gcnew array<unsigned char>(mdlMeshDataNewSize);
		array<unsigned char>^ mdlMeshDataNewHeader = gcnew array<unsigned char>(mdlMeshDataHeaderSize);

		array<int>^ newsubmeshAddressArray = gcnew array<int>(numberOfSubmeshes);	//store for testing only

		//2. a) calculate total size of submeshes and b) fill in mdlMeshDataNew header (submesh addresses)
		int newsubmeshAddressReferenceAddress = 0;
		int mdlMeshDataSubmeshAddressCurrent = mdlMeshDataAddress + mdlMeshDataHeaderSize;
		for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
		{
			array<unsigned char>^ currentnewsubmesh = newsubmeshArray[submeshIndex];
			array<unsigned char>^ currentsubmeshAddressByteArray = BitConverter::GetBytes(mdlMeshDataSubmeshAddressCurrent);
			insertByteArray(mdlMeshDataNewHeader, currentsubmeshAddressByteArray, newsubmeshAddressReferenceAddress);
			insertByteArray(mdlMeshDataNew, currentsubmeshAddressByteArray, newsubmeshAddressReferenceAddress);
			newsubmeshAddressArray[submeshIndex] = mdlMeshDataSubmeshAddressCurrent;	//store for testing only

			mdlMeshDataSubmeshAddressCurrent = mdlMeshDataSubmeshAddressCurrent + currentnewsubmesh->Length;

			newsubmeshAddressReferenceAddress = newsubmeshAddressReferenceAddress + BYTES_PER_INT;
		}

		int mdlMeshDataNewIndex = newsubmeshAddressReferenceAddress;	//add newsubmesh data directly after submesh address references
		//3. fill in mdlMeshDataNew (data) with newsubmesh data
		for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
		{
			array<unsigned char>^ currentnewsubmesh = newsubmeshArray[submeshIndex];
			for(int i=0; i<currentnewsubmesh->Length; i++)
			{
				mdlMeshDataNew[mdlMeshDataNewIndex] = currentnewsubmesh[i];
				mdlMeshDataNewIndex++;
			}
		}
		Console::WriteLine("mdlMeshDataNew->Length = {0}", mdlMeshDataNew->Length);

		for(int submeshIndex=0; submeshIndex<numberOfSubmeshes; submeshIndex++)
		{
			Console::WriteLine("submeshAddressArray[submeshIndex], newsubmeshAddressArray[submeshIndex]  = {0} {1}", submeshAddressArray[submeshIndex], newsubmeshAddressArray[submeshIndex]);
		}
	}
}

static void insertByteArray(array<unsigned char>^ byteArrayToUpdate, array<unsigned char>^ byteArrayToInsert, int index)
{
	for(int i=0; i<byteArrayToInsert->Length; i++)
	{
		byteArrayToUpdate[i+index] = byteArrayToInsert[i];
	}
}

