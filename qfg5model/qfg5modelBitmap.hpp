/*******************************************************************************
 *
 * File Name: qfg5modelBitmap.hpp
 * Author(s): Richard Bruce Baxter - Copyright (c) 2013 Baxter AI (baxterai.com)
 *            Robert Monnig - Copyright (c) 2013 Monnig Design (monnigdesign.co.nz)
 * Project: QFG5 Model
 * Project Version: 08-March-2013c
 * Description: Bitmap i/o functions
 *
 *******************************************************************************/

void importBMPfile(System::String^ inputFileName, array<unsigned char>^ mdlTextureData, array<unsigned char>^ % bitmapPalette, array<unsigned char>^ % newmdlTextureData);
void exportBMPfile(System::String^ inputFileName, System::String^ outputFileName);
