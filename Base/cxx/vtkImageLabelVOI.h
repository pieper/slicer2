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
// .NAME vtkImageLabelVOI -  Select label volume of interest
// 
// .SECTION Description
//
// vtkImageLabelVOI will remove label values outside of a subvolume.
// This is used for editing of labelmaps.
//

#ifndef __vtkImageLabelVOI_h
#define __vtkImageLabelVOI_h

#include "vtkImageToImageFilter.h"

class VTK_EXPORT vtkImageLabelVOI : public vtkImageToImageFilter
{
public:
    static vtkImageLabelVOI *New();
  vtkTypeMacro(vtkImageLabelVOI,vtkImageToImageFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetMacro(Method, int);
  vtkSetMacro(Method, int);

  void SetCorner1(int c1x_, int c1y_, int c1z_)
  {
    this->c1x = c1x_; this->c1y = c1y_; this->c1z = c1z_;
  }

  void SetCorner2(int c2x_, int c2y_, int c2z_)
  {
    this->c2x = c2x_; this->c2y = c2y_; this->c2z = c2z_;
  }

  void GetCorner1(int c1[3])
  {
    c1[0] = this->c1x;
    c1[1] = this->c1y;
    c1[2] = this->c1z;
  }

  void GetCorner2(int c2[3])
  {
    c2[0] = this->c2x;
    c2[1] = this->c2y;
    c2[2] = this->c2z;
  }

protected:
    vtkImageLabelVOI();
    ~vtkImageLabelVOI() {};
    vtkImageLabelVOI(const vtkImageLabelVOI&) {};
    void operator=(const vtkImageLabelVOI&) {};

  int c1x;
  int c1y;
  int c1z;
  int c2x;
  int c2y;
  int c2z;

  int Method;

    void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
        int extent[6], int id);
};

#endif

