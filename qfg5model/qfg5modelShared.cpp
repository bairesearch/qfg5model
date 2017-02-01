/*******************************************************************************
 *
 * File Name: qfg5modelShared.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Shared functions
 *
 *******************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

float bytesToFloat(array<unsigned char>^ tempByteArray)
{
	float output;
	unsigned char b[] = {tempByteArray[0], tempByteArray[1], tempByteArray[2], tempByteArray[3]};
	memcpy(&output, &b, sizeof(output));
	return output;
}

System::String^ convertFloatToManagedString(float f)
{
	char tempString[SUBMESH_DATAPOINT_MAX_LENGTH];
	sprintf(tempString, "%0.6f", f);
	String^ tempString2 = gcnew String(tempString);
	return tempString2;
}

System::String^ convertIntToManagedString(int i)
{
	char tempString[SUBMESH_DATAPOINT_MAX_LENGTH];
	sprintf(tempString, "%d", i);
	String^ tempString2 = gcnew String(tempString);
	return tempString2;
}



