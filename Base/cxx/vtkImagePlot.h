/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
// .NAME vtkImagePlot - duh
// .SECTION Description
// This filter displays a plot of the input data.  It expects input only along
// the x-axis and will create basically a bar graph of it. 
// Used to make the histogram in the slicer.

#ifndef __vtkImagePlot_h
#define __vtkImagePlot_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkScalarsToColors.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImagePlot : public vtkImageToImageFilter
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
  void ExecuteInformation(){this->vtkImageToImageFilter::ExecuteInformation();};
  void ExecuteData(vtkDataObject *);
  // void vtkImagePlotExecute(vtkImageData *inData,  unsigned char *inPtr,  int inExt[6], vtkImageData *outData, unsigned char *outPtr, int outExt[6]);
};

#endif
