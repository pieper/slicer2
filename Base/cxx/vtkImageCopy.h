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
//.NAME vtkImageCopy copies an image
//.SECTION Description 
// vtkImageCopy takes an image as input and produces a copy of the image
// as output. By setting the clear variable, the output data can be all zeros.
// instead of a copy.

#ifndef __vtkImageCopy_h
#define __vtkImageCopy_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageCopy : public vtkImageToImageFilter
{
public:
  static vtkImageCopy *New();
  const char *GetClassName() {return "vtkImageCopy";};
  void PrintSelf(ostream& os, vtkIndent indent);
  

  // Description:
  // If Clear is set to 1, the output image is all zeros.
  // If Clear is set to 0, the output image is a copy of the input.
  vtkSetMacro(Clear, int);
  vtkGetMacro(Clear, int);
  vtkBooleanMacro(Clear, int);

protected:
  vtkImageCopy();
  ~vtkImageCopy() {};
  vtkImageCopy(const vtkImageCopy&) {};
  void operator=(const vtkImageCopy&) {};

  int Clear;
  
  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		       int extent[6], int id);
};

#endif

