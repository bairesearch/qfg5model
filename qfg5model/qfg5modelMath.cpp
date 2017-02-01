/*******************************************************************************
 *
 * File Name: qfg5modelMath.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Math/Vector functions
 *
 *******************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "qfg5modelMath.hpp"
#include "qfg5modelShared.hpp"

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

void printValue(System::String^ title, float val)
{
	Console::WriteLine("{0} = {1}", title, val);
}
void printVector(System::String^ title, vec vect)
{
	Console::WriteLine("Vector {0} = {1} {2} {3}", title, vect.x, vect.y, vect.z);
}
void printPoly(System::String^ title, poly pol)
{
	Console::WriteLine("Poly: {0} = ", title);
	printVector("pol.a", pol.a);
	printVector("pol.b", pol.b);
	printVector("pol.c", pol.c);
}
void printVectorLDR(System::String^ title, vec vect)
{//prints point
	Console::WriteLine("0 Vector/Point {0} =", title);
	Console::WriteLine("1 3 {0} {1} {2} 0.1 0 0 0 0.1 0 0 0 0.1 point.dat", vect.x, vect.y, vect.z);
}
void printVectorLDR(System::String^ title, vec vect, int col, bool small)
{//prints point
	String^ colString = convertIntToManagedString(col);
	Console::WriteLine("0 Vector/Point {0} =", title);
	if(small)
	{
		Console::WriteLine("1 {3} {0} {1} {2} 0.01 0 0 0 0.01 0 0 0 0.01 point.dat", vect.x, vect.y, vect.z, colString);
	}
	else
	{
		Console::WriteLine("1 {3} {0} {1} {2} 0.1 0 0 0 0.1 0 0 0 0.1 point.dat", vect.x, vect.y, vect.z, colString);
	}
}

void printLineLDR(System::String^ title, vec pt1, vec pt2)
{
	Console::WriteLine("0 Line {0} =", title);
	Console::WriteLine("2 1 {0} {1} {2} {3} {4} {5}", pt1.x, pt1.y, pt1.z, pt2.x, pt2.y, pt2.z);
}
void printPolyLDR(System::String^ title, poly pol)
{
	Console::WriteLine("0 Poly {0} =", title);
	Console::WriteLine("3 41 {0} {1} {2} {3} {4} {5} {6} {7} {8}", pol.a.x, pol.a.y, pol.a.z, pol.b.x, pol.b.y, pol.b.z, pol.c.x, pol.c.y, pol.c.z);	//41 - transparent blue
}


bool compareVectorsArbitraryError(vec vect1, vec vect2, double error)
{
	bool result = true;
	if(!compareDoublesArbitraryError(vect1.x, vect2.x, error))
	{
		result = false;
	}
	if(!compareDoublesArbitraryError(vect1.y, vect2.y, error))
	{
		result = false;
	}
	if(!compareDoublesArbitraryError(vect1.z, vect2.z, error))
	{
		result = false;
	}
	return result;
}

bool compareVectors(vec vect1, vec vect2)
{
	bool result = true;
	if(!compareDoubles(vect1.x, vect2.x))
	{
		result = false;
	}
	if(!compareDoubles(vect1.y, vect2.y))
	{
		result = false;
	}
	if(!compareDoubles(vect1.z, vect2.z))
	{
		result = false;
	}
	return result;
}


void copyVector(vec % vecNew, vec vecbToCopy)
{

	vecNew.x = vecbToCopy.x;
	vecNew.y = vecbToCopy.y;
	vecNew.z = vecbToCopy.z;
}

void setVectorVal(vec % vect, int j, float value)
{
	if(j == 0)
	{
		vect.x = value;
	}
	else if(j == 1)
	{
		vect.y = value;
	}
	else if(j == 2)
	{
		vect.z = value;
	}
	else
	{
		Console::WriteLine("setPolygonPoint error");
	}
}
float getVectorVal(vec vect, int j)
{
	float output;
	if(j == 0)
	{
		output = vect.x;
	}
	else if(j == 1)
	{
		output = vect.y;
	}
	else if(j == 2)
	{
		output = vect.z;
	}
	else
	{
		Console::WriteLine("setPolygonPoint error");
	}
	return output;
}

void setPolyVertex(poly % pol, int v, vec value)
{
	if(v == 0)
	{
		pol.a = value;
	}
	else if(v == 1)
	{
		pol.b = value;
	}
	else if(v == 2)
	{
		pol.c = value;
	}
	else
	{
		Console::WriteLine("setPolyVertex error");
	}
}
vec getPolyVertex(poly pol, int v)
{
	vec output;
	if(v == 0)
	{
		output = pol.a;
	}
	else if(v == 1)
	{
		output = pol.b;
	}
	else if(v == 2)
	{
		output = pol.c;
	}
	else
	{
		Console::WriteLine("getPolyVertex error");
	}
	return output;
}

void setPolygonPoint(poly % pol, int v, int j, float value)
{
	if(v == 0)
	{
		if(j == 0)
		{
			pol.a.x = value;
		}
		else if(j == 1)
		{
			pol.a.y = value;
		}
		else if(j == 2)
		{
			pol.a.z = value;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else if(v == 1)
	{
		if(j == 0)
		{
			pol.b.x = value;
		}
		else if(j == 1)
		{
			pol.b.y = value;
		}
		else if(j == 2)
		{
			pol.b.z = value;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else if(v == 2)
	{
		if(j == 0)
		{
			pol.c.x = value;
		}
		else if(j == 1)
		{
			pol.c.y = value;
		}
		else if(j == 2)
		{
			pol.c.z = value;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else
	{
		Console::WriteLine("setPolygonPoint error");
	}
}

float getPolygonPoint(poly pol, int v, int j)
{
	float output;
	if(v == 0)
	{
		if(j == 0)
		{
			output = pol.a.x;
		}
		else if(j == 1)
		{
			output = pol.a.y;
		}
		else if(j == 2)
		{
			output = pol.a.z;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else if(v == 1)
	{
		if(j == 0)
		{
			output = pol.b.x;
		}
		else if(j == 1)
		{
			output = pol.b.y;
		}
		else if(j == 2)
		{
			output = pol.b.z;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else if(v == 2)
	{
		if(j == 0)
		{
			output = pol.c.x;
		}
		else if(j == 1)
		{
			output = pol.c.y;
		}
		else if(j == 2)
		{
			output = pol.c.z;
		}
		else
		{
			Console::WriteLine("setPolygonPoint error");
		}
	}
	else
	{
		Console::WriteLine("setPolygonPoint error");
	}
	return output;
}

void fillSubmeshVerticesPolygonArray(array<float>^ submeshFaceArray, array<float>^ submeshVerticesArray, array<poly>^ submeshVerticesPolyArray)
{
	//Console::WriteLine("submeshFaceArray->Length = {0}", submeshFaceArray->Length);
	//Console::WriteLine("submeshVerticesArray->Length = {0}", submeshVerticesArray->Length);

	for(int f=0; f<submeshFaceArray->Length; f=f+SUBMESH_DATAPOINTS_PER_FACE)
	{
		for(int v=SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_VERTEX; v<SUBMESH_DATAPOINTS_VERTICES_PER_FACE; v++)
		{
			int vertexIndex = int(submeshFaceArray[f + v]);
			//Console::WriteLine("vertexIndex {1} = {0}", vertexIndex, v);
			for(int j=0; j<SUBMESH_DATAPOINTS_PER_VERTEX; j++)
			{
				float dataPoint = submeshVerticesArray[vertexIndex*SUBMESH_DATAPOINTS_PER_VERTEX+j];
				setPolygonPoint(submeshVerticesPolyArray[f/SUBMESH_DATAPOINTS_PER_FACE], v, j, dataPoint);
			}
		}
	}
}

void fillSubmeshUVcoordsPolygonArray(array<float>^ submeshFaceArray, array<float>^ submeshUVcoordsArray, array<poly>^ submeshUVcoordsPolyArray)
{
	//Console::WriteLine("submeshFaceArray->Length = {0}", submeshFaceArray->Length);
	//Console::WriteLine("submeshUVcoordsArray->Length = {0}", submeshUVcoordsArray->Length);

	for(int f=0; f<submeshFaceArray->Length; f=f+SUBMESH_DATAPOINTS_PER_FACE)
	{
		for(int v=0; v<SUBMESH_DATAPOINTS_UVCOORDS_PER_FACE; v++)
		{
			int vIndexInFace = v+SUBMESH_DATAPOINTS_FACE_INDEX_OF_FIRST_UVCOORD;
			int vertexIndex = int(submeshFaceArray[f + vIndexInFace]);
			//Console::WriteLine("vertexIndex {1} = {0}", vertexIndex, vIndexInFace);
			for(int j=0; j<SUBMESH_DATAPOINTS_PER_UV_COORDS; j++)
			{
				float dataPoint = submeshUVcoordsArray[vertexIndex*SUBMESH_DATAPOINTS_PER_UV_COORDS+j];
				//Console::WriteLine("dataPoint = {0}", dataPoint);
				setPolygonPoint(submeshUVcoordsPolyArray[f/SUBMESH_DATAPOINTS_PER_FACE], v, j, dataPoint);
			}
			setPolygonPoint(submeshUVcoordsPolyArray[f/SUBMESH_DATAPOINTS_PER_FACE], v, SUBMESH_DATAPOINTS_PER_UV_COORDS, 0.0F);	 //using x(U),y(V) coords only (set z coordinate to zero)
		}
	}
}

/*

	for(int p=0; p<submeshPolyArray.Length; p++)
	{
		vec polygonCentre = calculateCentreOfPolygon(submeshPolyArray[p]);

		float minDistance = SOME_LARGE_DISTANCE;
		int indexOfNewPolygonWithAtMinDistance = 0;
		for(int pNew=0; pNew<newsubmeshPolyArray.Length; pNew++)
		{
			vec newpolygonCentre = calculateCentreOfPolygon(newsubmeshPolyArray[pNew]);
			float distance = calculateTheDistanceBetweenTwoPoints(polygonCentre, newpolygonCentre);
			if(distance < minDistance)
			{
				minDistance = distance;
				indexOfNewPolygonWithAtMinDistance = pNew;
			}
		}

	}

*/

vec calculateCentreOfPolygon(poly pol)
{
	vec averageValues;
	for(int v=0; v<NUM_VERTICES_PER_POLYGON; v++)
	{
		for(int j=0; j<NUM_DATAPOINTS_PER_VERTEX; j++)
		{
			float currentAveragedVal = getVectorVal(averageValues, j);
			float dataPoint = getPolygonPoint(pol, v, j);
			setVectorVal(averageValues, j, currentAveragedVal+dataPoint);
		}
	}
	for(int j=0; j<NUM_DATAPOINTS_PER_VERTEX; j++)
	{
		float currentAveragedVal = getVectorVal(averageValues, j);
		setVectorVal(averageValues, j, currentAveragedVal/SUBMESH_DATAPOINTS_PER_VERTEX);
	}
	return averageValues;
}


vec calculateNormalOfPoly(poly pol)
{
	vec vec1 = subtractVectors(pol.b, pol.a);
	vec vec2 = subtractVectors(pol.c, pol.a);
	vec normal = calculateNormal(vec1, vec2);
	return normal;
}

vec calculateNormal(vec pt1, vec pt2)
{
	return crossProduct(pt1, pt2);
}

vec crossProduct(vec vect1, vec vect2)
{
	vec vect;
	vect.x = (vect1.y)*(vect2.z) - (vect2.y)*(vect1.z);
	vect.y = -((vect1.x)*(vect2.z) - (vect2.x)*(vect1.z));
	vect.z = (vect1.x)*(vect2.y) - (vect2.x)*(vect1.y);
	return vect;
}

vec subtractVectors(vec vect1, vec vect2)
{
	vec vect;
	vect.x = vect1.x - vect2.x;
	vect.y = vect1.y - vect2.y;
	vect.z = vect1.z - vect2.z;
	return vect;
}

vec addVectors(vec vect1, vec vect2)
{
	vec vect;
	vect.x = vect1.x + vect2.x;
	vect.y = vect1.y + vect2.y;
	vect.z = vect1.z + vect2.z;
	return vect;
}

double dotProduct(vec vect1, vec vect2)
{
	double scalar = vect1.x*vect2.x + vect1.y*vect2.y + vect1.z*vect2.z;
	return scalar;
}

vec multiplyVectorByScalar(vec vect1, double scalar)
{
	vec vect;
	vect.x = vect1.x * scalar;
	vect.y = vect1.y * scalar;
	vect.z = vect1.z * scalar;
	return vect;
}

vec divideVectorByScalar(vec vect1, double divisor)
{
	vec vect;
	vect.x = vect1.x/divisor;
	vect.y = vect1.y/divisor;
	vect.z = vect1.z/divisor;
	return vect;
}


void initialiseVector(vec % vect)
{
	vect.x = 0.0;
	vect.y = 0.0;
	vect.z = 0.0;
}


double calculateAngleInDegreesBetweenTwoVectors(vec pt1, vec pt2)
{
	double interiorAngle = acos(dotProduct(pt1, pt2) / (findMagnitudeOfVector(pt1) * findMagnitudeOfVector(pt2)));
	interiorAngle = (interiorAngle/PI)*180.0F;
	//Console::WriteLine("calculateAngleInDegreesBetweenTwoVectors = {0}", interiorAngle);
	return interiorAngle;
}

vec negativeVector(vec vect1)
{
	vec vect;
	vect.x = -(vect1.x);
	vect.y = -(vect1.y);
	vect.z = -(vect1.z);
	return vect;
}

double findMagnitudeOfVector(vec vect1)
{
	return sqrt(pow(vect1.x, 2) + pow(vect1.y,2) + pow(vect1.z,2));
}


vec normaliseVector(vec vect1)
{
	vec normalisedVector;
	double magnitude = findMagnitudeOfVector(vect1);
	if(magnitude > DOUBLE_MIN_PRECISION)
	{
		normalisedVector.x = (vect1.x)/magnitude;
		normalisedVector.y = (vect1.y)/magnitude;
		normalisedVector.z = (vect1.z)/magnitude;
	}
	else
	{
		normalisedVector.x = 0.0;
		normalisedVector.y = 0.0;
		normalisedVector.z = 0.0;
	}
	return vect1;
}


//stackoverflow:
/*
bool checkIfPointLiesOnTriangle3D(poly tri, vec P)
{
	//calculate centre of tri
	vec P4;	//poly centre
	initialiseVector(polyCentre);
	for(int v=0; v<3; v++)
	{
		for(int j=0; j<3; j++)
		{
			float jVal = getPolygonPoint(tri, v, j);
			float jP4 = getVectorVal(P4, j);
			setVectorVal(P4, j, jP4 + jVal);
		}
	}
	for(int j=0; j<3; j++)
	{
		float jP4 = getVectorVal(P4, j);
		setVectorVal(P4, j, jP4/3.0F);
	}

	//now see if P and P4 lie on the same side of each of the three lines
}
*/

bool checkIfPointLiesOnTriangle3D(poly tri, vec P)
{
	if(sameSide(P, tri.a, tri.b, tri.c) && sameSide(P, tri.b, tri.a, tri.c) && sameSide(P, tri.c, tri.a, tri.b))
	{
		return true;
	}
	else
	{
		return false;
	}

}
bool sameSide(vec p1, vec p2, vec a, vec b)
{
	vec cp1 = crossProduct(subtractVectors(b, a), subtractVectors(p1, a));
	vec cp2 = crossProduct(subtractVectors(b, a), subtractVectors(p2, a));
	if(dotProduct(cp1, cp2) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//from xbdev.net/maths_of_3d/collision_detection/line_with_plane.php
vec calculateIntersectionPointOfLineAndPolygonPlane(vec P0, vec P1, poly pol, bool % pass)
{
	vec intersectionP;
	vec N = calculateNormalOfPoly(pol);
	vec P2 = pol.a;	//let P2 be some arbitrary point on the plane
	vec vecP2minusP0 = subtractVectors(P2, P0);
	vec vecP1minusP0 = subtractVectors(P1, P0);
	if(compareDoubles(dotProduct(N,vecP1minusP0), 0.0))
	{
		pass = false;
		//no intersection
		intersectionP.x = SOME_LARGE_DISTANCE;
		intersectionP.y = SOME_LARGE_DISTANCE;
		intersectionP.z = SOME_LARGE_DISTANCE;
	}
	else
	{
		pass = true;
		double t = dotProduct(N,vecP2minusP0) / dotProduct(N,vecP1minusP0);
		intersectionP = addVectors(P0, multiplyVectorByScalar(vecP1minusP0, t));
	}
	return intersectionP;
}

vec calculateIntersectionOfTwoLinesIn3D(vec LineApoint1, vec LineApoint2, vec LineBpoint1, vec LineBpoint2, bool % pass, bool % intersectionOnLineSegments)
{
	intersectionOnLineSegments = false;
	pass = false;

	vec ip;

	vec da = subtractVectors(LineApoint2, LineApoint1);
	vec db = subtractVectors(LineBpoint2, LineBpoint1);
	vec dc = subtractVectors(LineBpoint1, LineApoint1);

	if(!compareDoublesArbitraryError(dotProduct(dc, crossProduct(da, db)), 0.0, DOUBLE_MODERATELY_RELAXED_PRECISION))
	{//lines are not coplanar
		pass = false;
		Console::WriteLine("dotProduct(dc, crossProduct(da, db)) = {0}", dotProduct(dc, crossProduct(da, db)));
	}
	else
	{
		double s = dotProduct(crossProduct(dc, db), crossProduct(da, db)) / norm2(crossProduct(da, db));

		if(s >= 0.0 && s <= 1.0)
		{
			//intersection point on line segments
			//Console::WriteLine("s = {0}", s);
			intersectionOnLineSegments = true;
		}

		ip = addVectors(LineApoint1, multiplyVectorByScalar(da, s));
		pass = true;
	}

	return ip;
}
double norm2(vec vect)
{
	return dotProduct(vect, vect);
}

/*
//from mathforum.org/library/drmath/view/63719.html
vec calculateIntersectionOfTwoLinesIn3DOLD(vec V1, vec P1, vec V2, vec P2, bool % pass)
{
	//not required;
	//V1 = normaliseVector(V1);
	//V2 = normaliseVector(V2);

	pass = false;
	vec intersectionPoint;

	//L1 = P1 +aV1
	//L2 = P1 +aV1	(?L2 = P2 +bV2)
	//a(V1xV2) = (P2-P1) x V2
	vec V1xV2 = crossProduct(V1, V2);
	vec P2minusP1 = subtractVectors(P2, P1);
	vec P2minusP1crossV2 = crossProduct(P2minusP1, V2);
	double a = calculateEquivalentScalarDivision(P2minusP1crossV2, V1xV2, pass);
	if(pass)
	{
		vec aV1 = multiplyVectorByScalar(V1, a);
		intersectionPoint = addVectors(P1, aV1);
		vec intersectionPointMinusP2 = subtractVectors(intersectionPoint, P2);
		double b = calculateEquivalentScalarDivision(intersectionPointMinusP2, V2, pass);
		//Console::WriteLine("1calculateIntersectionOfTwoLinesIn3D pass: x y z = {0},{1},{2}", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
		if(pass)
		{
			//success
			//Console::WriteLine("2calculateIntersectionOfTwoLinesIn3D pass: x y z = {0},{1},{2}", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
		}
	}
	else
	{

	}

	return intersectionPoint;
}

double calculateEquivalentScalarDivision(vec vect1, vec vect2, bool % pass)
{
	vec vect;
	vect.x = vect1.x / vect2.x;
	vect.y = vect1.y / vect2.y;
	vect.z = vect1.z / vect2.z;

	if(compareDoublesRelaxed(vect.x, vect.y) && compareDoublesRelaxed(vect.y, vect.z))
	{
		pass = true;
	}
	else
	{
		pass = false;
	}

	return vect.x;
}
*/

vec addThreeVectors(vec A, vec B, vec C)
{
	vec comp1 = addVectors(A, B);
	vec comp2 = addVectors(comp1, C);
	return comp2;
}
vec mapPointFromTriangle1toTriangle2(poly ABC, vec P, poly DEF, bool % pass)
{
	vec A = getPolyVertex(ABC, 0);
	vec B = getPolyVertex(ABC, 1);
	vec C = getPolyVertex(ABC, 2);

	//compute the normal of the triangle
	vec N = normaliseVector(crossProduct(subtractVectors(B, A), subtractVectors(C, A)));
	//compute twice area of triangle ABC
	float AreaABC = dotProduct(N, crossProduct(subtractVectors(B, A), subtractVectors(C, A)));
	//compute a
	float AreaPBC = dotProduct(N, crossProduct(subtractVectors(B, P), subtractVectors(C, P)));
	float a = AreaPBC / AreaABC;
	//compute b
	float AreaPCA = dotProduct(N, crossProduct(subtractVectors(C, P), subtractVectors(A, P)));
	float b = AreaPCA / AreaABC;
	//compute c
	float c = 1.0f - a - b;


	//test
	vec PP = addThreeVectors(multiplyVectorByScalar(A, a), multiplyVectorByScalar(B, b), multiplyVectorByScalar(C, c));
	vec zero;
	initialiseVector(zero);
	if(compareVectors(subtractVectors(PP, P), zero))
	{
		pass = true;
	}
	else
	{
		Console::WriteLine("mapPointFromTriangle1toTriangle2 error");
		pass = false;
	}

	vec D = getPolyVertex(DEF, 0);
	vec E = getPolyVertex(DEF, 1);
	vec F = getPolyVertex(DEF, 2);
	/*
	vec comp1 = D;
	vec comp2 = multiplyVectorByScalar(subtractVectors(E, D), a);
	vec comp3 = multiplyVectorByScalar(subtractVectors(F, D), b);
	vec comp1andComp2 = addVectors(comp1, comp2);
	vec Pnew = addVectors(comp1andComp2, comp3);
	*/
	vec Pnew = addThreeVectors(multiplyVectorByScalar(D, a), multiplyVectorByScalar(E, b), multiplyVectorByScalar(F, c));

	return Pnew;
}


double calculateTheDistanceBetweenTwoPoints(vec positionOf1, vec positionOf2)
{
	return calculateTheDistanceBetweenTwoPoints(positionOf1.x, positionOf2.x, positionOf1.y, positionOf2.y, positionOf1.z, positionOf2.z);
}

double calculateTheDistanceBetweenTwoPoints(double positionX1, double positionX2, double positionY1, double positionY2, double positionZ1, double positionZ2)
{
	double xDistanceBetweenTheTwoPoints = positionX1 - positionX2;
	double yDistanceBetweenTheTwoPoints = positionY1 - positionY2;
	double zDistanceBetweenTheTwoPoints = positionZ1 - positionZ2;
	double distanceBetweenTheTwoPoints = sqrt(pow(xDistanceBetweenTheTwoPoints, 2) + pow(yDistanceBetweenTheTwoPoints, 2) + pow(zDistanceBetweenTheTwoPoints, 2));

	return distanceBetweenTheTwoPoints;
}
double calculateTheDistanceBetweenTwoPointsXYOnly(vec positionOf1, vec positionOf2)
{
	return calculateTheDistanceBetweenTwoPoints(positionOf1.x, positionOf2.x, positionOf1.y, positionOf2.y, 0.0, 0.0);
}

bool compareDoubles(double a, double b)
{
	return compareDoublesArbitraryError(a, b, DOUBLE_MIN_PRECISION);
}

bool compareDoublesRelaxed(double a, double b)
{
	return compareDoublesArbitraryError(a, b, 0.01);
}

bool compareDoublesArbitraryError(double a, double b, double error)
{
	bool result;
	if((a < (b+error)) && (a > (b-error)))
	{
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}




