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

#include "vtkImageTrulyInPlaceFilter.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageTrulyInPlaceFilter* vtkImageTrulyInPlaceFilter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageTrulyInPlaceFilter");
  if(ret)
  {
    return (vtkImageTrulyInPlaceFilter*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageTrulyInPlaceFilter;
}
  
//----------------------------------------------------------------------------
// This Update method looks out of date with the latest pipeline architecture
// (see vtkSource::Update), but it works!
void vtkImageTrulyInPlaceFilter::InternalUpdate(vtkDataObject *data)
{
  vtkImageData *outData = (vtkImageData *)data;

  // Make sure the Input has been set.
  if ( ! this->GetInput())
  {
    vtkErrorMacro(<< "Input is not set.");
    return;
  }

  // prevent infinite update loops.
  if (this->Updating)
  {
    return;
  }
  this->Updating = 1;
  this->AbortExecute = 0;

  vtkImageData *inData;
  this->GetInput()->Update();
  inData = this->GetInput();
  if (inData != outData)
  {
    vtkErrorMacro(<< "InData differs from outData.");
    return;
  }

  // The StartMethod call is placed here to be after updating the input.
  if ( this->StartMethod )
  {
    (*this->StartMethod)(this->StartMethodArg);
  }

  // fill the output region 
  this->Execute(inData, outData);

  if ( this->EndMethod )
  {
    (*this->EndMethod)(this->EndMethodArg);
  }
  this->Updating = 0;
}

