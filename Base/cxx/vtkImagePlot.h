/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImagePlot - duh
// .SECTION Description
// This filter does stuff

#ifndef __vtkImagePlot_h
#define __vtkImagePlot_h

#include "vtkImageToImageFilter.h"
#include "vtkScalarsToColors.h"

class VTK_EXPORT vtkImagePlot : public vtkImageToImageFilter
{
public:
  static vtkImagePlot *New();
  const char *GetClassName() {return "vtkImagePlot";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetMacro(Height, int);
  vtkGetMacro(Height, int);

  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetMacro(Thickness, int);
  vtkGetMacro(Thickness, int);

  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetVector2Macro(DataRange, int);
  vtkGetVector2Macro(DataRange, int);

  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetVector2Macro(DataDomain, int);
  vtkGetVector2Macro(DataDomain, int);

  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetVector3Macro(Color, float);
  vtkGetVector3Macro(Color, float);

  // Description:
  // The whole extent of the output has to be set explicitely.
  vtkSetObjectMacro(LookupTable,vtkScalarsToColors);
  vtkGetObjectMacro(LookupTable,vtkScalarsToColors);

  // Description:
  // The whole extent of the output has to be set explicitely.
  int MapBinToScalar(int bin);
  int MapScalarToBin(int scalar);

  // Description:
  // We need to check the modified time of the lookup table too.
  unsigned long GetMTime();

protected:
  vtkImagePlot();
  ~vtkImagePlot();
  vtkImagePlot(const vtkImagePlot&) {};
  void operator=(const vtkImagePlot&) {};

  vtkScalarsToColors *LookupTable;
  
  float Color[3];

  int Thickness;
  int Height;
  int DataRange[2];
  int DataDomain[2];

  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif
