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
// .NAME vtkImageLiveWireEdgeWeights -  Computes edge weights for input
// to vtkImageLiveWire.
// .SECTION Description
// 

#ifndef __vtkImageLiveWireEdgeWeights_h
#define __vtkImageLiveWireEdgeWeights_h

#include "vtkImageNeighborhoodFilter.h"

class VTK_EXPORT vtkImageLiveWireEdgeWeights : public vtkImageNeighborhoodFilter
{
public:
  static vtkImageLiveWireEdgeWeights *New();
  vtkTypeMacro(vtkImageLiveWireEdgeWeights,vtkImageNeighborhoodFilter);

  // Description:
  // Maximum edge weight that this filter will output
  vtkGetMacro(MaxEdgeWeight, int);
  vtkSetMacro(MaxEdgeWeight, int);

protected:
  vtkImageLiveWireEdgeWeights();
  ~vtkImageLiveWireEdgeWeights();

  int MaxEdgeWeight;

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
    int extent[6], int id);
};

#endif

