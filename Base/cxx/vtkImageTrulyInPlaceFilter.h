/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageTrulyInPlaceFilter - Filter that operates in place.
// .SECTION Description
// vtkImageTrulyInPlaceFilter is a filter super class that 
// operates directly on the input region.  The data is copied
// if the requested region has different extent than the input region
// or some other object is referencing the input region.  

// .SECTION See Also
// vtkImageToImageFilter vtImageMultipleInputFilter vtkImageTwoInputFilter
// vtkImageTwoOutputFilter


#ifndef __vtkImageTrulyInPlaceFilter_h
#define __vtkImageTrulyInPlaceFilter_h

#include "vtkImageInPlaceFilter.h"

class VTK_EXPORT vtkImageTrulyInPlaceFilter : public vtkImageInPlaceFilter
{
public:
  static vtkImageTrulyInPlaceFilter *New();
  const char *GetClassName() {return "vtkImageTrulyInPlaceFilter";};

  // Description:
  // This method is called by the cache.  It eventually calls the
  // Execute(vtkImageData *, vtkImageData *) method.  Information has
  // already been updated by this point, and outRegion is in local
  // coordinates.  This method will stream to get the input. Only the
  // UpdateExtent from output will get updated.
  virtual void InternalUpdate(vtkDataObject *data);

protected:
  vtkImageTrulyInPlaceFilter() {};
  ~vtkImageTrulyInPlaceFilter() {};
  vtkImageTrulyInPlaceFilter(const vtkImageTrulyInPlaceFilter&) {};
  void operator=(const vtkImageTrulyInPlaceFilter&) {};
};

#endif







