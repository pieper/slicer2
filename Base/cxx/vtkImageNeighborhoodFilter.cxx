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
#include "vtkImageNeighborhoodFilter.h"
#include <time.h>
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageNeighborhoodFilter* vtkImageNeighborhoodFilter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageNeighborhoodFilter");
  if(ret)
    {
      return (vtkImageNeighborhoodFilter*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageNeighborhoodFilter;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageNeighborhoodFilter::vtkImageNeighborhoodFilter()
{
  
  this->Mask = NULL;
  this->HandleBoundaries = 1;
  this->SetNeighborTo4();
}


//----------------------------------------------------------------------------
vtkImageNeighborhoodFilter::~vtkImageNeighborhoodFilter()
{
  
  if (this->Mask != NULL)
    {
      delete [] this->Mask;
    }
  
}


//----------------------------------------------------------------------------
// This method sets the size of the neighborhood and the default middle of the
// neighborhood.  Also sets the size of (allocates) the matching mask.
void vtkImageNeighborhoodFilter::SetKernelSize(int size0, int size1, int size2)
{
  int modified = 0;

  if (this->KernelSize[0] != size0)
    {
    modified = 1;
    this->KernelSize[0] = size0;
    this->KernelMiddle[0] = size0 / 2;
    }
  if (this->KernelSize[1] != size1)
    {
    modified = 1;
    this->KernelSize[1] = size1;
    this->KernelMiddle[1] = size1 / 2;
    }
  if (this->KernelSize[2] != size2)
    {
    modified = 1;
    this->KernelSize[2] = size2;
    this->KernelMiddle[2] = size2 / 2;
    }


  if (modified)
    {
      if (this->Mask != NULL)
    {
      delete [] this->Mask;
    }
      this->Mask = new unsigned char[this->KernelSize[0]*
                    this->KernelSize[1]*this->KernelSize[2]];      
      this->Modified();
    }
  //cout << "kernel middle: " <<KernelMiddle[0]<<" "<<KernelMiddle[1]<<" "<<KernelMiddle[2]<<endl;
}


//----------------------------------------------------------------------------
void vtkImageNeighborhoodFilter::SetNeighborTo4()
{
  this->SetKernelSize(3,3,3);
    
  this->Neighbor = 4;

  // clear
  memset(this->Mask, 0, this->KernelSize[0]*this->KernelSize[1]*
     this->KernelSize[2]);

  // set 4 neighbors in center slice
  int z = 0;
  for (int y=-1; y <= 1; y++)
    for (int x=-1; x <= 1; x++)
      if (x*y == 0)
    Mask[(1+z)*9+(1+y)*3+(1+x)] = 1;

  // unset center (current) pixel
  Mask[1*9+1*3+1] = 0;
  // set center pix in slice before/after (3D 4-connectivity)
  Mask[0*9+1*3+1] = 1;
  Mask[2*9+1*3+1] = 1;

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkImageNeighborhoodFilter::SetNeighborTo8()
{
  this->SetKernelSize(3,3,3);

  this->Neighbor = 8;

  // set
  memset(this->Mask, 1, this->KernelSize[0]*this->KernelSize[1]*
     this->KernelSize[2]);

  // only unset current (center) pixel
  Mask[1*9+1*3+1] = 0;

  this->Modified();
}

//----------------------------------------------------------------------------
// Description:
// increments to loop through mask.
void vtkImageNeighborhoodFilter::GetMaskIncrements(int &maskInc0, 
                           int &maskInc1, 
                           int &maskInc2)
{
  maskInc0 = 1;
  maskInc1 = this->KernelSize[0];
  maskInc2 = this->KernelSize[0]*this->KernelSize[1];
}

//----------------------------------------------------------------------------
// Description:
// This is like the extent of the neighborhood, but relative to the 
// current voxel
void vtkImageNeighborhoodFilter::GetRelativeHoodExtent(int &hoodMin0, 
                               int &hoodMax0, 
                               int &hoodMin1, 
                               int &hoodMax1, 
                               int &hoodMin2, 
                               int &hoodMax2)
{
  // Neighborhood around current pixel (kernel has radius 1)
  hoodMin0 = - this->KernelMiddle[0];
  hoodMin1 = - this->KernelMiddle[1];
  hoodMin2 = - this->KernelMiddle[2];

  hoodMax0 = hoodMin0 + this->KernelSize[0] - 1;
  hoodMax1 = hoodMin1 + this->KernelSize[1] - 1;
  hoodMax2 = hoodMin2 + this->KernelSize[2] - 1;

//    cout << "mins: " << hoodMin0 << " " << hoodMin1 
//         << " " << hoodMin2 << endl;
//    cout << "max: " << hoodMax0 << " " << hoodMax1 
//         << " " << hoodMax2 << endl;
}




