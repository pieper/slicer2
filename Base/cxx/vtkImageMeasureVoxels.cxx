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
#include "vtkImageMeasureVoxels.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageMeasureVoxels* vtkImageMeasureVoxels::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageMeasureVoxels");
  if(ret)
    {
    return (vtkImageMeasureVoxels*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageMeasureVoxels;
}

//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageMeasureVoxels::vtkImageMeasureVoxels()
{  
  this->FileName = NULL;
}

//----------------------------------------------------------------------------
vtkImageMeasureVoxels::~vtkImageMeasureVoxels()
{
  if (this->FileName != NULL)
    {
      delete [] this->FileName;
    }
}

//----------------------------------------------------------------------------
void vtkImageMeasureVoxels::EnlargeOutputUpdateExtents(vtkDataObject *vtkNotUsed(data) )
{
  int wholeExtent[8];
  
  this->GetOutput()->GetWholeExtent(wholeExtent);
  this->GetOutput()->SetUpdateExtent(wholeExtent);
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageMeasureVoxelsExecute(vtkImageMeasureVoxels *self,
				      vtkImageData *inData, T *inPtr,
				      vtkImageData *outData, int *outPtr)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  unsigned long count = 0;
  unsigned long target;
  int outExt[6];
  float origin[3];
  ofstream file;
  char *filename;

  // Open file
  filename = self->GetFileName();
  if (filename==NULL)
    {
      printf("Execute: Set the filename first");
      return;
    }
  file.open(filename);
  if (file.fail())
    {
    printf("Execute: Could not open file %", filename);
    return;
    }  

  // find the region to loop over
  outData->GetExtent(outExt);
  rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // amount input histogram is shifted by
  inData->GetOrigin(origin);
  printf("origin: %f %f %f\n", origin[0], origin[1], origin[2]);

  // Loop through ouput pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++)
      {
	if (!(count%target))
	  {
	    self->UpdateProgress(count/(50.0*target));
	  }
	count++;
	for (idxR = 0; idxR < rowLength; idxR++)
	  {
	    // Pixel operation	    
	    if (*inPtr > 0)
	      {
		//printf ("%f: %d\n", idxR+origin[0], *inPtr);
		file << idxR+origin[0] << " : " << *inPtr << "\n";
	      }
	    // just copy the image to output.
	    *outPtr = (T)(*inPtr);
	    outPtr++;
	    inPtr++;
	  }
	outPtr += outIncY;
	inPtr += inIncY;
      }
    outPtr += outIncZ;
    inPtr += inIncZ;
    }

  file.close();
}

	

//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkImageMeasureVoxels::Execute(vtkImageData *inData, 
				 vtkImageData *outData)
{
  void *inPtr;
  int *outPtr;

  int extent[6];

  inPtr  = inData->GetScalarPointer();
  outPtr = (int *)outData->GetScalarPointer();

  // this filter expects that input is type int.
  if (inData->GetScalarType() != VTK_INT)
  {
    vtkErrorMacro(<< "Execute: input ScalarType " << outData->GetScalarType()
		  << " must be int\n");
    return;
  }

  // this filter expects that input is along x axis only
  inData->GetExtent(extent);
  if (extent[2] != extent[3] || extent[4] != extent[5])
  {
    vtkErrorMacro(<< "Execute: input must be 1D but extent was " 
    << extent[0] << " " << extent[1] << " " << extent[2] << " " 
    << extent[3] << " " << extent[4] << " " << extent[5] << ".\n ");
    return;
  }
  
  switch (inData->GetScalarType())
  {
    case VTK_INT:
      vtkImageMeasureVoxelsExecute(this, 
			  inData, (int *)(inPtr), outData, outPtr);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unsupported ScalarType");
      return;
    }
}


void vtkImageMeasureVoxels::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "FileName: "
     << this->FileName << "\n";
  
  vtkImageToImageFilter::PrintSelf(os,indent);

}

