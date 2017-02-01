/*******************************************************************************
 *
 * File Name: qfg5modelBitmap.cpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 *            Robert Monnig - Copyright (c) 2013 Monnig Design (monnigdesign.co.nz)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Bitmap i/o functions
 *
 *******************************************************************************/

#include "stdafx.h"
#include "qfg5modelShared.hpp"
#include <iostream>
#include <fstream>

#using <mscorlib.dll>
using namespace System;
using namespace System::IO;

//void importBMPfile(System::String^ inputFileName, array<unsigned char>^ % mdlTextureData, array<unsigned char>^ % bitmapPalette, array<unsigned char>^ % newmdlTextureData);
	static void insertByteArray(array<unsigned char>^ byteArrayToUpdate, array<unsigned char>^ byteArrayToInsert, int index);
	static int calculatePowerOf2(int value);
//void exportBMPfile(System::String^ inputFileName, System::String^ outputFileName);
	static void writeBMP(String^ outputFileName, array<unsigned char>^ mdlPalette, array<unsigned char>^ pixelData, int bitmapwidth, int bitmapheight);

static void swapPalette(array<unsigned char>^ palette);

//Preconditions; importing BMPs of a different resolution (width x height) is only designed to work if there is a single subBitmap (because the format of MultiBitmapHeaderAdditionSpace is undefined, and MultiBitmapHeaderAdditionSpace is not used when there is only a single subbitmap)
void importBMPfile(System::String^ inputFileNameBase, array<unsigned char>^ mdlTextureData, array<unsigned char>^ % bitmapPalette, array<unsigned char>^ % newmdlTextureData)
{
	//will overwrite both bitmapPalette and bitmapPixelData...;

	//get mdlBitmapType
	array<unsigned char>^ mdlBitmapHeaderType = { mdlTextureData[0], mdlTextureData[1], mdlTextureData[2], mdlTextureData[3]};
	int mdlBitmapType = (BitConverter::ToUInt32(mdlBitmapHeaderType, 0))/MDL_BITMAP_HEADER_TYPE_MULTIPLIER - 1;	//4,8,12,16 => 0,1,2,3
	Console::WriteLine("mdlBitmapType = {0}", mdlBitmapType);

	//create mdlBitmapHeader
	int numberOfBytesOfMultiBitmapHeaderAdditionSpace = BYTES_PER_INT*mdlBitmapType;
	array<unsigned char>^ mdlBitmapHeader = gcnew array<unsigned char>(BYTES_PER_INT + numberOfBytesOfMultiBitmapHeaderAdditionSpace);
	for(int i=0; i<BYTES_PER_INT; i++)
	{
		mdlBitmapHeader[i] = mdlTextureData[i];
	}
	for(int i=0; i<numberOfBytesOfMultiBitmapHeaderAdditionSpace; i++)
	{
		mdlBitmapHeader[BYTES_PER_INT+i] = mdlTextureData[BYTES_PER_INT+i];
	}

	array<array<unsigned char>^>^ newSubBitmapHeaderArray = gcnew array<array<unsigned char>^>(mdlBitmapType+1);
	array<array<unsigned char>^>^ newSubBitmapPixelDataArray = gcnew array<array<unsigned char>^>(mdlBitmapType+1);
	int newSubBitmapPixelDataArrayTotalContentsSize = 0;
	int newSubBitmapHeaderArrayTotalContentsSize = 0;

		/*
		MDLbitmapHeaderType0 (4)
		020.MDL: startOfMDLheader = 630536 = 99F08
			[Check bitmap size = B9F24 - 99F24 = 20000x = 131072 = 512 * 256]

		MDLbitmapHeaderType1 (8)
		066.MDL: startOfMDLheader = 56784 = DDD0
		DDF0 + 10000 (256 * 256 in hex) = 1DDF0
			numberOfBytesOfMultiBitmapHeaderAdditionSpace:
			200001 = 2097153 =

		MDLbitmapHeaderType2 (12)
		128.MDL: startOfMDLheader = 95520 = 17520
		17544 + 10000 (256 * 256 in hex) = 27544
		2755D + 10000 (256 * 256 in hex) = 3755D
			numberOfBytesOfMultiBitmapHeaderAdditionSpace:
			240001 = 2359297 =
			3C0002 = 3932162 =

		MDLbitmapHeaderType3 (16)
		090.MDL: startOfMDLheader = 64708 = FCC4
		FCED + 20000 (512 * 256 in hex) = 2FCED (subbitmap header #2)
		2FD04 + 20000 (512 * 256 in hex) = 4FD04
		4FD1D + 20000 (512 * 256 in hex) = 6FD1D
			numberOfBytesOfMultiBitmapHeaderAdditionSpace::
			280002 = 2621442 =
			400004 = 4194308 =
			580006 = 5767174 =
		*/

	for(int subBitmap=0; subBitmap<mdlBitmapType+1; subBitmap++)
	{
		String^ inputFileNameSubBitmap = inputFileNameBase;
		if(mdlBitmapType>0)
		{
			String^ mdlBitmapTypeString = convertIntToManagedString(subBitmap);
			inputFileNameSubBitmap = inputFileNameBase + "-" + mdlBitmapTypeString;
		}
		inputFileNameSubBitmap = inputFileNameSubBitmap + BMP_EXTENSION;


		//read data from Bitmap file
		BinaryReader^ bReader = gcnew BinaryReader(File::Open(inputFileNameSubBitmap, FileMode::Open));
		try
		{
			//read BMP bitmap size (may differ from original pixelmap size stored in MDL file)
			bReader->BaseStream->Seek(18, SeekOrigin::Begin);
			array<unsigned char>^ BMPwidthByteArray = bReader->ReadBytes(BYTES_PER_INT);
			array<unsigned char>^ BMPheightByteArray = bReader->ReadBytes(BYTES_PER_INT);
			int newSubBitmapwidth = BitConverter::ToUInt32(BMPwidthByteArray, 0);
			int newSubBitmapheight = BitConverter::ToUInt32(BMPheightByteArray, 0);

			array<unsigned char>^ newSubBitmapHeader = gcnew array<unsigned char>(SUBBITMAP_HEADER_LENGTH);
			array<unsigned char>^ mdlBitmapHeader32bitWidthValueToInsert;
			array<unsigned char>^ mdlBitmapHeader32bitHeightValueToInsert;
			mdlBitmapHeader32bitWidthValueToInsert = BitConverter::GetBytes((float)(newSubBitmapwidth));	//width (float32)
			mdlBitmapHeader32bitHeightValueToInsert = BitConverter::GetBytes((float)(newSubBitmapheight));	//height (float32)
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitWidthValueToInsert, SUBBITMAP_HEADER_WIDTH_FLOAT_ADDRESS);
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitHeightValueToInsert, SUBBITMAP_HEADER_HEIGHT_FLOAT_ADDRESS);
			mdlBitmapHeader32bitWidthValueToInsert = BitConverter::GetBytes((unsigned int)(calculatePowerOf2(newSubBitmapwidth)));		//width (power of 2)
			mdlBitmapHeader32bitHeightValueToInsert = BitConverter::GetBytes((unsigned int)(calculatePowerOf2(newSubBitmapheight)));	//height (power of 2)
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitWidthValueToInsert, SUBBITMAP_HEADER_WIDTH_POWER_OF_TWO_ADDRESS);
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitHeightValueToInsert, SUBBITMAP_HEADER_HEIGHT_POWER_OF_TWO_ADDRESS);
			mdlBitmapHeader32bitWidthValueToInsert = BitConverter::GetBytes((unsigned int)(newSubBitmapwidth - 1));	//width -1		//eg 1024 -1	OR	512 -1
			mdlBitmapHeader32bitHeightValueToInsert = BitConverter::GetBytes((unsigned int)(newSubBitmapheight - 1));	//height -1		//eg 512 - 1	OR	256 - 1
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitWidthValueToInsert, SUBBITMAP_HEADER_WIDTH_MINUS_ONE_ADDRESS);
			insertByteArray(newSubBitmapHeader, mdlBitmapHeader32bitHeightValueToInsert, SUBBITMAP_HEADER_HEIGHT_MINUS_ONE_ADDRESS);
			newSubBitmapHeaderArray[subBitmap] = newSubBitmapHeader;
			newSubBitmapHeaderArrayTotalContentsSize = newSubBitmapHeaderArrayTotalContentsSize + newSubBitmapHeader->Length;
			/*
			//testing:
			Console::WriteLine("newSubBitmapwidth = {0}", newSubBitmapwidth);
			Console::WriteLine("newSubBitmapheight = {0}", newSubBitmapheight);
			Console::WriteLine("calculatePowerOf2(newSubBitmapwidth) = {0}", calculatePowerOf2(newSubBitmapwidth));
			Console::WriteLine("calculatePowerOf2(newSubBitmapheight) = {0}", calculatePowerOf2(newSubBitmapheight));
			Console::WriteLine("(float)(newSubBitmapwidth) = {0}", (float)(newSubBitmapwidth));
			Console::WriteLine("(float)(newSubBitmapheight) = {0}", (float)(newSubBitmapheight));
			for(int i=0; i<BMPwidthNumOfBytes; i++)
			{
				Console::WriteLine("(float)(newSubBitmapwidth) byte {1} = {0}", mdlBitmapHeader32bitWidthValueToInsert[i], i);
			}
			for(int i=0; i<BMPheightNumOfBytes; i++)
			{
				Console::WriteLine("(float)(newSubBitmapheight) byte {1} = {0}", mdlBitmapHeader32bitHeightValueToInsert[i], i);
			}
			*/

			bReader->BaseStream->Seek(58, SeekOrigin::Begin);
			array<unsigned char>^ newSubBitmapPalette = bReader->ReadBytes(bitmapPalette->Length);
			bReader->BaseStream->Seek(1, SeekOrigin::Current);
			array<unsigned char>^ newSubBitmapPixelData = gcnew array<unsigned char>(newSubBitmapwidth*newSubBitmapheight);
			newSubBitmapPixelData = bReader->ReadBytes(newSubBitmapwidth * newSubBitmapheight);
			newSubBitmapPixelDataArray[subBitmap] = newSubBitmapPixelData;
			newSubBitmapPixelDataArrayTotalContentsSize = newSubBitmapPixelDataArrayTotalContentsSize + newSubBitmapPixelData->Length;

			//Console::Write("bitmapPixelData1b->Length {0}", bitmapPixelData->Length);
			bReader->Close();

			//the palettes from each new subbitmap must all be identical
			swapPalette(newSubBitmapPalette);
			bitmapPalette = newSubBitmapPalette;	
		}
		finally
		{
		}
	}

	newmdlTextureData = gcnew array<unsigned char>(mdlBitmapHeader->Length + newSubBitmapHeaderArrayTotalContentsSize + newSubBitmapPixelDataArrayTotalContentsSize);
	int positionInNewMdlTextureData = 0;
	for(int i=0; i<mdlBitmapHeader->Length; i++)
	{
		newmdlTextureData[positionInNewMdlTextureData] = mdlBitmapHeader[i];
		positionInNewMdlTextureData++;
	}
	for(int subBitmap=0; subBitmap<mdlBitmapType+1; subBitmap++)
	{
		array<unsigned char>^ newSubBitmapHeader = newSubBitmapHeaderArray[subBitmap];
		array<unsigned char>^ newSubBitmapPixelData = newSubBitmapPixelDataArray[subBitmap];

		for(int i=0; i<newSubBitmapHeader->Length; i++)
		{
			newmdlTextureData[positionInNewMdlTextureData] = newSubBitmapHeader[i];
			positionInNewMdlTextureData++;
		}
		for(int i=0; i<newSubBitmapPixelData->Length; i++)
		{
			newmdlTextureData[positionInNewMdlTextureData] = newSubBitmapPixelData[i];
			positionInNewMdlTextureData++;
		}
	}
}

void exportBMPfile(System::String^ inputFileName, String^ outputFileNameBase)
{
	//Console::WriteLine("exportBMPfile");

	//mdl addresses
	int mdlPaletteAddress = MDL_HEADER_BITMAPPALETTE_ADDRESS;
	int mdlBitmapHeaderAddress;

	//mdl bitmap palette data
	array<unsigned char>^ mdlPalette = gcnew array<unsigned char>(BITMAP_PALETTE_LENGTH);

	BinaryReader^ bReader = gcnew BinaryReader(File::Open(inputFileName, FileMode::Open));
	try
	{
		//export mdl palette
		bReader->BaseStream->Seek(mdlPaletteAddress, SeekOrigin::Begin);
		mdlPalette = bReader->ReadBytes(mdlPalette->Length);
		swapPalette(mdlPalette);

		//get mdlBitmapType
		mdlBitmapHeaderAddress = (int)bReader->ReadUInt32();
		Console::WriteLine("mdlBitmapHeaderAddress = {0}", mdlBitmapHeaderAddress);
		bReader->BaseStream->Seek(mdlBitmapHeaderAddress, SeekOrigin::Begin);
		array<unsigned char>^ mdlBitmapHeaderType = bReader->ReadBytes(4);
		int mdlBitmapType = (BitConverter::ToUInt32(mdlBitmapHeaderType, 0))/MDL_BITMAP_HEADER_TYPE_MULTIPLIER - 1;	//4,8,12,16 => 0,1,2,3
		Console::WriteLine("mdlBitmapType = {0}", mdlBitmapType);

		//skip numberOfBytesOfMultiBitmapHeaderAdditionSpace
		int numberOfBytesOfMultiBitmapHeaderAdditionSpace = BYTES_PER_INT*mdlBitmapType;
		bReader->BaseStream->Seek(numberOfBytesOfMultiBitmapHeaderAdditionSpace, SeekOrigin::Current);	//skip numberOfBytesOfMultiBitmapHeaderAdditionSpace
			/*
			MDLbitmapHeaderType0 (4)
			020.MDL: startOfMDLheader = 630536 = 99F08
				[Check bitmap size = B9F24 - 99F24 = 20000x = 131072 = 512 * 256]

			MDLbitmapHeaderType1 (8)
			066.MDL: startOfMDLheader = 56784 = DDD0
			DDF0 + 10000 (256 * 256 in hex) = 1DDF0
				numberOfBytesOfMultiBitmapHeaderAdditionSpace:
				200001 = 2097153 =

			MDLbitmapHeaderType2 (12)
			128.MDL: startOfMDLheader = 95520 = 17520
			17544 + 10000 (256 * 256 in hex) = 27544
			2755D + 10000 (256 * 256 in hex) = 3755D
				numberOfBytesOfMultiBitmapHeaderAdditionSpace:
				240001 = 2359297 =
				3C0002 = 3932162 =

			MDLbitmapHeaderType3 (16)
			090.MDL: startOfMDLheader = 64708 = FCC4
			FCED + 20000 (512 * 256 in hex) = 2FCED (subbitmap header #2)
			2FD04 + 20000 (512 * 256 in hex) = 4FD04
			4FD1D + 20000 (512 * 256 in hex) = 6FD1D
				numberOfBytesOfMultiBitmapHeaderAdditionSpace::
				280002 = 2621442 =
				400004 = 4194308 =
				580006 = 5767174 =
			*/

		for(int subBitmap=0; subBitmap<mdlBitmapType+1; subBitmap++)
		{
			//export mdl subbitmap header
			array<unsigned char>^ SubBitmapHeader = gcnew array<unsigned char>(SUBBITMAP_HEADER_LENGTH);
			SubBitmapHeader = bReader->ReadBytes(SUBBITMAP_HEADER_LENGTH);
			array<unsigned char>^ temp1 = { SubBitmapHeader[SUBBITMAP_HEADER_WIDTH_MINUS_ONE_ADDRESS+0], SubBitmapHeader[SUBBITMAP_HEADER_WIDTH_MINUS_ONE_ADDRESS+1], SubBitmapHeader[SUBBITMAP_HEADER_WIDTH_MINUS_ONE_ADDRESS+2], SubBitmapHeader[SUBBITMAP_HEADER_WIDTH_MINUS_ONE_ADDRESS+3]};
			array<unsigned char>^ temp2 = { SubBitmapHeader[SUBBITMAP_HEADER_HEIGHT_MINUS_ONE_ADDRESS+0], SubBitmapHeader[SUBBITMAP_HEADER_HEIGHT_MINUS_ONE_ADDRESS+1], SubBitmapHeader[SUBBITMAP_HEADER_HEIGHT_MINUS_ONE_ADDRESS+2], SubBitmapHeader[SUBBITMAP_HEADER_HEIGHT_MINUS_ONE_ADDRESS+3]};
			int SubBitmapwidth = BitConverter::ToUInt32(temp1, 0) + 1;
			int SubBitmapheight = BitConverter::ToUInt32(temp2, 0) + 1;
			Console::WriteLine("SubBitmapwidth = {0}", SubBitmapwidth);
			Console::WriteLine("SubBitmapheight = {0}", SubBitmapheight);
			//export mdl subbitmap pixel data
			array<unsigned char>^ SubBitmapPixelData = gcnew array<unsigned char>(SubBitmapwidth * SubBitmapheight);
			SubBitmapPixelData = bReader->ReadBytes(SubBitmapPixelData->Length);

			String^ outputFileNameSubBitmap = outputFileNameBase;
			if(mdlBitmapType>0)
			{
				String^ mdlBitmapTypeString = convertIntToManagedString(subBitmap);
				outputFileNameSubBitmap = outputFileNameBase + "-" + mdlBitmapTypeString;
			}
			outputFileNameSubBitmap = outputFileNameSubBitmap + BMP_EXTENSION;

			writeBMP(outputFileNameSubBitmap, mdlPalette, SubBitmapPixelData, SubBitmapwidth, SubBitmapheight);
		}

		bReader->Close();
	}
	finally
	{
	}


}


static void writeBMP(String^ outputFileName, array<unsigned char>^ mdlPalette, array<unsigned char>^ pixelData, int bitmapwidth, int bitmapheight)
{
	BinaryWriter^ bWriter = gcnew BinaryWriter(File::Open(outputFileName, FileMode::Create));
	try
	{
		//write windows bitmap header (http://www.fastgraph.com/help/bmp_header_format.html)
		array<unsigned char>^ temp3 =  { 0x42, 0x4d }; //signature, must be 4D42 hex
		bWriter->Write(temp3); //signature, must be 4D42 hex
		bWriter->Write(54 + (mdlPalette->Length + 5) + pixelData->Length); //size of BMP file in bytes (unreliable)
		bWriter->Write((UInt16)0); //reserved, must be zero
		bWriter->Write((UInt16)0); //reserved, must be zero
		bWriter->Write(54 + (mdlPalette->Length + 5)); //offset to start of image data in bytes
		bWriter->Write(40); //size of BITMAPINFOHEADER structure, must be 40
		bWriter->Write(bitmapwidth); //image width in pixels
		bWriter->Write(bitmapheight); //image height in pixels
		bWriter->Write((UInt16)1); //number of planes in the image, must be 1
		bWriter->Write((UInt16)8); //number of bits per pixel (1, 4, 8, or 24)
		bWriter->Write(0); //compression type (0=none, 1=RLE-8, 2=RLE-4)
		bWriter->Write(bitmapwidth * bitmapheight); //size of image data in bytes
		bWriter->Write(2834); //horizontal resolution in pixels per meter (unreliable)
		bWriter->Write(2834); //vertical resolution in pixels per meter (unreliable)
		bWriter->Write(0); //number of colors in image, or zero
		bWriter->Write(0); //number of important colors, or zero
		bWriter->Write(0); //added 22 Feb 2013 - to account for MDL palette size specification (0x427 - 0x2D + 1 = 1019)
		//write palette
		bWriter->Write(mdlPalette);
		//Console::WriteLine("mdlPalette->Length = {0}", mdlPalette->Length);
		bWriter->Write((Char)0); //1 byte padding	//changed char -> Char (byte) 8 March 2013
		//write data
		bWriter->Write(pixelData);

		bWriter->Close();
	}
	finally
	{
	}
}

static void insertByteArray(array<unsigned char>^ byteArrayToUpdate, array<unsigned char>^ byteArrayToInsert, int index)
{
	for(int i=0; i<byteArrayToInsert->Length; i++)
	{
		byteArrayToUpdate[i+index] = byteArrayToInsert[i];
	}
}

static int calculatePowerOf2(int value)
{
	int powerOf2 = 0;
	while(((value % 2) == 0) && (value > 1))
	{
		value = value/2;
		powerOf2++;
	}
	return powerOf2;
}

static void swapPalette(array<unsigned char>^ palette)
{
	//swap RGB to BGR
	char * tmpPalette = new char[BITMAP_PALETTE_LENGTH];

	for (int i = 0; i < BITMAP_PALETTE_LENGTH; i += 4)
	{
		tmpPalette[i] = palette[(i + 2)];
		tmpPalette[(i + 1)] = palette[(i + 1)];
		tmpPalette[(i + 2)] = palette[i];
		if ((i + 4) < BITMAP_PALETTE_LENGTH)	//this check is probably no longer required from 22 Feb 2013
		{
			tmpPalette[(i + 3)] = (char)0;
		}
	}

	for(int i=0; i<BITMAP_PALETTE_LENGTH; i++)
	{
		palette[i] = tmpPalette[i];
	}
}


