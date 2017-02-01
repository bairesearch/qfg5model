/*******************************************************************************
 *
 * File Name: qfg5model.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 *            Robert Monnig - Copyright (c) 2013 Monnig Design (monnigdesign.co.nz)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Main entry point for qfg5model
 *
 *******************************************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "qfg5modelBitmap.hpp"
#include "qfg5modelMesh.hpp"

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

//int Main(array<System::String ^> ^args);
	static void importOrExportBitmapOrMesh(System::String^ inputFileName, System::String^ outputFileName, bool exportBitmap, bool importBitmap, bool exportMesh, bool importMesh);
	static void printUsage();

static void printUsage()
{
	Console::WriteLine("Usage: qfgmodel <input file> <output file>\n");
	Console::WriteLine("Help:  To export MDL to BMP, set input file .MDL and output file .BMP");
	Console::WriteLine("       To import BMP to MDL, set input file .BMP and output file .MDL");
	Console::WriteLine("       To export MDL to HAK, set input file .MDL and output file .HAK");
	Console::WriteLine("       To import HAK to MDL, set input file .HAK and output file .MDL");
	Console::WriteLine("       HAK = 3D mesh (to generate high resolution mesh use hakenberg subdivision)");
}

int main(array<System::String ^> ^args)
{
	//cout << args.length() << endl;

	if (args->Length == 0 || args[0]->Length == 0 || args[1]->Length == 0)
	{
		printUsage();
		return 1;
	}

	//args
	System::String ^inputFileName = args[0];
	System::String ^outputFileName = args[1];

	bool IgnoreCase = true;
	if((inputFileName->EndsWith(MDL_EXTENSION, IgnoreCase, nullptr)) && (outputFileName->EndsWith(BMP_EXTENSION, IgnoreCase, nullptr)))
	{
		Console::WriteLine("qfg5model.exe mode: exportBitmap");
		importOrExportBitmapOrMesh(inputFileName, outputFileName, true, false, false, false);
	}
	else if((inputFileName->EndsWith(BMP_EXTENSION, IgnoreCase, nullptr)) && (outputFileName->EndsWith(MDL_EXTENSION, IgnoreCase, nullptr)))
	{
		Console::WriteLine("qfg5model.exe mode: importBitmap");
		importOrExportBitmapOrMesh(inputFileName, outputFileName, false, true, false, false);
	}
	else if((inputFileName->EndsWith(MDL_EXTENSION, IgnoreCase, nullptr)) && (outputFileName->EndsWith(HAK_EXTENSION, IgnoreCase, nullptr)))
	{
		Console::WriteLine("qfg5model.exe mode: exportMesh");
		importOrExportBitmapOrMesh(inputFileName, outputFileName, false, false, true, false);
	}
	else if((inputFileName->EndsWith(HAK_EXTENSION, IgnoreCase, nullptr)) && (outputFileName->EndsWith(MDL_EXTENSION, IgnoreCase, nullptr)))
	{
		Console::WriteLine("qfg5model.exe mode: importMesh");
		importOrExportBitmapOrMesh(inputFileName, outputFileName, false, false, false, true);
	}
	else
	{
		printUsage();
		return 1;
	}

	return 0;
}

static void importOrExportBitmapOrMesh(System::String^ inputFileName, System::String^ outputFileName, bool exportBitmap, bool importBitmap, bool exportMesh, bool importMesh)
{
	String^ inputFileNameBase = inputFileName->Substring(0, inputFileName->Length-4);
	String^ outputFileNameBase = outputFileName->Substring(0, outputFileName->Length-4);

	if(exportBitmap)
	{
		exportBMPfile(inputFileName, outputFileNameBase);
	}
	else
	{
		//addresses
		int mdlNameAddress = MDL_HEADER_NAME_ADDRESS;
		int mdlNumberOfSubmeshesAddress = MDL_HEADER_NUMBER_OF_SUBMESHES_ADDRESS;
		int mdlPaletteAddress = MDL_HEADER_BITMAPPALETTE_ADDRESS;
		int mdlBitmapHeaderAddressReferenceAddress = MDL_HEADER_BITMAPHEADERADDRESSREFERENCE_ADDRESS;
		int mdlMeshDataAddress = MDL_HEADER_3DDATA_ADDRESS;

		//mdl header
		array<unsigned char>^ mdlHeader = gcnew array<unsigned char>(mdlPaletteAddress);
			int numberOfSubmeshes = 0;
		array<unsigned char>^ bitmapPalette = gcnew array<unsigned char>(BITMAP_PALETTE_LENGTH);
		int mdlBitmapHeaderAddress;	//variable

		//3d mesh data
		array<unsigned char>^ mdlMeshData;

		//texture data
		array<unsigned char>^ mdlTextureData;

		//read data from MDL file
		String^ mdlFileName;
		if(importBitmap || importMesh)
		{
			mdlFileName = outputFileName;
		}
		else if(exportMesh)
		{
			mdlFileName = inputFileName;
		}

		BinaryReader^ bReader = gcnew BinaryReader(File::Open(mdlFileName, FileMode::Open));
		int sizeOfMDLfile = bReader->BaseStream->Length;
		try
		{
			//get mdl header
			mdlHeader = bReader->ReadBytes(mdlHeader->Length);
			//Console::WriteLine("mdlHeader->Length = {0}", mdlHeader->Length);
			bitmapPalette = bReader->ReadBytes(bitmapPalette->Length);
			//Console::WriteLine("bitmapPalette->Length = {0}", bitmapPalette->Length);
			mdlBitmapHeaderAddress = (int)(bReader->ReadUInt32());
			//Console::WriteLine("mdlBitmapHeaderAddress = {0}", mdlBitmapHeaderAddress);

			//get 3d mesh data
			mdlMeshData = gcnew array<unsigned char>(mdlBitmapHeaderAddress - BYTES_PER_INT - bitmapPalette->Length - mdlHeader->Length);
			mdlMeshData = bReader->ReadBytes(mdlMeshData->Length);
			//Console::WriteLine("mdlMeshData->Length = {0}", mdlMeshData->Length);

			//get texture data
			mdlTextureData = gcnew array<unsigned char>(sizeOfMDLfile - mdlBitmapHeaderAddress);
			mdlTextureData = bReader->ReadBytes(mdlTextureData->Length);

			array<unsigned char>^ mdlHeadernumberOfSubmeshes = { mdlHeader[mdlNumberOfSubmeshesAddress], mdlHeader[mdlNumberOfSubmeshesAddress+1], mdlHeader[mdlNumberOfSubmeshesAddress+2], mdlHeader[mdlNumberOfSubmeshesAddress+3] };
			numberOfSubmeshes = BitConverter::ToUInt32(mdlHeadernumberOfSubmeshes, 0);
			//Console::WriteLine("numberOfSubmeshes = {0}", numberOfSubmeshes);

			bReader->Close();
		}
		finally
		{

		}

		if(importBitmap)
		{
			array<unsigned char>^ mdlTextureDataNew;

			//Console::Write("mdlTextureData->Length {0}", mdlTextureData->Length);
			importBMPfile(inputFileNameBase, mdlTextureData, bitmapPalette, mdlTextureDataNew);
			//Console::WriteLine("importBitmap");
			//Console::Write("mdlTextureData->Length {0}", mdlTextureData->Length);

			mdlTextureData = mdlTextureDataNew;
		}

		if(importMesh || exportMesh)
		{
			array<unsigned char>^ mdlMeshDataNew;

			importOrExportMeshFile(mdlMeshData, mdlMeshDataNew, numberOfSubmeshes, mdlMeshDataAddress, inputFileNameBase, outputFileNameBase, importMesh);

			if(importMesh)
			{

				for(int i = 0; i<mdlMeshData->Length; i++)
				{
					if(mdlMeshData[i] == mdlMeshDataNew[i])
					{
						//Console::WriteLine("mdlMeshData[i] == mdlMeshDataNew[i] @ {0}", i);
					}
					else
					{
						//Console::WriteLine("mdlMeshData[i] != mdlMeshDataNew[i] @ {0}", i);
						//mdlMeshDataNew[i] = mdlMeshData[i];		//temporary correction..
					}
				}
				//note this code can be disabled for test to prevent new data from being written to file

				Console::WriteLine("mdlMeshData->Length = {0}", mdlMeshData->Length);
				Console::WriteLine("mdlMeshDataNew->Length = {0}", mdlMeshDataNew->Length);
				mdlBitmapHeaderAddress = mdlBitmapHeaderAddress - mdlMeshData->Length + mdlMeshDataNew->Length;
				mdlMeshData = mdlMeshDataNew;

			}
		}

		if(importBitmap || importMesh)
		{
			BinaryWriter^ bWriter = gcnew BinaryWriter(File::Open(outputFileName, FileMode::Create));
			try
			{
				bWriter->Write(mdlHeader);
				bWriter->Write(bitmapPalette);
				bWriter->Write(mdlBitmapHeaderAddress);
				bWriter->Write(mdlMeshData);
				bWriter->Write(mdlTextureData);
			}
			finally
			{
			}
		}
	}
}


