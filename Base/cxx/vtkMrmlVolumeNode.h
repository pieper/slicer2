/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
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

#include <iostream.h>
#include <fstream.h>
#include "vtkMrmlNode.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT vtkMrmlVolumeNode : public vtkMrmlNode
{
public:
  static vtkMrmlVolumeNode *New();
  vtkTypeMacro(vtkMrmlVolumeNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlVolumeNode *node);

  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);


  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // Name displayed on the user interface
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

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
  vtkGetMacro(Window, int);
  vtkSetMacro(Window, int);

  // Description:
  // The level value to use when autoWindowLevel is 'no'
  vtkGetMacro(Level, int);
  vtkSetMacro(Level, int);

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

  // Added by Attila Tanacs 10/10/2000

  // DICOMFileList
  int GetNumberOfDICOMFiles() { return DICOMFiles; }
  void AddDICOMFileName(char *);
  char *GetDICOMFileName(int idx);
  void DeleteDICOMFileNames();
  char **GetDICOMFileNamesPointer() { return DICOMFileList;}

  // End

protected:
  vtkMrmlVolumeNode();
  ~vtkMrmlVolumeNode();
  vtkMrmlVolumeNode(const vtkMrmlVolumeNode&) {};
  void operator=(const vtkMrmlVolumeNode&) {};

  void ComputeRasToIjk(vtkMatrix4x4 *matRotate, 
    float ox, float oy, float oz);

  // Strings
  char *Name;
  char *FilePattern;
  char *FilePrefix;
  char *FullPrefix;
  char *RasToIjkMatrix;
  char *RasToVtkMatrix;
  char *PositionMatrix;
  char *LUTName;
  char *ScanOrder;

  // Numbers
  float Tilt;
  int ScalarType;
  int NumScalars;
  int Window;
  int Level;
  int UpperThreshold;
  int LowerThreshold;

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

  // Added by Attila Tanacs 10/10/2000
  int DICOMFiles;
  char **DICOMFileList;
  // End
};

#endif

