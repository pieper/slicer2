/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for any purpose, 
provided that the above copyright notice and the following three paragraphs 
appear on all copies of this software.  Use of this software constitutes 
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkImageLiveWireTester - Identify and process islands of similar pixels
// .SECTION Description
//  The input data type must be shorts.

#ifndef __vtkImageLiveWireTester_h
#define __vtkImageLiveWireTester_h

#include "vtkImageToImageFilter.h"
#include "vtkImageLiveWire.h"
#include "vtkImageLiveWireEdgeWeights.h"

class VTK_EXPORT vtkImageLiveWireTester : public vtkImageToImageFilter
{
public:
  static vtkImageLiveWireTester *New();
  vtkTypeMacro(vtkImageLiveWireTester,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Starting point of shortest path (mouse click #1)
  vtkSetVector2Macro(StartPoint, int);
  vtkGetVector2Macro(StartPoint, int);

  // Description:
  // Ending point of shortest path (mouse click #2)
  vtkSetVector2Macro(EndPoint, int);
  vtkGetVector2Macro(EndPoint, int);

  // Description:
  // LiveWire object this class makes edges for
  vtkSetObjectMacro(LiveWire, vtkImageLiveWire);
  vtkGetObjectMacro(LiveWire, vtkImageLiveWire);

  // Description:
  // Number of filters for edges (number of directions of edges)
  vtkGetMacro(NumberOfEdgeFilters, int);

  // Description:
  // Array of edge filters.
  vtkImageLiveWireEdgeWeights **GetEdgeFilters(){return this->EdgeFilters;};
  vtkImageLiveWireEdgeWeights *GetEdgeFilter(int i){return this->EdgeFilters[i];};

  // Description:
  // Array of edge filters.
  vtkImageData *GetEdgeImage(int filter);

  // Description:
  // User input to filters that create edges, to control importance 
  // of different factors in calculation of edges.
  // val must be >= 0.
  void SetDifference(float val);
  void SetInsidePixel(float val);
  void SetOutsidePixel(float val);

protected:
  vtkImageLiveWireTester();
  ~vtkImageLiveWireTester();
  vtkImageLiveWireTester(const vtkImageLiveWireTester&) {};
  void operator=(const vtkImageLiveWireTester&) {};
  
  int StartPoint[2];
  int EndPoint[2];

  // Description:
  // object we are creating input (multiple edges) for.
  // this filter's output should also be input to it
  vtkImageLiveWire *LiveWire;

  // Description:
  // Array of filters to find edges in all directions
  vtkImageLiveWireEdgeWeights **EdgeFilters;
  int NumberOfEdgeFilters;

  void Execute(vtkImageData *inData, vtkImageData *outData);
};

#endif



