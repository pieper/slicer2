/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
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
// .NAME vtkMrmlVolumeNode - Writes images to files.
// .SECTION Description
// vtkMrmlVolumeNode writes images to files with any data type. The data type of

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
  const char *GetClassName() {return "vtkMrmlVolumeNode";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlVolumeNode *node);

  // Description:
  // Write the node's attributes
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Header Information
  //--------------------------------------------------------------------------

  // Description:
  //
  vtkGetVector2Macro(ImageRange, int);
  vtkSetVector2Macro(ImageRange, int);

  // Description:
  //
  vtkGetVector2Macro(Dimensions, int);
  vtkSetVector2Macro(Dimensions, int);

  // Description:
  //
  vtkGetVector3Macro(Spacing, float);
  vtkSetVector3Macro(Spacing, float);
  
  // Description:
  // Scalar Type
  vtkSetMacro(ScalarType, int);
  vtkGetMacro(ScalarType, int);
  void SetScalarTypeToFloat() {this->SetScalarType(VTK_FLOAT);};
  void SetScalarTypeToInt() {this->SetScalarType(VTK_INT);};
  void SetScalarTypeToShort() {this->SetScalarType(VTK_SHORT);};
  void SetScalarTypeToUnsignedShort() 
    {this->SetScalarType(VTK_UNSIGNED_SHORT);};
  void SetScalarTypeToUnsignedChar() 
    {this->SetScalarType(VTK_UNSIGNED_CHAR);};
  
  // Description:
  //
  vtkGetMacro(NumScalars, int);
  vtkSetMacro(NumScalars, int);

  // Description:
  //
  vtkGetMacro(LittleEndian, int);
  vtkSetMacro(LittleEndian, int);
  vtkBooleanMacro(LittleEndian, int);

  // Description:
  //
  vtkGetMacro(Tilt, float);
  vtkSetMacro(Tilt, float);


  //--------------------------------------------------------------------------
  // RAS->IJK Matrix Calculation
  //--------------------------------------------------------------------------

  // Description:
  // Set/Get the order string which designates the order the slices 
  // were scanned.  Possible values are: LR,RL,AP,PA,SI,IS
  void ComputeRasToIjkFromScanOrder(char *order);
  void SetScanOrder(char *s);
  vtkGetStringMacro(ScanOrder);

  // Description:
  void ComputeRasToIjkFromCorners(float *fc, float *ftl, float *ftr, 
    float *fbr, float *lc, float *ltl);
  void ComputeRasToIjkFromCorners(
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
    this->ComputeRasToIjkFromCorners(fc, ftl, ftr, fbr, lc, ltl);};

  // Description:
  // 
  vtkSetStringMacro(RasToIjkMatrix);
  vtkGetStringMacro(RasToIjkMatrix);

  // Description:
  // 
  vtkSetStringMacro(RasToVtkMatrix);
  vtkGetStringMacro(RasToVtkMatrix);


  //--------------------------------------------------------------------------
  // Non-Header Information
  //--------------------------------------------------------------------------
  
  // Description:
  // 
  vtkSetStringMacro(FilePrefix);
  vtkGetStringMacro(FilePrefix);

  // Description:
  // 
  vtkSetStringMacro(FilePattern);
  vtkGetStringMacro(FilePattern);

  // Description:
  //
  vtkSetStringMacro(Name);
  vtkGetStringMacro(Name);

  
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


  //--------------------------------------------------------------------------
  // Display Information
  //--------------------------------------------------------------------------
  
  // Description:
  // 
  vtkSetStringMacro(LUTName);
  vtkGetStringMacro(LUTName);

  // Description:
  //
  vtkGetMacro(LabelMap, int);
  vtkSetMacro(LabelMap, int);
  vtkBooleanMacro(LabelMap, int);

  // Description:
  //
  vtkBooleanMacro(AutoWindowLevel, int);
  vtkGetMacro(AutoWindowLevel, int);
  vtkSetMacro(AutoWindowLevel, int);
  vtkGetMacro(Window, int);
  vtkSetMacro(Window, int);
  vtkGetMacro(Level, int);
  vtkSetMacro(Level, int);

  // Description:
  //
  vtkBooleanMacro(AutoThreshold, int);
  vtkGetMacro(AutoThreshold, int);
  vtkSetMacro(AutoThreshold, int);
  vtkGetMacro(UpperThreshold, int);
  vtkSetMacro(UpperThreshold, int);
  vtkGetMacro(LowerThreshold, int);
  vtkSetMacro(LowerThreshold, int);

  // Description:
  // Set/Get interpolate reformated slices
  vtkGetMacro(Interpolate, int);
  vtkSetMacro(Interpolate, int);
  vtkBooleanMacro(Interpolate, int);


private:
  vtkMrmlVolumeNode();
  ~vtkMrmlVolumeNode();
  vtkMrmlVolumeNode(const vtkMrmlVolumeNode&) {};
  void operator=(const vtkMrmlVolumeNode&) {};

  void ComputeRasToIjk(vtkMatrix4x4 *matRotate, 
    float ox, float oy, float oz);

  char *Name;
  char *FilePattern;
  char *FilePrefix;
  char *FullPrefix;
  char *RasToIjkMatrix;
  char *RasToVtkMatrix;
  char *LUTName;
  char *ScanOrder;

  float Tilt;
  float Spacing[3];
  int ImageRange[2];
  int Dimensions[2];

  int UseRasToVtkMatrix;
  int LabelMap;
  int LittleEndian;
  int ScalarType;
  int NumScalars;

  int Interpolate;
  int AutoWindowLevel;
  int Window;
  int Level;
  int AutoThreshold;
  int UpperThreshold;
  int LowerThreshold;

  vtkMatrix4x4 *WldToIjk;
  vtkMatrix4x4 *RasToWld;
  vtkMatrix4x4 *RasToIjk;

};

#endif

