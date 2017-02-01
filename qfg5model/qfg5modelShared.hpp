/*******************************************************************************
 *
 * File Name: qfg5modelShared.hpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Shared functions
 *
 *******************************************************************************/

#include "stdafx.h"

float bytesToFloat(array<unsigned char>^ tempByteArray);
System::String^ convertFloatToManagedString(float f);
System::String^ convertIntToManagedString(int i);