/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
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



