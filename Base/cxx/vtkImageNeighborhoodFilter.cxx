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

// Lauren change Neighborhood variable?
//----------------------------------------------------------------------------
void vtkImageNeighborhoodFilter::SetNeighborhoodToLine(int length, 
						       int direction)
{
  int x, y, z;

  //this->SetKernelSize(length, 1, 1);
  // Lauren can incorporate information from 
  // previous, next slice also using larger kernel..
  // also could make 'distance map' 
  // so edge filter should be multiple input!! ??
  this->SetKernelSize(3, 3, 1);
  // Lauren test!
  //this->SetKernelSize(3, 3, 3);

  // Lauren ?
  this->Neighbor = 1;

  // set
  memset(this->Mask, 0, this->KernelSize[0]*this->KernelSize[1]*
	 this->KernelSize[2]*sizeof(unsigned char));
  // unset middle
  //this->Mask[1] = 0;

  // set the neighbor on the line
  // directions: 0 1 2 3 == locations top bottom right left 
  // == dirs right left down up

  // Lauren fix this!
  // 0 1 2
  // 3 4 5
  // 6 7 8

  switch (direction)
    {
    case 0: {this->Mask[5] = 1;} // right
    case 1: {this->Mask[3] = 1;} // left
    case 2: {this->Mask[7] = 1;} // down
    case 3: {this->Mask[1] = 1;} // up
    }


  //cout << "MASK: " << this->KernelSize[0]*this->KernelSize[1]*this->KernelSize[2]*sizeof(unsigned char) <<
  //    " " << sizeof(unsigned char) << " " << this->Mask[0] << " " << this->Mask[1] << endl;
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




