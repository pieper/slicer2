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
#include "vtkImageReplaceRegion.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageReplaceRegion* vtkImageReplaceRegion::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageReplaceRegion");
  if(ret)
  {
    return (vtkImageReplaceRegion*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageReplaceRegion;
}

//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageReplaceRegion::vtkImageReplaceRegion()
{
  this->Region = NULL;
  this->Indices = NULL;
}

//----------------------------------------------------------------------------
vtkImageReplaceRegion::~vtkImageReplaceRegion()
{
  if (this->Region != NULL) 
  {
    this->Region->UnRegister(this);
  }

  if (this->Indices != NULL) 
  {
    this->Indices->UnRegister(this);
  }
}


void vtkImageReplaceRegion::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageInPlaceFilter::PrintSelf(os,indent);

  // vtkSetObjectMacro
  os << indent << "Indices: " << this->Indices << "\n";
  if (this->Indices)
  {
    this->Indices->PrintSelf(os,indent.GetNextIndent());
  }

  os << indent << "Region: " << this->Region << "\n";
  if (this->Region)
  {
    this->Region->PrintSelf(os,indent.GetNextIndent());
  }
}
//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageReplaceRegionExecute(vtkImageReplaceRegion *self,
				     vtkImageData *outData, T *outPtr)
{
  vtkImageData *inData = self->GetRegion();
  int size, *ext = inData->GetExtent();
  vtkIntArray *indices = self->GetIndices();
  int i, idx, maxX, maxY, maxZ;
  T* inPtr;
  
	outPtr = (T *)(outData->GetScalarPointer());
  inPtr = (T *)(inData->GetScalarPointer());

  maxX = ext[1] - ext[0]; 
	maxY = ext[3] - ext[2];
  maxZ = ext[5] - ext[4];
  size = (maxX+1)*(maxY+1)*(maxZ+1);

  // Copy output to region, and then copy input to output
  //
	for (i = 0; i < size; i++)
  {
    idx = indices->GetValue(i);
    if (idx >= 0)
    {
      outPtr[idx] = *inPtr;
    }
    inPtr++;
  }
}  

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageReplaceRegion::Execute(vtkImageData *vtkNotUsed(inData), 
				 vtkImageData *outData)
{
  void *ptr = NULL;
  int s1, s2;

  if (this->Region == NULL)
  {
    vtkErrorMacro("Execute: Region not set");
    return;
  }
  if (this->Indices == NULL)
  {
    vtkErrorMacro("Execute: Indices not set");
    return;
  }
  
  // scalar types must match
  s1 = outData->GetScalarType();
  s2 = this->Region->GetScalarType();
  if (s1 != s2)
  {
    vtkErrorMacro(<<"Input scalar type is "<<s1<<", but region is "<<s2<<"\n");
    return;
  }
  
  switch (outData->GetScalarType())
  {
	  case VTK_CHAR:
		  vtkImageReplaceRegionExecute(this, outData, (char *)ptr);
		  break;
	  case VTK_UNSIGNED_CHAR:
		  vtkImageReplaceRegionExecute(this, outData, (unsigned char *)ptr);
		  break;
	  case VTK_SHORT:
		  vtkImageReplaceRegionExecute(this, outData, (short *)ptr);
		  break;
	  case VTK_UNSIGNED_SHORT:
		  vtkImageReplaceRegionExecute(this, outData, (unsigned short *)ptr);
		  break;
  	case VTK_INT:
		  vtkImageReplaceRegionExecute(this, outData, (int *)ptr);
		  break;
  	case VTK_UNSIGNED_INT:
		  vtkImageReplaceRegionExecute(this, outData, (unsigned int *)ptr);
		  break;
	  case VTK_FLOAT:
		  vtkImageReplaceRegionExecute(this, outData, (float *)ptr);
		  break;
	  case VTK_DOUBLE:
		  vtkImageReplaceRegionExecute(this, outData, (double *)ptr);
		  break;
	  default:
		  vtkErrorMacro(<< "Execute: Unknown input ScalarType");
		  return;
  }
}

