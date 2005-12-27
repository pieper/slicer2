/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkStreamlineConvolve.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkStreamlineConvolve.h"

#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "math.h"

vtkCxxRevisionMacro(vtkStreamlineConvolve, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkStreamlineConvolve);

// Construct with lower threshold=0, upper threshold=1, and threshold 
// function=upper.
vtkStreamlineConvolve::vtkStreamlineConvolve()
{

  int idx;
  for (idx = 0; idx < 343; idx++)
    {
    this->Kernel[idx] = 0.0;
    }

  // Construct a primary id function kernel that does nothing at all
  double kernel[9];
  for (idx = 0; idx < 9; idx++)
    {
    kernel[idx] = 0.0;
    }
  kernel[4] = 1.0;
  KernelSize[0]=3;
  KernelSize[1]=3;
  KernelSize[2]=1;
  
  this->Streamlines = NULL; 
}


//----------------------------------------------------------------------------
// Set a 3x3 kernel 
void vtkStreamlineConvolve::SetKernel3x3(vtkDoubleArray* kernel)
{  // 9  elements
  // Fill the kernel
  this->SetKernel(kernel, 3, 3, 1);
}


//----------------------------------------------------------------------------
// Set a 5x5 kernel 
void vtkStreamlineConvolve::SetKernel5x5(vtkDoubleArray* kernel)
{  //25 elements
  // Fill the kernel
  this->SetKernel(kernel, 5, 5, 1);
}

//----------------------------------------------------------------------------
// Set a 7x7 kernel 
void vtkStreamlineConvolve::SetKernel7x7(vtkDoubleArray* kernel)
{  // 49 elements
  // Fill the kernel
  this->SetKernel(kernel, 7, 7, 1);
}

//----------------------------------------------------------------------------
// Set a 3x3x3 kernel
void vtkStreamlineConvolve::SetKernel3x3x3(vtkDoubleArray *kernel)
{  // 27 elements
  // Fill the kernel
  this->SetKernel(kernel, 3, 3, 3);
}

//----------------------------------------------------------------------------
// Set a 5x5x5 kernel
void vtkStreamlineConvolve::SetKernel5x5x5(vtkDoubleArray *kernel)
{
  // Fill the kernel
  this->SetKernel(kernel, 5, 5, 5);
}

//----------------------------------------------------------------------------
// Set a 7x7x7 kernel
void vtkStreamlineConvolve::SetKernel7x7x7(vtkDoubleArray *kernel)
{  // 343 elements
  // Fill the kernel
  this->SetKernel(kernel, 7, 7, 7);
}

//----------------------------------------------------------------------------
// Set a kernel, this is an internal method
void vtkStreamlineConvolve::SetKernel(vtkDoubleArray* kernel,
                                 int sizeX, int sizeY, int sizeZ)
{
  int modified=0;

  // Set the correct kernel size
  this->KernelSize[0] = sizeX;
  this->KernelSize[1] = sizeY;
  this->KernelSize[2] = sizeZ;

  int kernelLength = sizeX*sizeY*sizeZ;

  for (int idx = 0; idx < kernelLength; idx++)
    {
    if ( this->Kernel[idx] != kernel->GetValue((vtkIdType) idx) )
      {
      modified = 1;
      this->Kernel[idx] = kernel->GetValue(idx);
      }
    }
  if (modified)
    {
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// Get the kernel, this is an internal method
void vtkStreamlineConvolve::GetKernel(vtkDoubleArray *kernel)
{
  int kernelLength = this->KernelSize[0]*
    this->KernelSize[1]*this->KernelSize[2];
  if(kernel) {
    kernel = vtkDoubleArray::New();
   }
       
  kernel->SetNumberOfValues(kernelLength);
  
  for (int idx = 0; idx < kernelLength; idx++)
    {
    kernel->SetValue((vtkIdType) idx,this->Kernel[idx]);
    }
}

//----------------------------------------------------------------------------
// Get the kernel, this is an internal method
double* vtkStreamlineConvolve::GetKernel()
{
  return this->Kernel;
}
  
template <class T>
void vtkStreamlineConvolveExecute(vtkStreamlineConvolve *self, 
                                  vtkImageData *input, T *inPtr)
{                  

  int *kernelSize;
  int kernelMiddle[3];

  vtkPoints *newPoints;
  vtkPointData *pd, *outPD;
  vtkCellArray *verts;
  vtkIdType ptId, numPts, pts[1];
  double x[3];
  vtkPolyData *output = self->GetOutput();
  vtkPolyData *streamlines = self->GetStreamlines();

  // For looping though output (and input) pixels.
  int inInc0, inInc1, inInc2;
  T *inPtr0, *inPtr1, *inPtr2;

  // For looping through hood pixels
  int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
  int hoodIdx0, hoodIdx1, hoodIdx2;
  T *hoodPtr0, *hoodPtr1, *hoodPtr2;

  // For looping through the kernel, and compute the kernel result
  int kernelIdx;
  double sum;

  // The extent of the whole input image
  int inImageMin0, inImageMin1, inImageMin2;
  int inImageMax0, inImageMax1, inImageMax2;

  // Points of the streamline
  double globalpt[3],pcoord[3];
  int roundpt[3];

  //
  double *origin = input->GetOrigin();
  double *spacing = input->GetSpacing();

  // to compute the range
  unsigned long count = 0;
  unsigned long target;

     
  numPts = streamlines->GetNumberOfPoints();
  cout<<"Number of points in Streamlines: "<<numPts<<endl;
  
  output->SetPoints(streamlines->GetPoints());
  output->SetLines(streamlines->GetLines());
  
  vtkDoubleArray* outScalars = vtkDoubleArray::New();
  outScalars->SetNumberOfValues(numPts);
  
  /*
  newPoints = vtkPoints::New();
  newPoints->Allocate(numPts);
  pd = streamline->GetPointData();
  outPD = output->GetPointData();
  outPD->CopyAllocate(pd);
  verts = vtkCellArray::New();
  verts->Allocate(verts->EstimateSize(numPts,1));
  */

  // Get information to march through data
  input->GetIncrements(inInc0, inInc1, inInc2); 
  self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
                                   inImageMax1, inImageMin2, inImageMax2);
  
  // Get ivars of this object (easier than making friends)
  kernelSize = self->GetKernelSize();

  kernelMiddle[0] = kernelSize[0] / 2;
  kernelMiddle[1] = kernelSize[1] / 2;
  kernelMiddle[2] = kernelSize[2] / 2;

  hoodMin0 = -kernelMiddle[0];
  hoodMin1 = -kernelMiddle[1];
  hoodMin2 = -kernelMiddle[2];

  hoodMax0 = hoodMin0 + kernelSize[0] - 1;
  hoodMax1 = hoodMin1 + kernelSize[1] - 1;
  hoodMax2 = hoodMin2 + kernelSize[2] - 1;                   
 
  // Get the kernel, just use GetKernel7x7x7(kernel) if the kernel is smaller
  // it still works :)
  double *kernel;
  kernel=self->GetKernel();


  // Check that the scalars of each point satisfy the threshold criterion
  int abort=0;
  vtkIdType progressInterval = numPts/20+1;
  
  cout<<"Starting convolution throught streamline points"<<endl;
  for (int ptId=0; ptId < numPts && !abort; ptId++)
    {
    if ( !(ptId % progressInterval) )
      {
      self->UpdateProgress((double)ptId/numPts);
      abort = self->GetAbortExecute();
      }
    //Streamline point in global coordinate system (scale IJK)
    streamlines->GetPoint(ptId,globalpt);
    
    // Point in IJK
    //roundpt[0]=static_cast<int> (floor((globalpt[0]-origin[0])/spacing[0]));
    //roundpt[1]=static_cast<int> (floor((globalpt[1]-origin[1])/spacing[1]));
    //roundpt[2]=static_cast<int> (floor((globalpt[2]-origin[2])/spacing[2]));
    if (0 == input->ComputeStructuredCoordinates(globalpt,roundpt,pcoord)) {
        outScalars->SetValue(ptId,0.0);
        break;
    }        
    inPtr = (T *) input->GetScalarPointer(roundpt);
    
    // loop through neighborhood pixels
    // as sort of a hack to handle boundaries, 
    // input pointer will be marching through data that does not exist.
    hoodPtr2 = inPtr - kernelMiddle[0] * inInc0 
                            - kernelMiddle[1] * inInc1 
                            - kernelMiddle[2] * inInc2;
          // Set the kernel index to the starting position
          kernelIdx = 0;
      sum = 0.0;

          for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
            {
            hoodPtr1 = hoodPtr2;

            for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
              {
              hoodPtr0 = hoodPtr1;

              for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
                {
                // A quick but rather expensive way to handle boundaries
                // This assumes the boundary values are zero
                if (roundpt[0] + hoodIdx0 >= inImageMin0 &&
                    roundpt[0] + hoodIdx0 <= inImageMax0 &&
                    roundpt[1] + hoodIdx1 >= inImageMin1 &&
                    roundpt[1] + hoodIdx1 <= inImageMax1 &&
                    roundpt[2] + hoodIdx2 >= inImageMin2 &&
                    roundpt[2] + hoodIdx2 <= inImageMax2)
                  {
                  sum += *hoodPtr0 * kernel[kernelIdx];

                  // Take the next postion in the kernel
                  kernelIdx++;
                  }

                hoodPtr0 += inInc0;
                }

              hoodPtr1 += inInc1;
              }

            hoodPtr2 += inInc2;
            }
        
      //Set the output value
      
      outScalars->SetValue(ptId,sum);
          
  }    
        

  // Update ourselves and release memory
  //
  output->GetPointData()->SetScalars(outScalars);
  outScalars->Delete();

  output->Squeeze();
}

void vtkStreamlineConvolve::Execute()
{

  vtkImageData *input = this->GetInput();
  void *inPtr = input->GetScalarPointer();
  
  if ( ! (input->GetPointData()->GetScalars()) )
    {
    vtkErrorMacro(<<"No scalar data to convolve");
    return;
    }

  if (! (this->GetKernel()) )
    {
    vtkErrorMacro(<<"No kernel to convolve with");
    return;
    }
    
  switch (input->GetScalarType())
    {
    vtkTemplateMacro3(vtkStreamlineConvolveExecute, this, input, 
                      (VTK_TT *)(inPtr));

    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }  

}


void vtkStreamlineConvolve::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  this->Superclass::PrintSelf(os, indent);
  
  os << indent << "KernelSize: (" <<
    this->KernelSize[0] << ", " <<
    this->KernelSize[1] << ", " <<
    this->KernelSize[2] << ")\n";

  os << indent << "Kernel: (";
  for (int k = 0; k < this->KernelSize[2]; k++)
    {
    for (int j = 0; j < this->KernelSize[1]; j++)
      {
      for (int i = 0; i < this->KernelSize[0]; i++)
        {
        os << this->Kernel[this->KernelSize[1]*this->KernelSize[0]*k +
                           this->KernelSize[0]*j +
                           i];
        
        if (i != this->KernelSize[0] - 1)
          {
          os << ", ";
          }
        }
      if (j != this->KernelSize[1] - 1 || k != this->KernelSize[2] - 1)
        {
        os << ",\n" << indent << "         ";
        }
      }
    }
  os << ")\n";
}
