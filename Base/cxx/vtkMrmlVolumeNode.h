/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
// .NAME vtkMrmlVolumeNode - MRML node for representing a volume (image stack).
// .SECTION Description
// Volume nodes describe data sets that can be thought of as stacks of 2D 
// images that form a 3D volume.  Volume nodes describe where the images 
// are stored on disk, how to render the data (window and level), and how 
// to read the files.  This information is extracted from the image 
// headers (if they exist) at the time the MRML file is generated.  
// Consequently, MRML files isolate MRML browsers from understanding how 
// to read the myriad of file formats for medical data. 

#ifndef __vtkMrmlVolumeNode_h
#define __vtkMrmlVolumeNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkSlicer.h"
#include "vtkMrmlVolumeReadWriteNode.h"


// found this the hard way: this had been a 'magic number' that allowed
// the user to run off the end of the array with no error checking -- tsk tsk.
#define DICOM_FILE_LIMIT 1000


class VTK_SLICER_BASE_EXPORT vtkMrmlVolumeNode : public vtkMrmlNode
{
  public:
  static vtkMrmlVolumeNode *New();
  vtkTypeMacro(vtkMrmlVolumeNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // these control the center of the voxel in ijk space - default is 0.5, center
  // of the voxel while 0.0 would be upper corner
  static void SetGlobalVoxelOffset(float offset);
  static float GetGlobalVoxelOffset();
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);


  //--------------------------------------------------------------------------
  // How to Read/Write volume data contained by this node.
  // This sub-node should contain information specific to each
  // type of volume that needs to be read in.  This can be used
  // to clean up the special cases in this file which handle
  // volumes of various types, such as dicom, header, etc.  In
  // future these things can be moved to the sub-node specific for that
  // type of volume.  The sub-nodes here that describe specific volume
  // types each correspond to an implementation of the reader/writer,
  // which can be found in a vtkMrmlDataVolumeReadWrite subclass.
  //--------------------------------------------------------------------------
  vtkSetObjectMacro(ReadWriteNode, vtkMrmlVolumeReadWriteNode);
  vtkGetObjectMacro(ReadWriteNode, vtkMrmlVolumeReadWriteNode);

  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Volume ID
  vtkSetStringMacro(VolumeID);
  vtkGetStringMacro(VolumeID);
  
  // Description:
  // The pattern for naming the files expressed in C language syntax. 
  // For example, a file named skin.001 has a patter of %s.%03d, and a file
  // named skin.1 has a pattern of %s.%d
  vtkSetStringMacro(FilePattern);
  vtkGetStringMacro(FilePattern);

  // Description:
  // Prefix of the image files. For example, the prefix of /d/skin.001 is /d/skin
  vtkSetStringMacro(FilePrefix);
  vtkGetStringMacro(FilePrefix);

  // Description:
  // Type of data file - e.g. Analyze, Dicom, Headerless, etc.  If not set, slicer will 
  // try to figure it from the other header values
  vtkSetStringMacro(FileType);
  vtkGetStringMacro(FileType);

  //--------------------------------------------------------------------------
  // Header Information
  //--------------------------------------------------------------------------

  // Description:
  // Two numbers: the first and last image numbers in the volume. 
  // For example: 1 124
  vtkGetVector2Macro(ImageRange, int);
  vtkSetVector2Macro(ImageRange, int);

  // Description:
  // Two numbers: the number of columns and rows of pixels in each image
  vtkGetVector2Macro(Dimensions, int);
  vtkSetVector2Macro(Dimensions, int);

  // Description:
  // Three numbers for the dimensions of each voxel, in millimeters
  vtkGetVector3Macro(Spacing, float);
  vtkSetVector3Macro(Spacing, float);
  
  // Description:
  // The type of data in the file. One of: Char, UnsignedChar, Short, 
  // UnsignedShort, Int, UnsignedInt, Long, UnsignedLong, Float, Double
  vtkSetMacro(ScalarType, int);
  vtkGetMacro(ScalarType, int);
  void SetScalarTypeToUnsignedChar() 
    {this->SetScalarType(VTK_UNSIGNED_CHAR);};
  void SetScalarTypeToChar() 
    {this->SetScalarType(VTK_CHAR);};
  void SetScalarTypeToShort() {
    this->SetScalarType(VTK_SHORT);};
  void SetScalarTypeToUnsignedShort() 
    {this->SetScalarType(VTK_UNSIGNED_SHORT);};
  void SetScalarTypeToInt() {
    this->SetScalarType(VTK_INT);};
  void SetScalarTypeToUnsignedInt() {
    this->SetScalarType(VTK_UNSIGNED_INT);};
  void SetScalarTypeToLong() {
    this->SetScalarType(VTK_LONG);};
  void SetScalarTypeToUnsignedLong() {
    this->SetScalarType(VTK_UNSIGNED_LONG);};
  void SetScalarTypeToFloat() {
    this->SetScalarType(VTK_FLOAT);};
  void SetScalarTypeToDouble() {
    this->SetScalarType(VTK_DOUBLE);};
  char* GetScalarTypeAsString();
  
  // Description:
  // The number of scalar components for each voxel. 
  // Gray-level data has 1. Color data has 3
  vtkGetMacro(NumScalars, int);
  vtkSetMacro(NumScalars, int);

  // Description:
  // Describes the order of bytes for each voxel.  Little endian 
  // positions the least-significant byte on the rightmost end, 
  // and is true of data generated on a PC or SGI.
  vtkGetMacro(LittleEndian, int);
  vtkSetMacro(LittleEndian, int);
  vtkBooleanMacro(LittleEndian, int);

  // Description:
  // The gantry tilt, in degrees, for CT slices
  vtkGetMacro(Tilt, float);
  vtkSetMacro(Tilt, float);


  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Numerical ID of the color lookup table to use for rendering the volume
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

  // Description:
  // Indicates if this volume is a label map, which is the output of 
  // segmentation that labels each voxel according to its tissue type.  
  // The alternative is a gray-level or color image.
  vtkGetMacro(LabelMap, int);
  vtkSetMacro(LabelMap, int);
  vtkBooleanMacro(LabelMap, int);

  // Description:
  // Specifies whether windowing and leveling are to be performed automatically
  vtkBooleanMacro(AutoWindowLevel, int);
  vtkGetMacro(AutoWindowLevel, int);
  vtkSetMacro(AutoWindowLevel, int);

  // Description:
  // The window value to use when autoWindowLevel is 'no'
  vtkGetMacro(Window, float);
  vtkSetMacro(Window, float);

  // Description:
  // The level value to use when autoWindowLevel is 'no'
  vtkGetMacro(Level, float);
  vtkSetMacro(Level, float);

  // Description:
  // Specifies whether to apply the threshold
  vtkBooleanMacro(ApplyThreshold, int);
  vtkGetMacro(ApplyThreshold, int);
  vtkSetMacro(ApplyThreshold, int);

  // Description:
  // Specifies whether the threshold should be set automatically
  vtkBooleanMacro(AutoThreshold, int);
  vtkGetMacro(AutoThreshold, int);
  vtkSetMacro(AutoThreshold, int);

  // Description:
  // The upper threshold value to use when autoThreshold is 'no'
  vtkGetMacro(UpperThreshold, int);
  vtkSetMacro(UpperThreshold, int);

  // Description:
  // The lower threshold value to use when autoThreshold is 'no'
  vtkGetMacro(LowerThreshold, int);
  vtkSetMacro(LowerThreshold, int);

  // Description:
  // MR Diffusion Tensor Images may be saved on disk with 
  // the frequency encode direction non-standard.  For supine
  // scans in the slicer, set this to 1 for in-plane 
  // rotation/axis swaps which will put the images into standard
  // ax/sag/cor orientations.  This is currently a boolean variable
  // here and not written to MRML; its effects are seen in the 
  // RasToIjk matrix.  In future this boolean, or a better description
  // of the transformation, may be put into the MRML format.
  vtkGetMacro(FrequencyPhaseSwap, int);
  vtkSetMacro(FrequencyPhaseSwap, int);

  // Description:
  // Set/Get interpolate reformated slices
  vtkGetMacro(Interpolate, int);
  vtkSetMacro(Interpolate, int);
  vtkBooleanMacro(Interpolate, int);

  //--------------------------------------------------------------------------
  // RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  // Description:
  // The order of slices in the volume. One of: LR (left-to-right), 
  // RL, AP, PA, IS, SI. This information is encoded in the rasToIjkMatrix.
  // This matrix can be computed either from corner points, or just he
  // scanOrder.
  void ComputeRasToIjkFromScanOrder(char *order);
  void SetScanOrder(char *s);
  vtkGetStringMacro(ScanOrder);

  // Description:
  // Compute the rasToIjkMatrix from the corner points of the volume.
  int ComputeRasToIjkFromCorners(float *fc, float *ftl, float *ftr, 
    float *fbr, float *lc, float *ltl);
  int ComputeRasToIjkFromCorners(
    float fcR,  float fcA,  float fcS,
    float ftlR, float ftlA, float ftlS, 
    float ftrR, float ftrA, float ftrS, 
    float fbrR, float fbrA, float fbrS, 
    float lcR,  float lcA,  float lcS, 
    float ltlR, float ltlA, float ltlS) {
    float fc[3], ftl[3], ftr[3], fbr[3], lc[3], ltl[3];
    fc[0]=fcR; fc[1]=fcA; fc[2]=fcS;
    ftl[0]=ftlR; ftl[1]=ftlA; ftl[2]=ftlS;
    ftr[0]=ftrR; ftr[1]=ftrA; ftr[2]=ftrS;
    fbr[0]=fbrR; fbr[1]=fbrA; fbr[2]=fbrS;
    lc[0]=lcR; lc[1]=lcA; lc[2]=lcS;
    ltl[0]=ltlR; ltl[1]=ltlA; ltl[2]=ltlS;
    return(this->ComputeRasToIjkFromCorners(fc, ftl, ftr, fbr, lc, ltl));};

  // Description:
  // The matrix that transforms a point in RAS space to IJK space. 
  // The 4x4 matrix is listed as a string of 16 numbers in row-major order.
  vtkSetStringMacro(RasToIjkMatrix);
  vtkGetStringMacro(RasToIjkMatrix);

  // Description:
  // Same as the rasToIjkMatrix except the y-axis is directed up instead 
  // of down, as is the convention in VTK
  vtkSetStringMacro(RasToVtkMatrix);
  vtkGetStringMacro(RasToVtkMatrix);

  // Description:
  // A ScaledIJK-to-RAS matrix formed by accounting for the position and 
  // orientation of the volume, but not the voxel size
  vtkSetStringMacro(PositionMatrix);
  vtkGetStringMacro(PositionMatrix);


  // Description:
  // This function solves the 4x4 matrix equation
  // A*B=C for the unknown matrix A, given matrices B and C.
  // While this is equivalent to A=C*Inverse(B), this function uses
  // faster and more accurate methods (LU factorization) than finding a 
  // matrix inverse and multiplying.  Returns 0 on failure.
  static int SolveABeqCforA(vtkMatrix4x4 * A,  vtkMatrix4x4 * B,
                vtkMatrix4x4 * C);
  static int SolveABeqCforB(vtkMatrix4x4 * A,  vtkMatrix4x4 * B,
                vtkMatrix4x4 * C);

  //--------------------------------------------------------------------------
  // Used by the MRML Interpreter
  //--------------------------------------------------------------------------
  
  // Description:
  // Full path name
  vtkSetStringMacro(FullPrefix);
  vtkGetStringMacro(FullPrefix);

  // Description:
  // Specify whether to use the RasToIjk or RasToVtk (y-flipped) matrix
  vtkGetMacro(UseRasToVtkMatrix, int);
  vtkSetMacro(UseRasToVtkMatrix, int);
  vtkBooleanMacro(UseRasToVtkMatrix, int);

  // Description:
  // Specify the registration matrix which this routine uses
  // to rebuild the WldToIjk matrix.
  // Uses the value of UseRasToVtkMatrix.
  void SetRasToWld(vtkMatrix4x4 *reg);

  // Description:
  // Get the final result of registration
  vtkGetObjectMacro(WldToIjk, vtkMatrix4x4);
  vtkGetObjectMacro(RasToWld, vtkMatrix4x4);
  vtkGetObjectMacro(RasToIjk, vtkMatrix4x4);
  vtkGetObjectMacro(Position, vtkMatrix4x4);

  // Added by Attila Tanacs 10/10/2000 1/4/02

  // DICOMFileList
  int GetNumberOfDICOMFiles() { return DICOMFiles; }
  void AddDICOMFileName(char *);
  char *GetDICOMFileName(int idx);
  void SetDICOMFileName(int idx, char *str);
  void DeleteDICOMFileNames();
  char **GetDICOMFileNamesPointer() { return DICOMFileList;}

  int GetNumberOfDICOMMultiFrameOffsets() {return DICOMMultiFrameOffsets;}
  void AddDICOMMultiFrameOffset(int offset);
  int GetDICOMMultiFrameOffset(int idx);
  void DeleteDICOMMultiFrameOffsets();
  int *GetDICOMMultiFrameOffsetPointer() {return DICOMMultiFrameOffsetList;}
  // End
  
protected:
  vtkMrmlVolumeNode();
  ~vtkMrmlVolumeNode();
  vtkMrmlVolumeNode(const vtkMrmlVolumeNode&) {};
  void operator=(const vtkMrmlVolumeNode&) {};

  // Strings
  char *VolumeID;
  char *FilePattern;
  char *FilePrefix;
  char *FullPrefix;
  char *FileType;
  char *RasToIjkMatrix;
  char *RasToVtkMatrix;
  char *PositionMatrix;
  char *LUTName;
  char *ScanOrder;

  // Numbers
  float Tilt;
  int ScalarType;
  int NumScalars;
  float Window;
  float Level;
  int UpperThreshold;
  int LowerThreshold;

  // odonnell.  Fixes for diffusion tensor image data
  int FrequencyPhaseSwap;

  // Arrays
  float Spacing[3];
  int ImageRange[2];
  int Dimensions[2];

  // Booleans
  int UseRasToVtkMatrix;
  int LabelMap;
  int LittleEndian;
  int Interpolate;
  int AutoWindowLevel;
  int ApplyThreshold;
  int AutoThreshold;

  vtkMatrix4x4 *WldToIjk;
  vtkMatrix4x4 *RasToWld;
  vtkMatrix4x4 *RasToIjk;
  vtkMatrix4x4 *Position;

  // Added by Attila Tanacs 10/10/2000 1/4/02
  int DICOMFiles;
  char **DICOMFileList;

  int DICOMMultiFrameOffsets;
  int *DICOMMultiFrameOffsetList;
  // End
  
  // odonnell, 07/2002
  vtkMrmlVolumeReadWriteNode *ReadWriteNode;
};

#endif

