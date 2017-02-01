/*******************************************************************************
 *
 * File Name: qfg5modelMeshImport.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: 3D mesh input functions
 *
 *******************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "qfg5modelMath.hpp"
#include "qfg5modelMeshImport.hpp"

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

static int readLineOfFloats(String^ currentLine, array<float>^ floatArray, int numberOfFloats, int arrayIndex);

void importHAKfile(System::String^ fileNameHakenberg, array<float>^ % newsubmeshVerticesArray, array<float>^ % newsubmeshFaceArray)
{
	#ifdef MDL_IMPORT_HAK_FILE_EXPECT_APPENDED_DOT_OUT_TO_FILE_NAME
	fileNameHakenberg = fileNameHakenberg + ".out";
	#endif

	#ifdef MDL_DEVEL_IMPORT_HAK_FILE
	Console::WriteLine("fileNameHakenberg = {0}", fileNameHakenberg);
	#endif
	StreamReader^ fileObjectHakenberg = File::OpenText(fileNameHakenberg);

	bool readingVertices = false;
	bool readingTriangles = false;
	int numberOfVertices = 0;
	int numberOfTriangles = 0;
	int arrayIndex = 0;
	String^ currentLine = "";
	bool previousCharNewLine = false;
	while((currentLine = fileObjectHakenberg->ReadLine()) != nullptr)
	{
		//Console::WriteLine("currentLine = {0}", currentLine);

		bool finishReadingArray = false;
		if(currentLine == "")
		{
			if(previousCharNewLine)
			{
				finishReadingArray = true;
			}
			previousCharNewLine = true;
		}
		else if(currentLine[0] == HAK_HEADER_COMMENT_CHAR)
		{
			//ignore commented line
			finishReadingArray = true;
		}
		else if(currentLine[0] == HAK_HEADER_VERTICES_CHAR)
		{
			if(currentLine[2] == HAK_HEADER_VERTICES_TYPE_CHAR)
			{
				readingVertices = true;
				String^ numberOfVerticesString = currentLine->Substring(4, currentLine->Length-4);
				numberOfVertices = Convert::ToInt32(numberOfVerticesString);
				#ifdef MDL_DEVEL_IMPORT_HAK_FILE
				Console::WriteLine("numberOfVertices = {0}", numberOfVertices);
				#endif
				int submeshVerticesArraySize = numberOfVertices*SUBMESH_DATAPOINTS_PER_VERTEX;
				newsubmeshVerticesArray = gcnew array<float>(submeshVerticesArraySize);	//initialise array
				arrayIndex = 0;
			}
			else
			{
				finishReadingArray = true;
				//ignore
			}
		}
		else if(currentLine[0] == HAK_HEADER_TRIANGES_CHAR)
		{
			if(currentLine[2] == HAK_HEADER_TRIANGES_TYPE_CHAR)
			{
				readingTriangles = true;
				String^ numberOfTrianglesString = currentLine->Substring(4, currentLine->Length-4);
				numberOfTriangles = Convert::ToInt32(numberOfTrianglesString);
				#ifdef MDL_DEVEL_IMPORT_HAK_FILE
				Console::WriteLine("numberOfTriangles = {0}", numberOfTriangles);
				#endif
				int submeshFacesArraySize = numberOfTriangles*SUBMESH_DATAPOINTS_PER_FACE;
				newsubmeshFaceArray = gcnew array<float>(submeshFacesArraySize);	//initialise array
				arrayIndex = 0;
			}
			else
			{
				finishReadingArray = true;
				//ignore
			}
		}
		else
		{
			if(readingVertices)
			{
				arrayIndex = readLineOfFloats(currentLine, newsubmeshVerticesArray, SUBMESH_DATAPOINTS_PER_VERTEX, arrayIndex);
				/*
				for(int i=arrayIndex-SUBMESH_DATAPOINTS_PER_VERTEX; i<arrayIndex; i++)
				{
					Console::WriteLine("readingVertices newsubmeshVerticesArray[i] = {0}", newsubmeshVerticesArray[i]);
				}
				*/
			}
			else if(readingTriangles)
			{
				arrayIndex = readLineOfFloats(currentLine, newsubmeshFaceArray, SUBMESH_DATAPOINTS_VERTICES_PER_FACE, arrayIndex);
				/*
				for(int i=arrayIndex-SUBMESH_DATAPOINTS_VERTICES_PER_FACE; i<arrayIndex; i++)
				{
					Console::WriteLine("readingTriangles newsubmeshFaceArray[i] = {0}", newsubmeshFaceArray[i]);
				}
				*/
				arrayIndex = arrayIndex + (SUBMESH_DATAPOINTS_PER_FACE - SUBMESH_DATAPOINTS_VERTICES_PER_FACE);	//skip UV coords and normals for now...
			}
		}

		if(finishReadingArray)
		{
			readingVertices = false;
			readingTriangles = false;
			arrayIndex = 0;
		}

		if(currentLine != "")
		{
			previousCharNewLine = false;
		}
	}

	fileObjectHakenberg->Close();
}

static int readLineOfFloats(String^ currentLine, array<float>^ floatArray, int numberOfFloats, int arrayIndex)
{
	int startOfdataPoint = 0;
	int endOfdataPoint = -1;

	//Console::WriteLine("at");

	for(int j=0; j<numberOfFloats; j++)
	{
		startOfdataPoint = endOfdataPoint+1;
		endOfdataPoint = currentLine->IndexOf(' ', startOfdataPoint);
		String^ dataPointString = currentLine->Substring(startOfdataPoint, endOfdataPoint-startOfdataPoint);
		//Console::WriteLine("dataPointString = {0}", dataPointString);
		float dataPoint;
		if(dataPointString == HAK_BAD_VERTEX)
		{
			Console::WriteLine("HAK_BAD_VERTEX detected");
			dataPoint = 1.0F;
		}
		else
		{
			dataPoint = Convert::ToSingle(dataPointString);
		}
		//Console::WriteLine("dataPoint = {0}", dataPointString);
		floatArray[arrayIndex] = dataPoint;
		arrayIndex++;
	}
	return arrayIndex;
}

/*
Algorithm:
generate i) UV mappings, ii) normals, iii) lighting entries for each new polygon/face
Regarding Comments: "tempUVmappingBoundaryTriangleFace" = "submeshUVmappingPoly"
*/
//preconditions: newsubmeshFaceArray->Length >= submeshFaceArray->Length
void formatNew3DmeshData(String^ submeshName, array<float>^ % submeshVerticesArray, array<float>^ % submeshUVcoordsArray, array<float>^ % submeshFaceArray, array<int>^ % submeshLightingEntriesArray, array<float>^ % newsubmeshVerticesArray, array<float>^ % newsubmeshUVcoordsArray, array<float>^ % newsubmeshFaceArray, array<int>^ % newsubmeshLightingEntriesArray)
{
	array<poly>^ submeshVerticesPolyArray = gcnew array<poly>(submeshFaceArray->Length/SUBMESH_DATAPOINTS_PER_FACE);
	array<poly>^ newsubmeshVerticesPolyArray = gcnew array<poly>(newsubmeshFaceArray->Length/SUBMESH_DATAPOINTS_PER_FACE);
	array<poly>^ submeshVerticesUVcoordsPolyArray = gcnew array<poly>(submeshFaceArray->Length/SUBMESH_DATAPOINTS_PER_FACE);
	fillSubmeshVerticesPolygonArray(submeshFaceArray, submeshVerticesArray, submeshVerticesPolyArray);
	fillSubmeshVerticesPolygonArray(newsubmeshFaceArray, newsubmeshVerticesArray, newsubmeshVerticesPolyArray);
	fillSubmeshUVcoordsPolygonArray(submeshFaceArray, submeshUVcoordsArray, submeshVerticesUVcoordsPolyArray);

	array<vec>^ submeshUVmappingVerticesArray = gcnew array<vec>(submeshVerticesArray->Length);
	array<poly>^ submeshUVmappingPolyArray = gcnew array<poly>(submeshVerticesPolyArray->Length);
	array<int>^ newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly = gcnew array<int>(newsubmeshVerticesPolyArray->Length);	//newsubmeshVerticesPolyArray's mapping with submeshUVmappingPolyArray is stored for future lookup
	array<int>^ newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPolyMethod = gcnew array<int>(newsubmeshVerticesPolyArray->Length);	//method 1 or 2
	#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
	array<bool>^ submeshUVmappingPolyArrayFoundIdenticalPnew = gcnew array<bool>(submeshUVmappingPolyArray->Length);	//used to compensate for identical polygons between orig submesh and new submesh
	array<bool>^ newsubmeshPolyArrayFoundIdenticalPorig = gcnew array<bool>(newsubmeshVerticesPolyArray->Length);	//used to compensate for identical polygons between orig submesh and new submesh
	#endif
	array<vec>^ submeshUVmappingPolyArrayNormals = gcnew array<vec>(submeshUVmappingPolyArray->Length);

	//use a combination of existing submeshUVcoordsArray data and new entries in submeshUVcoordsArray
	int newsubmeshUVcoordsArrayMaximumSize = submeshUVcoordsArray->Length + (newsubmeshVerticesPolyArray->Length*SUBMESH_DATAPOINTS_PER_UV_COORDS*3); //append additional UV coordinates (and use existing UV coordinate data for every nearby/matched vertex)
	int newsubmeshUVcoordsArrayNewDataIndex =  submeshUVcoordsArray->Length;
	array<float>^ newsubmeshUVcoordsArrayTemp =  gcnew array<float>(newsubmeshUVcoordsArrayMaximumSize);
	//copy shared UV coord data:
	for(int i=0; i<submeshUVcoordsArray->Length; i++)
	{
		newsubmeshUVcoordsArrayTemp[i] = submeshUVcoordsArray[i];
	}

	//determine unknownDataValue (this value corresponds to the subBitmap index for the face - enabling UV coord mapping to occur)
	int unknownDataValue = -1;
	bool unknownDataValueFound = false;
	for(int i=0; i<submeshFaceArray->Length; i++)
	{
		int faceInternalDataPointIndex = i%SUBMESH_DATAPOINTS_PER_FACE;
		if(faceInternalDataPointIndex == SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
		{
			if(unknownDataValueFound)
			{
				if(unknownDataValue != submeshFaceArray[i])
				{
					Console::WriteLine("error: unknownDataValue != submeshFaceArray[i]; a submesh utilises more than one subBitmap");
				}
			}
			else
			{
				unknownDataValue = submeshFaceArray[i];
			}
		}
	}

	int newnumberOfVertices = (newsubmeshVerticesArray->Length)/SUBMESH_DATAPOINTS_PER_VERTEX;
	int numberOfVertices = (submeshVerticesArray->Length)/SUBMESH_DATAPOINTS_PER_VERTEX;
	Console::WriteLine("newnumberOfVertices = {0}", newnumberOfVertices);
	Console::WriteLine("newsubmeshVerticesArray->Length = {0}", newsubmeshVerticesArray->Length);
	newsubmeshLightingEntriesArray = gcnew array<int>(newnumberOfVertices * SUBMESH_NUMBER_OF_LIGHTING_ENTRIES_PER_VERTEX);

	//fill lighting data (currently use the nearest lighting data from original submesh - this should be interpolated in the future)
	for(int iNew=0; iNew<newnumberOfVertices; iNew++)
	{
		int matchingVertexFoundIndexi = 0;
		float minDistance = SOME_LARGE_DISTANCE;
		for(int i=0; i<numberOfVertices; i++)
		{
			vec P1;
			P1.x = submeshVerticesArray[i*SUBMESH_DATAPOINTS_PER_VERTEX + 0];
			P1.y = submeshVerticesArray[i*SUBMESH_DATAPOINTS_PER_VERTEX + 1];
			P1.z = submeshVerticesArray[i*SUBMESH_DATAPOINTS_PER_VERTEX + 2];
			vec P2;
			P2.x = newsubmeshVerticesArray[iNew*SUBMESH_DATAPOINTS_PER_VERTEX + 0];
			P2.y = newsubmeshVerticesArray[iNew*SUBMESH_DATAPOINTS_PER_VERTEX + 1];
			P2.z = newsubmeshVerticesArray[iNew*SUBMESH_DATAPOINTS_PER_VERTEX + 2];

			double distance = calculateTheDistanceBetweenTwoPoints(P1, P2);
			if(distance < minDistance)
			{
				minDistance = distance;
				matchingVertexFoundIndexi = i;
			}
		}

		for(int d=0; d<SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY; d++)
		{
			//newsubmeshLightingEntriesArray[iNew*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY+d] = 0.0F;
			newsubmeshLightingEntriesArray[iNew*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY+d] = submeshLightingEntriesArray[matchingVertexFoundIndexi*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY+d];
		}
	}


	/*
	1. a)
	For each submeshFaceArray vertex, calculate the nearest newsubmeshFaceArray vertex - use this as the temp UV mapping boundary triangle (submeshUVmappingPoly)
	store these faceNew vector references in submeshUVmappingPolyArray (and generate newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly for lookup)
	*/

	Console::WriteLine("1. a) i) calculate submeshUVmappingPolyArrayFoundIdenticalPnew/newsubmeshPolyArrayFoundIdenticalPorig");

	//first search for identical polygons (in the special case that newSubmesh = oldSubmesh)
		//this function only works if verticies in identical polygons are defined in the same order
	#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
	for(int p=0; p<submeshVerticesPolyArray->Length; p++)
	{
		submeshUVmappingPolyArrayFoundIdenticalPnew[p] = false;
	}
	for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
	{
		newsubmeshPolyArrayFoundIdenticalPorig[pNew] = false;
	}
	for(int p=0; p<submeshVerticesPolyArray->Length; p++)
	{
		bool foundAMatchingNewPolygon = false;
		for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
		{
			bool vertexMatchArray[NUM_VERTICES_PER_POLYGON];
			vertexMatchArray[0] = false;
			vertexMatchArray[1] = false;
			vertexMatchArray[2] = false;

			for(int v=0; v<NUM_VERTICES_PER_POLYGON; v++)
			{
				vec vertex = getPolyVertex(submeshVerticesPolyArray[p], v);
				vec vertexNew = getPolyVertex(newsubmeshVerticesPolyArray[pNew], v);
				if(compareVectors(vertex, vertexNew))
				{
					vertexMatchArray[v] = true;
				}
			}
			if(vertexMatchArray[0] && vertexMatchArray[1] && vertexMatchArray[2])
			{//new submesh polygon is identical to original submesh polygon
				//Console::WriteLine("vertexMatchArray true: new submesh polygon is identical to original submesh polygon")
				submeshUVmappingPolyArray[p] = newsubmeshVerticesPolyArray[pNew];
				newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew] = p;	//submeshUVmappingPolyArray's mapping with newsubmeshVerticesPolyArray is stored for future lookup
				submeshUVmappingPolyArrayFoundIdenticalPnew[p] = true;
				newsubmeshPolyArrayFoundIdenticalPorig[pNew] = true;
				foundAMatchingNewPolygon = true;
			}
		}
	}
	#endif

	Console::WriteLine("1. a) ii) calculate submeshUVmappingVerticesArray");

		//NB if polygons are identical; their vertices must be defined in the same order to be detected above
	//generate submeshUVmappingVerticesArray: for every vector in submeshVerticesArray, calculate nearest vector in newsubmeshVerticesArray, and store in table submeshUVmappingVerticesArray
	for(int i=0; i<submeshVerticesArray->Length; i=i+SUBMESH_DATAPOINTS_PER_VERTEX)
	{
		vec vertex;
		vertex.x = submeshVerticesArray[i+0];
		vertex.y = submeshVerticesArray[i+1];
		vertex.z = submeshVerticesArray[i+2];

		float minDistance = SOME_LARGE_DISTANCE;
		vec vertexNewAtMinDistance;
		for(int iNew=0; iNew<newsubmeshVerticesArray->Length; iNew=iNew+SUBMESH_DATAPOINTS_PER_VERTEX)
		{
			vec vertexNew;
			vertexNew.x = newsubmeshVerticesArray[iNew+0];
			vertexNew.y = newsubmeshVerticesArray[iNew+1];
			vertexNew.z = newsubmeshVerticesArray[iNew+2];
			float distance = calculateTheDistanceBetweenTwoPoints(vertex, vertexNew);
			if(distance < minDistance)
			{
				minDistance = distance;
				vertexNewAtMinDistance = vertexNew;
			}
		}
		submeshUVmappingVerticesArray[i/SUBMESH_DATAPOINTS_PER_VERTEX] = vertexNewAtMinDistance;
	}

	Console::WriteLine("1. a) iii) calculate submeshUVmappingPolyArray");

	//generate submeshUVmappingPolyArray: based on submeshUVmappingVerticesArray and submeshVerticesPolyArray
	for(int p=0; p<submeshVerticesPolyArray->Length; p++)
	{
		for(int v=0; v<NUM_VERTICES_PER_POLYGON; v++)
		{
			vec vertex = getPolyVertex(submeshVerticesPolyArray[p], v);
			bool foundMatch = false;
			for(int i=0; i<submeshVerticesArray->Length; i=i+SUBMESH_DATAPOINTS_PER_VERTEX)
			{
				vec vertextest;
				vertextest.x = submeshVerticesArray[i+0];
				vertextest.y = submeshVerticesArray[i+1];
				vertextest.z = submeshVerticesArray[i+2];
				if(compareVectors(vertextest, vertex))
				{
					foundMatch = true;
					setPolyVertex(submeshUVmappingPolyArray[p], v, submeshUVmappingVerticesArray[i/SUBMESH_DATAPOINTS_PER_VERTEX]);
				}
			}

			if(!foundMatch)
			{
				Console::WriteLine("error: = !foundMatch");
				printVector("vertex", vertex);
			}
		}
		#ifdef MDL_DEVEL_DEBUG_MISSING_TRIANGLES
		if(submeshName == "head")
		{
			printPolyLDR("submeshUVmappingPolyArray[p]", submeshUVmappingPolyArray[p]);
		}
		#endif
	}

	Console::WriteLine("1. a) iv) generate newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPol");


	//generate newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly (For each new polygon, calculate the nearest orig/UVmapping polygon vertex)
	for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
	{
		poly newsubmeshPoly = newsubmeshVerticesPolyArray[pNew];
		vec vertexpNewCentre = calculateCentreOfPolygon(newsubmeshVerticesPolyArray[pNew]);

		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		if(!newsubmeshPolyArrayFoundIdenticalPorig[pNew])
		{
		#endif

			//method a) intersectionPointWithsubmeshUVmappingPoly must lie on UV mapping triange (only works for convex shapes: assumes UVmapping polys lie internal to newsubmesh polys)
			//method b) intersectionPointWithsubmeshUVmappingPoly  must lie on UV mapping triange (this is required to compensate for concave shapes: ie UVmapping polys lie external to newsubmesh polys)
			int pAtMinDistance[NUMBER_OF_METHODS] = {0, 0, 0};
			float minDistance[NUMBER_OF_METHODS] = {SOME_LARGE_DISTANCE, SOME_LARGE_DISTANCE, SOME_LARGE_DISTANCE};
			bool foundClosestUVmappingPoly[NUMBER_OF_METHODS] = {false, false, false};

			for(int p=0; p<submeshVerticesPolyArray->Length; p++)
			{
				poly submeshUVmappingPoly = submeshUVmappingPolyArray[p];
				for(int method = 0; method <= METHOD_2; method++)
				{
					vec N;
					if(method == METHOD_1)
					{
						 N = calculateNormalOfPoly(submeshUVmappingPoly);	//method 1
					}
					else if(method == METHOD_2)
					{
						N = calculateNormalOfPoly(newsubmeshPoly);	//method 2 (doesnt work with concave shapes, but works better in general)
					}
					N = multiplyVectorByScalar(N, 100.0);

					bool foundIntesectionPoint = false;
					vec P1 = vertexpNewCentre;
					vec P2 = addVectors(P1, N);
					vec intersectionPointWithsubmeshUVmappingPoly = calculateIntersectionPointOfLineAndPolygonPlane(P1, P2, submeshUVmappingPoly, foundIntesectionPoint);

					if(foundIntesectionPoint)
					{
						float distance = calculateTheDistanceBetweenTwoPoints(intersectionPointWithsubmeshUVmappingPoly, vertexpNewCentre);

						if(distance < minDistance[method])
						{
							if(checkIfPointLiesOnTriangle3D(submeshUVmappingPoly, intersectionPointWithsubmeshUVmappingPoly))
							{
								minDistance[method] = distance;
								pAtMinDistance[method] = p;
								foundClosestUVmappingPoly[method] = true;
							}
						}
					}
				}

				vec vertexpCentre = calculateCentreOfPolygon(submeshUVmappingPoly);
				float distance = calculateTheDistanceBetweenTwoPoints(vertexpCentre, vertexpNewCentre);
				if(distance < minDistance[METHOD_3])
				{
					minDistance[METHOD_3] = distance;
					pAtMinDistance[METHOD_3] = p;
					foundClosestUVmappingPoly[METHOD_3] = true;
				}
			}

			if(foundClosestUVmappingPoly[METHOD_2])
			{
				newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew] = pAtMinDistance[METHOD_2];	//newsubmeshVerticesPolyArray's mapping with submeshUVmappingPolyArray is stored for future lookup
				newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPolyMethod[pNew] = METHOD_2;
			}
			/*
			else if(foundClosestUVmappingPoly[METHOD_1])
			{
				newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew] = pAtMinDistance[METHOD_1];	//newsubmeshVerticesPolyArray's mapping with submeshUVmappingPolyArray is stored for future lookup
				newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPolyMethod[pNew] = METHOD_1;
			}
			*/
			else
			{
				Console::WriteLine("0 error: = !foundClosestUVmappingPoly");

				#ifdef MDL_DEVEL_DEBUG_MISSING_TRIANGLES
				if(submeshName == "head")
				{
					printPolyLDR("newsubmeshVerticesPolyArray[pNew]", newsubmeshVerticesPolyArray[pNew]);
				}
				#endif
			}
		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		}
		#endif
	}
	/*
	#ifdef MDL_DEVEL_DEBUG_MISSING_TRIANGLES
	if(submeshName == "head")
	{
		exit(0);
	}
	#endif
	*/


	/*
	1. b) i)
	For each submeshUVmappingPoly, calculate its normal
	for each newsubmeshVerticesPolyArray vertex, calculate the intersection point "X" of a line drawn in the direction of the newsubmeshPoly [OLD: submeshUVmappingPoly] normal and the 3D plane of its nearest submeshUVmappingPoly
	*/
	Console::WriteLine("1. b) i) For each submeshUVmappingPoly, calculate its normal");
	for(int p=0; p<submeshVerticesPolyArray->Length; p++)
	{
		vec normal;
		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		if(submeshUVmappingPolyArrayFoundIdenticalPnew[p])
		{
			//use original normal values
			for(int j=0; j<SUBMESH_DATAPOINTS_PER_NORMAL; j++)
			{
				float value = submeshFaceArray[p*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+j];
				setVectorVal(normal, j, value);
			}
		}
		else
		{
		#endif
			//regenerate normal values (these will be slightly different than original normal values)
			normal = calculateNormalOfPoly(submeshUVmappingPolyArray[p]);
		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		}
		#endif
		submeshUVmappingPolyArrayNormals[p] = normal;
	}
	Console::WriteLine("1. b) ii) for each newsubmeshVerticesPolyArray vertex, calculate the intersection point \"X\" of a line drawn in the direction of the newsubmeshPoly [OLD: submeshUVmappingPoly] normal and the 3D plane of its nearest submeshUVmappingPoly");
	for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
	{
		poly newsubmeshPoly = newsubmeshVerticesPolyArray[pNew];
		for(int vNew=0; vNew<NUM_VERTICES_PER_POLYGON; vNew++)
		{
			vec vertexNew = getPolyVertex(newsubmeshVerticesPolyArray[pNew], vNew);
			bool newsubmeshVectorIsInsubmeshUVmappingPolyArray = false;
			int pAtMatch = 0;
			int vAtMatch = 0;

			//now lookup the UVmapping poly
			int p = newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew];
			for(int v=0; v<NUM_VERTICES_PER_POLYGON; v++)
			{
				vec vertex = getPolyVertex(submeshUVmappingPolyArray[p], v);
				if(compareVectors(vertex, vertexNew))
				{
					newsubmeshVectorIsInsubmeshUVmappingPolyArray = true;
					pAtMatch = p;
					vAtMatch = v;
				}
			}
			if(newsubmeshVectorIsInsubmeshUVmappingPolyArray)
			{
				//use exact UVsubmeshPoly UVcoord values
				newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+vNew] = submeshFaceArray[pAtMatch*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+vAtMatch];
				newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN] = submeshFaceArray[pAtMatch*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN];
			}
			else
			{//calculate the nearest point and its corresponding polygon on submeshUVmappingPolyArray (with respect to pNew,vNew)

				//Console::WriteLine("\n!newsubmeshVectorIsInsubmeshUVmappingPolyArray");

				//lookup the correct UV mapping polygon
				int p = newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew];

				//calculate the intersection point on the UV mapping polygon of a line drawn from the new polygon point Pnew Vnew in the direction of the normal of the UV mapping polygon
				bool closestIntersectionXPointFound = false;

				vec N;
				int method = newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPolyMethod[pNew];

				if(method == METHOD_1)
				{
					N = submeshUVmappingPolyArrayNormals[p];
				}
				else if(method == METHOD_2)
				{
					N = calculateNormalOfPoly(newsubmeshPoly);
				}
				N = multiplyVectorByScalar(N, 100.0);		//this is required for calculateIntersectionPointOfLineAndPolygonPlane routine to work...

				vec P1 = getPolyVertex(newsubmeshVerticesPolyArray[pNew], vNew);
				vec P2 = addVectors(P1, N);
				vec closestIntersectionXPoint = calculateIntersectionPointOfLineAndPolygonPlane(P1, P2, submeshUVmappingPolyArray[p], closestIntersectionXPointFound);	//intersectionPointWithsubmeshUVmappingPoly

				if(closestIntersectionXPointFound)
				{
					//now calculate corresponding UV information
					/*
					1. b) iii)
						based on linear interpolation of the submeshUVmappingPoly's 3 bounding UV coordinates (X,Y), calculate the appropriate UV coordinate of the intersection point "X" (use this for the newsubmeshVerticesPolyArray vertex)
								{OPTIONB [OLD]: find the nearest UV coordinate in the submeshUVcoordsArray}
								{OPTIONA [ACTIVE]: generate additional UV coodinates and add them to the newsubmeshUVcoordsArrayTemp
								store these new UV coordinates in the submeshFaceArray
					*/

					//Console::WriteLine("\nclosestIntersectionXPointFound");

					vec submeshUVmappingPolyUVvaluesVertexA = getPolyVertex(submeshVerticesUVcoordsPolyArray[p], 0);
					vec submeshUVmappingPolyUVvaluesVertexB = getPolyVertex(submeshVerticesUVcoordsPolyArray[p], 1);
					vec submeshUVmappingPolyUVvaluesVertexC = getPolyVertex(submeshVerticesUVcoordsPolyArray[p], 2);

					//TODO: calculate idealUVvaluesBasedOnLinearInterpolation (calculate UV by tracing 2 vectors of submeshUVmappingPoly by required amounts: ab + bc)
					//calculate vector from ai: from submeshUVmappingPolyUVvalues.a to intersection point closestIntersectionPoint
					vec A = submeshUVmappingPolyArray[p].a;
					vec B = submeshUVmappingPolyArray[p].b;
					vec C = submeshUVmappingPolyArray[p].c;

					//NEW method (use barycentric coordinates)
					bool baryCentricPass = false;
					vec idealUVvaluesBasedOnLinearInterpolation = mapPointFromTriangle1toTriangle2(submeshUVmappingPolyArray[p], closestIntersectionXPoint, submeshVerticesUVcoordsPolyArray[p], baryCentricPass);
					if(baryCentricPass)
					{
						//add new UV coordinate data to array, and reference this new UV coordinate data..
						//search for an identical UV coord previously added, and use it if existent
						bool isExistingNewUVcoord = false;
						int existingNewUVcoordIndex = 0;
						for(int i=0; i<newsubmeshUVcoordsArrayTemp->Length; i=i+SUBMESH_DATAPOINTS_PER_UV_COORDS)
						{
							vec existingNewUVcoord;
							existingNewUVcoord.x = newsubmeshUVcoordsArrayTemp[i];
							existingNewUVcoord.y = newsubmeshUVcoordsArrayTemp[i+1];
							existingNewUVcoord.z = 0.0F;
							idealUVvaluesBasedOnLinearInterpolation.z = 0.0F;	//enforce for safety
							if(compareVectors(existingNewUVcoord, idealUVvaluesBasedOnLinearInterpolation))
							{
								isExistingNewUVcoord = true;
								existingNewUVcoordIndex = i;
							}
						}
						if(isExistingNewUVcoord)
						{
							newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+vNew] = float(existingNewUVcoordIndex/SUBMESH_DATAPOINTS_PER_UV_COORDS);
						}
						else
						{
							newsubmeshUVcoordsArrayTemp[newsubmeshUVcoordsArrayNewDataIndex+0] = idealUVvaluesBasedOnLinearInterpolation.x;
							newsubmeshUVcoordsArrayTemp[newsubmeshUVcoordsArrayNewDataIndex+1] = idealUVvaluesBasedOnLinearInterpolation.y;
							newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+vNew] = float(newsubmeshUVcoordsArrayNewDataIndex/SUBMESH_DATAPOINTS_PER_UV_COORDS);
							newsubmeshUVcoordsArrayNewDataIndex = newsubmeshUVcoordsArrayNewDataIndex+SUBMESH_DATAPOINTS_PER_UV_COORDS;
							
							//Console::WriteLine("Notice: newsubmeshUVcoordsArrayNewDataIndex = newsubmeshUVcoordsArrayNewDataIndex+SUBMESH_DATAPOINTS_PER_UV_COORDS");
						}
						newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN] = unknownDataValue;
					}
					else
					{
						Console::WriteLine("error: !baryCentricPass");
					}
				}
				else
				{
					Console::WriteLine("\n0 error: !closestIntersectionXPointFound");
					//exit(0);
				}
			}
		}
	}
	#ifdef MDL_DEVEL_DEBUG_MISSING_TRIANGLES
	if(submeshName == "head")
	{
		exit(0);
	}
	#endif

	/*
	2. a)
	For each poly in newsubmeshVerticesPolyArray calculate its normal
	*/
	Console::WriteLine("2. a) For each poly in newsubmeshVerticesPolyArray calculate its normal");
	for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
	{
		//this case is only required for testing (when the newsubmesh is identical to the old submesh)
		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		if(newsubmeshPolyArrayFoundIdenticalPorig[pNew])
		{
			//Console::WriteLine("norm apply:");
			int indexOfIdenticalsubmeshUVmappingPoly = newsubmeshPolyArrayIndexOfClosestsubmeshUVmappingPoly[pNew];
			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+0] = submeshFaceArray[indexOfIdenticalsubmeshUVmappingPoly*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+0];
			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+1] = submeshFaceArray[indexOfIdenticalsubmeshUVmappingPoly*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+1];
			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+2] = submeshFaceArray[indexOfIdenticalsubmeshUVmappingPoly*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+2];
		}
		else
		{
		#endif
			vec normal = calculateNormalOfPoly(newsubmeshVerticesPolyArray[pNew]);

			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+0] = normal.x;
			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+1] = normal.y;
			newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+2] = normal.z;

		#ifdef MDL_SUPPORT_IMPORT_MESH_OF_EXACT_SAME_VALUES
		}
		#endif
	}

	#ifdef MDL_DEVEL_IMPORT_DESTROY_ALL_UV_NORMALS_AND_LIGHTING_DATA
	for(int pNew=0; pNew<newsubmeshVerticesPolyArray->Length; pNew++)
	{
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+0] = 0.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+1] = 0.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD+2] = 0.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+0] = 1.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+1] = 0.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_NORMAL+2] = 0.0;
		newsubmeshFaceArray[pNew*SUBMESH_DATAPOINTS_PER_FACE+SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN] = 0.0;
	}
	for(int iNew=0; iNew<newnumberOfVertices; iNew++)
	{
		for(int d=0; d<SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY; d++)
		{
			newsubmeshLightingEntriesArray[iNew*SUBMESH_DATAPOINTS_PER_LIGHTING_ENTRY+d] = 0;	//initialise to zero
		}
	}
	#ifdef MDL_DEVEL_IMPORT_DESTROY_UV_COORD_ARRAY
	newsubmeshUVcoordsArrayTemp = gcnew array<float>(1*SUBMESH_DATAPOINTS_PER_UV_COORDS);
	newsubmeshUVcoordsArrayTemp[0] = 0.0;
	newsubmeshUVcoordsArrayTemp[1] = 0.0;
	#endif
	#endif



	//copy newsubmeshUVcoordsArrayTemp data into newsubmeshUVcoordsArray (newsubmeshUVcoordsArrayTemp has been used to reduce memory allocation)
	newsubmeshUVcoordsArray =  gcnew array<float>(newsubmeshUVcoordsArrayNewDataIndex);
	for(int i=0; i<newsubmeshUVcoordsArrayNewDataIndex; i++)
	{
		newsubmeshUVcoordsArray[i] = newsubmeshUVcoordsArrayTemp[i];
	}

	
	/*
	//diagnostics: determine what is SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN 
	for(int i=0; i<newsubmeshVerticesArray->Length; i++)
	{
		//Console::WriteLine("i = {0}", i);
		if(newsubmeshVerticesArray[i] != submeshVerticesArray[i])
		{
			//newsubmeshVerticesArray[i] = submeshVerticesArray[i];	//to help diagnosis
			Console::WriteLine("newsubmeshVerticesArray[i] {0} != submeshVerticesArray[i] {1}", newsubmeshVerticesArray[i], submeshVerticesArray[i]); 
		}
	}
	for(int i=0; i<newsubmeshUVcoordsArray->Length; i++)
	{
		if(newsubmeshUVcoordsArray[i] != submeshUVcoordsArray[i])
		{
			Console::WriteLine("newsubmeshUVcoordsArray[i] != submeshUVcoordsArray[i]"); 
		}
	}
	for(int i=0; i<newsubmeshFaceArray->Length; i++)
	{
		if(newsubmeshFaceArray[i] != submeshFaceArray[i])
		{
			Console::WriteLine("newsubmeshFaceArray[i] != submeshFaceArray[i]"); 
			int faceInternalDataPointIndex = i%SUBMESH_DATAPOINTS_PER_FACE;
			if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD)
			{
				Console::WriteLine("faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD"); 
			}
			else if(faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
			{
				Console::WriteLine("faceInternalDataPointIndex < SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN"); 
			}
			else if(faceInternalDataPointIndex == SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN)
			{
				//newsubmeshFaceArray[i] = submeshFaceArray[i];
				Console::WriteLine("faceInternalDataPointIndex == SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN, numberoffaces = {0}", (newsubmeshFaceArray->Length)/SUBMESH_DATAPOINTS_PER_FACE); 
				Console::WriteLine("newsubmeshFaceArray[i] {0} != submeshFaceArray[i] {1}", newsubmeshFaceArray[i], submeshFaceArray[i]); 
			}
			else
			{
				Console::WriteLine("faceInternalDataPointIndex > SUBMESH_DATAPOINTS_FACE_INDEX_OF_UNKNOWN"); 
			}
		}
	}
	for(int i=0; i<newsubmeshLightingEntriesArray->Length; i++)
	{
		if(newsubmeshLightingEntriesArray[i] != submeshLightingEntriesArray[i])
		{
			Console::WriteLine("newsubmeshLightingEntriesArray[i] != submeshLightingEntriesArray[i]"); 
		}
	}
	*/
	

}
