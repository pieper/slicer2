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
// .NAME vtkImageCloseUp2D -  Creates a magnified 2D image
// .SECTION Description
// vtkImageCloseUp2D shows a magnified square portion of a 2D image.

#ifndef __vtkImageCloseUp2D_h
#define __vtkImageCloseUp2D_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageCloseUp2D : public vtkImageToImageFilter
{
public:
	static vtkImageCloseUp2D *New();
  vtkTypeMacro(vtkImageCloseUp2D,vtkImageToImageFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the Center of the window (X,Y) that we zoom in on.
  // Set the Magnification
  // Set the half-width of the region to zoom in on (radius)
  // The half-length is set to the same value.
	vtkSetMacro(X, int);
	vtkSetMacro(Y, int);
	vtkSetMacro(Radius, int);
	vtkSetMacro(Magnification, int);

	int Magnification;
	int Radius;
	int X;
	int Y;

protected:
	vtkImageCloseUp2D();
	~vtkImageCloseUp2D() {};
        vtkImageCloseUp2D(const vtkImageCloseUp2D&) {};
        void operator=(const vtkImageCloseUp2D&) {};

        void ExecuteInformation(vtkImageData *inData, 
                                vtkImageData *outData);

	// Override this function since inExt != outExt
	void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);
  
	void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
		int extent[6], int id);
};

#endif



