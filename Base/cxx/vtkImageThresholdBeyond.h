/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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


#ifndef __vtkImageThresholdBeyond_h
#define __vtkImageThresholdBeyond_h

#define VTK_THRESHOLD_BYLOWER 0
#define VTK_THRESHOLD_BYUPPER 1
#define VTK_THRESHOLD_BETWEEN 2
#define VTK_THRESHOLD_BEYOND  3

#include "vtkImageThreshold.h"

class VTK_EXPORT vtkImageThresholdBeyond : public vtkImageThreshold
{
public:
  static vtkImageThresholdBeyond *New();
  vtkTypeMacro(vtkImageThresholdBeyond,vtkImageThreshold);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // The values beyond a range match (lower than "lower", or 
  // higher than "upper", exclusive).
  void ThresholdBeyond(float lower, float upper);

  // Description:
  // change superclass function
  void ThresholdBetween(float lower, float upper);
  void ThresholdByUpper(float thresh);
  void ThresholdByLower(float thresh);
  
  vtkGetMacro(Algo, int);

protected:
  vtkImageThresholdBeyond();
  ~vtkImageThresholdBeyond() {};
  vtkImageThresholdBeyond(const vtkImageThresholdBeyond&) {};
  void operator=(const vtkImageThresholdBeyond&) {};

  int Algo;

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
               int extent[6], int id);
};

#endif



