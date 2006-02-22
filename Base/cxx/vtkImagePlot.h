/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImagePlot.h,v $
  Date:      $Date: 2006/02/22 23:47:16 $
  Version:   $Revision: 1.22 $

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
  vtkSetVector3Macro(Color, vtkFloatingPointType);
  vtkGetVector3Macro(Color, vtkFloatingPointType);

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
  vtkImagePlot(const vtkImagePlot&);
  void operator=(const vtkImagePlot&);

  vtkScalarsToColors *LookupTable;
  
  vtkFloatingPointType Color[3];

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
