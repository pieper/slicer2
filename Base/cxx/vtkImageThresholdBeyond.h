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



