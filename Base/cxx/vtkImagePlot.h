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
// .NAME vtkImagePlot - duh
// .SECTION Description
// This filter displays a plot of the input data.  It expects input only along
// the x-axis and will create basically a bar graph of it. 
// Used to make the histogram in the slicer.

#ifndef __vtkImagePlot_h
#define __vtkImagePlot_h

#include "vtkImageToImageFilter.h"
#include "vtkScalarsToColors.h"

class VTK_EXPORT vtkImagePlot : public vtkImageToImageFilter
{
public:
  static vtkImagePlot *New();
  vtkTypeMacro(vtkImagePlot,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // 
  vtkSetMacro(Height, int);
  vtkGetMacro(Height, int);

  // Description:
  // 
  vtkSetMacro(Thickness, int);
  vtkGetMacro(Thickness, int);

  // Description:
  // 
  vtkSetVector2Macro(DataRange, int);
  vtkGetVector2Macro(DataRange, int);

  // Description:
  // 
  vtkSetVector2Macro(DataDomain, int);
  vtkGetVector2Macro(DataDomain, int);

  // Description:
  // Color of the curve to draw. (?)
  vtkSetVector3Macro(Color, float);
  vtkGetVector3Macro(Color, float);

  // Description:
  // 
  vtkSetObjectMacro(LookupTable,vtkScalarsToColors);
  vtkGetObjectMacro(LookupTable,vtkScalarsToColors);

  // Description:
  // 
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
  void vtkImagePlotExecute(
				  vtkImageData *inData,  unsigned char *inPtr,  int inExt[6],
				  vtkImageData *outData, unsigned char *outPtr, int outExt[6]);
};

#endif
