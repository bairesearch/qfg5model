# Qfg5model

![Comparison.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/Comparison.png)

## Background

Qfg5model is a small command-line program to export and import textures and meshes from Quest for Glory 5. The current release also supports importing higher resolution textures and meshes.

The textures component was created by GMFattay (with contributions from others), and the meshes component was created by richardbrucebaxter.

## Contents

  * 1 High Resolution Mesh/Texture Downloads
  * 2 Exporting & Importing textures and 3d meshes using qfg5model
    * 2.1 Extracting a MDL file for qfg5model
    * 2.2 Texture Upgrades using qfg5model
    * 2.3 3D Mesh Upgrades using qfg5model
    * 2.4 Software Downloads
    * 2.5 Examples

## High Resolution Mesh/Texture Downloads

To install the high resolution models, just copy the extracted `DATA/mdl` folder (files) to the `[QFG5 installation folder]/Data/mdl` folder. QFG5 will then load these models instead of its original versions (`hdnw.SPK`).

High resolution meshes (does not include high resolution textures);

  * [qfg5modelHighResolutionMeshes-08March2013c.zip](https://github.com/bairesearch/qfg5model/releases/download/Files/qfg5modelHighResolutionMeshes-08March2013c.zip)

High resolution texture for Hero (MDL and PSD source);

  * [http://www.qfgmods.net/files/heroHD-12August2013.rar](https://github.com/bairesearch/qfg5model/releases/download/Files/heroHD-12August2013.rar)


## Exporting & Importing textures and 3d meshes using qfg5model

You can extract MDL files files from SPK archives using Game Extractor. The MDL files are located in the `hdnw.SPK` archive.

Put any modified MDL files under `[QFG5 installation folder]/Data/mdl` and the game engine will load the modified files over the original files.

Usage: `qfg5model <input file> <output file>`

* To export MDL to BMP, set input file `.MDL` and output file `.BMP`
* To import BMP to MDL, set input file `.BMP` and output file `.MDL`
* To export MDL to HAK, set input file `.MDL` and output file `.HAK`
* To import HAK to MDL, set input file `.HAK` and output file `.MDL`

HAK = 3D mesh (to generate high resolution mesh use hakenberg subdivision)

The current software release is available here: [qfg5modelDevel-08March2013cSMALL.zip](https://github.com/bairesearch/qfg5model/releases/download/Files/qfg5modelDevel-08March2013cSMALL.zip)

### Extracting an MDL file for qfg5model

1. Extract an MDL file from a QFG5 .SPK archive:
  * Ensure Java Runtime Environment (JRE) is installed ([java.com/en/download/index.jsp](java.com/en/download/index.jsp), e.g. `jre-7uX-windows-i586.exe/x64.exe`)  
  * Download and install WinRAR ([http://www.rarlab.com/download.htm](http://www.rarlab.com/download.htm) e.g. `wrar420.exe`)
  * Download QFG5 SPK plugin [Plugin_SPK_2.zip](https://github.com/bairesearch/qfg5model/releases/download/Files/Plugin_SPK_2.zip)
  * Download and Install Game Extractor [extract_201.exe](https://sourceforge.net/projects/gameextractor/files/Game%20Extractor%202.0x/2.01/extract_201.exe/download) - may require administrator privileges
  * Use WinRAR to open `[Game Extractor installation folder]/GameExtractor.jar`, and add file `Plugin_SPK_2.class`
  * Run Game Extractor - may require administrator privileges (right click - run as administrator)
  * Open `[QFG5 installation folder]/Data/HDNW.SPK` with Game Extractor
  * Find and extract an MDL file (eg `mdl/010.MDL`)
2. Download and install qfg5model:
  * Download and install the latest version of `qfg5model.exe` [qfg5modelDevel-08March2013cSMALL.zip](https://github.com/bairesearch/qfg5model/releases/download/Files/qfg5modelDevel-08March2013cSMALL.zip)
    * To compile it yourself use MS Visual C++ 2010 (open `qfg5model.sln`, and Build All / click F7)
  * Create a backup of original MDL file (`copy 010.MDL 010original.MDL`)
  * Place `010.MDL` in the same folder as `qfg5model.exe`

### Texture Upgrades using qfg5model

Here is an example of how to upgrade a model's skin (e.g. `010.MDL`):
1. Extract an MDL file for qfg5model (e.g. `010.MDL`) [see above]
2. Upgrade the model's texture using qfg5model:
  * Open Windows command prompt (Start - search - cmd)
  * `cd [qfg5model.exe installation folder]`
  * `qfg5model.exe 010.MDL 010.BMP`
  * Enhance `010.BMP` using an image editor (e.g. Photoshop/PSP/GIMP) and increase its resolution
    * (Ensure to save the image back to a 256 bit palette. E.g. save the original 256 color palette, perform image touch ups, then reapplying it)
  * `qfg5model.exe 010.BMP 010.MDL`
3. Install the MDL File and play QFG5:
  * Copy the MDL file to the `[QFG5 installation folder]/Data/mdl` folder (create this folder if it doesn't exist)
  * Play the game

Example Screenshots;

![http://i.imgur.com/rRvNah3.png](http://i.imgur.com/rRvNah3.png)

### 3D Mesh Upgrades using qfg5model

Here is an example of how to upgrade a model's 3D Mesh (e.g. `010.MDL`):
1. Extract a MDL file for qfg5model (e.g. `010.MDL`) [see above]
2. Extract the HAK file from the MDL file:
  * Open Windows command prompt (Start - search - cmd)
  * `cd [qfg5model.exe installation folder]`
  * `qfg5model.exe 010.MDL 010.HAK`
    * Note this will generate HAK files for each submesh within `010.MDL`; eg `010-0-footleft.hak`
3. Convert HAK file using Hakenberg subdivision software:
  * Download [Hakenberg software](http://hakenberg.de/subdivision/ultimate_consumer.htm). The Hakenberg `mesh3d.exe`/`subdiv.exe` program can be downloaded along with the source code on their website
  * Either a) use `subdiv.exe` straight from [subdivision.zip](http://hakenberg.de/subdivision/consumer/subdivision.zip) [untested]) or b) compile a version of `mesh3d.exe` from the source code ([volutil.zip](http://hakenberg.de/subdivision/general/volutil.zip)) using [Visual C++ 2010](http://www.microsoft.com/visualstudio/eng#products/visual-studio-2010-express)
    * To compile one's own version of the Hakenberg subdivision software:
      * Create MS Visual Studio 2010 console project
      * import relevant `.cpp`/`.hpp` files
      * Build All / click F7, and manually fix up all compilation errors;
        * Comment out all error throwing template references; eg `template <class type> array<type>::array(char *fn)`
        * In `array.hpp`, comment out this line; `//if (array_output) cout << fn << " ->\t" << *this;`
      * Open Start - All Programs - MS Visual Studio 2010 Express - Visual Studio Command Prompt (2010).
        * Note this will enable use of `cl.exe` (the VS compiler) within `compile.bat`. The software must be compiled using `cl.exe` to use it (else it may crash).
      * `cd [mesh3d.exe installation folder]`
      * `compile.bat`
  * Execute `subdiv.exe`/`mesh3d.exe` manually or create and execute a script (e.g. [qfg5modelHAKconversionExampleScript.bat](https://github.com/bairesearch/qfg5model/releases/download/Files/qfg5modelHAKconversionExampleScript.bat))
    * High resolution example: `mesh3d.exe 010-0-footleft.hak b s`
    * Ultrahigh resolution example: `mesh3d.exe 010-0-footleft.hak b s s`
      * Note this will generate `.hak.out` files
4. Import the HAK(.out) files into the MDL file
  * to prevent having to rename the `.hak.out` files back to `.hak` you can uncomment `#define MDL_DEVEL_IMPORT_HAK_FILE_EXPECT_APPENDED_DOT_OUT_TO_FILE_NAME` in `stdafx.h` (and recompile `qfg5model.exe`)
  * `qfg5model.exe 010.HAK 010.MDL`
5. Perform manual touch-ups to high resolution mesh (e.g. nose shape);
  * Create a backup of the new high resolution MDL file (`copy 010.MDL 010highres.MDL`)
  * locate the high resolution .HAK(.out) files generated by `mesh3d.exe`
  * export high res LDR files from the high resolution MDL file (`qfg5model.exe 010.MDL 010.HAK`)
    * (note this will also export a new set of high resolution HAK files if the original ones generated by mesh3d.exe are not available)
  * open the LDR file corresponding to the problematic submesh with LDView (e.g faceX), and identify which polygons/vertices need modification (use LDView in conjunction with a text editor, e.g. TextPad, to determine the specific nature of those modifications, e.g. move vertex XYZ by +q in the Y direction)
  * manually apply these modifications to the corresponding high res HAK file (using a text editor)
  * reimport the (touched-up) high res HAK files into the original MDL file
    * `copy 010original.MDL 010.MDL`
    * `qfg5model.exe 010.HAK 010.MDL`
6. Install the MDL File and play QFG5:
  * Copy the MDL file to the `[QFG5 installation folder]/Data/mdl` folder (create this folder if it doesn't exist)
  * Play the game

### Software Downloads

The latest software/source (qfg5model.exe);

  * [qfg5modelDevel-08March2013cSMALL.zip](https://github.com/bairesearch/qfg5model/releases/download/Files/qfg5modelDevel-08March2013cSMALL.zip)

Batch scripts;

  * [batchUpgradeMesh.bat](https://github.com/bairesearch/qfg5model/releases/download/Files/batchUpgradeMesh.bat)
  * [batchExtractBitmap.bat](https://github.com/bairesearch/qfg5model/releases/download/Files/batchExtractBitmap.bat)

### Examples

*Note the following examples were generated before face geometry touch-ups were applied.*

Submodel screenshots (HAK/LDR);

  * qfg5modelWithHakenberg3dMesh-medResDemo.gif

![qfg5modelWithHakenberg3dMesh-medResDemo.gif](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelWithHakenberg3dMesh-medResDemo.gif)

  * qfg5modelWithHakenberg3dMesh-hiResDemo.gif

![qfg5modelWithHakenberg3dMesh-hiResDemo.gif](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelWithHakenberg3dMesh-hiResDemo.gif)

  * qfg5modelWithHakenberg3dMesh-ultraHiResDemo.gif

![qfg5modelWithHakenberg3dMesh-ultraHiResDemo.gif](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelWithHakenberg3dMesh-ultraHiResDemo.gif)

Short in-game animations (<2MB);
  * [qfg5modelHighResolutionMeshExampleAnimation1.mp4](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelHighResolutionMeshExampleAnimation1.mp4)

  * [qfg5modelHighResolutionMeshExampleAnimation2.mp4](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelHighResolutionMeshExampleAnimation2.mp4)

  * qfg5modelHighResolutionMeshExampleAnimation2.gif

![qfg5modelHighResolutionMeshExampleAnimation2.gif](https://github.com/bairesearch/qfg5model/releases/download/Examples/qfg5modelHighResolutionMeshExampleAnimation2.gif)

Example in-game screenshots;

  * batty1.png

![batty1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/batty1.png)

  * cerberus1.png

![cerberus1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/cerberus1.png)

  * guards1.png

![guards1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/guards1.png)

  * hippie.png

![hippie.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/hippie.png)

  * katrina.png

![katrina.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/katrina.png)

  * ogres2.png

![ogres2.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/ogres2.png)

  * pegasus1.png

![pegasus1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/pegasus1.png)

  * pegasus2.png

![pegasus2.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/pegasus2.png)

  * rakeesh.png

![rakeesh.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/rakeesh.png)

  * statue1.png

![statue1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/statue1.png)

  * town1.png

![town1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/town1.png)

  * town2.png

![town2.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/town2.png)

  * wolfie1.png

![wolfie1.png](https://github.com/bairesearch/qfg5model/releases/download/Examples/wolfie1.png)

