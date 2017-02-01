/*******************************************************************************
 *
 * File Name: qfg5modelMath.hpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Math/Vector functions
 *
 *******************************************************************************/

value struct vec
{
	float x, y, z;
};
value struct mat
{
	vec a; vec b; vec c;
};
value struct poly
{
	vec a; vec b; vec c;
};
#define PI (3.14159265F)

void printValue(System::String^ title, float val);
void printVector(System::String^ title, vec vect);
void printPoly(System::String^ title, poly pol);
void printVectorLDR(System::String^ title, vec vect);
void printVectorLDR(System::String^ title, vec vect, int col, bool small);
void printLineLDR(System::String^ title, vec pt1, vec pt2);
void printPolyLDR(System::String^ title, poly pol);
bool compareVectorsArbitraryError(vec vect1, vec vect2, double error);
bool compareVectors(vec vect1, vec vect2);
void copyVector(vec % vecNew, vec vecbToCopy);
void setVectorVal(vec % vect, int j, float value);
float getVectorVal(vec vect, int j);
void setPolyVertex(poly % pol, int v, vec value);
vec getPolyVertex(poly pol, int v);
void setPolygonPoint(poly % pol, int v, int j, float value);
float getPolygonPoint(poly pol, int v, int j);
#define NUM_VERTICES_PER_POLYGON 3
#define NUM_DATAPOINTS_PER_VERTEX 3

vec calculateCentreOfPolygon(poly pol);

vec calculateNormalOfPoly(poly pol);
vec calculateNormal(vec pt1, vec pt2);
vec crossProduct(vec vect1, vec vect2);
vec subtractVectors(vec vect1, vec vect2);
vec addVectors(vec vect1, vec vect2);
double dotProduct(vec vect1, vec vect2);
vec multiplyVectorByScalar(vec vect1, double scalar);
vec divideVectorByScalar(vec vect1, double divisor);
void copyVector(vec % vecNew, vec vecbToCopy);
void initialiseVector(vec % vect);
double calculateAngleInDegreesBetweenTwoVectors(vec pt1, vec pt2);
vec negativeVector(vec vect1);
double findMagnitudeOfVector(vec vect1);
vec normaliseVector(vec vect1);

bool checkIfPointLiesOnTriangle3D(poly tri, vec P);
	bool sameSide(vec p1, vec p2, vec a, vec b);
vec calculateIntersectionPointOfLineAndPolygonPlane(vec P0, vec P1, poly pol, bool % pass);
/*
vec calculateIntersectionOfTwoLinesIn3DOLD(vec V1, vec P1, vec V2, vec P2, bool % pass);
double calculateEquivalentScalarDivision(vec vect1, vec vect2, bool % pass);
*/
vec calculateIntersectionOfTwoLinesIn3D(vec LineApoint1, vec LineApoint2, vec LineBpoint1, vec LineBpoint2, bool % pass, bool % intersectionOnLineSegments);
	double norm2(vec vect);
vec mapPointFromTriangle1toTriangle2(poly ABC, vec P, poly DEF, bool % pass);
	vec addThreeVectors(vec A, vec B, vec C);

void fillSubmeshVerticesPolygonArray(array<float>^ submeshFaceArray, array<float>^ submeshVerticesArray, array<poly>^ submeshVerticesPolyArray);
void fillSubmeshUVcoordsPolygonArray(array<float>^ submeshFaceArray, array<float>^ submeshUVcoordsArray, array<poly>^ submeshUVcoordsPolyArray);

double calculateTheDistanceBetweenTwoPoints(vec positionOf1, vec positionOf2);
	double calculateTheDistanceBetweenTwoPoints(double positionX1, double positionX2, double positionY1, double positionY2, double positionZ1, double positionZ2);
double calculateTheDistanceBetweenTwoPointsXYOnly(vec positionOf1, vec positionOf2);
bool compareDoubles(double a, double b);
bool compareDoublesArbitraryError(double a, double b, double error);


