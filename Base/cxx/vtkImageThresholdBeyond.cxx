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
#include "vtkImageThresholdBeyond.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageThresholdBeyond* vtkImageThresholdBeyond::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageThresholdBeyond");
  if(ret)
    {
    return (vtkImageThresholdBeyond*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageThresholdBeyond;
}


//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageThresholdBeyond::vtkImageThresholdBeyond()
{
  this->Algo = VTK_THRESHOLD_BYLOWER;
}


//----------------------------------------------------------------------------
// The values greater than or equal to the value match.
void vtkImageThresholdBeyond::ThresholdByUpper(float thresh)
{
  if (this->LowerThreshold != thresh)
    {
    this->LowerThreshold = thresh;
    this->Algo = VTK_THRESHOLD_BYUPPER;
    this->Modified();
    }
}


//----------------------------------------------------------------------------
// The values less than or equal to the value match.
void vtkImageThresholdBeyond::ThresholdByLower(float thresh)
{
  if (this->UpperThreshold != thresh)
    {
    this->UpperThreshold = thresh;
    this->Algo = VTK_THRESHOLD_BYLOWER;
    this->Modified();
    }
}


//----------------------------------------------------------------------------
// The values in a range (inclusive) match
void vtkImageThresholdBeyond::ThresholdBetween(float lower, float upper)
{
  if (this->LowerThreshold != lower || this->UpperThreshold != upper)
    {
    this->LowerThreshold = lower;
    this->UpperThreshold = upper;
    this->Algo = VTK_THRESHOLD_BETWEEN;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// The values beyond a range (exclusive) match
void vtkImageThresholdBeyond::ThresholdBeyond(float lower, float upper)
{
  if (this->LowerThreshold != lower || this->UpperThreshold != upper)
    {
    this->LowerThreshold = lower;
    this->UpperThreshold = upper;
    this->Algo = VTK_THRESHOLD_BEYOND;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class IT, class OT>
static void vtkImageThresholdBeyondExecute(vtkImageThresholdBeyond *self,
				     vtkImageData *inData, IT *inPtr,
				     vtkImageData *outData, OT *outPtr, 
				     int outExt[6], int id)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  int replaceIn = self->GetReplaceIn();
  OT  inValue = (OT)(self->GetInValue());
  int replaceOut = self->GetReplaceOut();
  OT  outValue = (OT)(self->GetOutValue());
  int algo = self->GetAlgo();
  unsigned long count = 0;
  unsigned long target;
  
  // DAVE do this safer someday 
  IT lowerThreshold = (IT)self->GetLowerThreshold();
  IT upperThreshold = (IT)self->GetUpperThreshold();
  IT temp;
  
  // find the region to loop over
  rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through data 
  inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // Loop through ouput pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++) 
  {
    for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++) 
    {
      // Update progress
      if (!id) 
      {
        if (!(count%target))
        {
          self->UpdateProgress(count/(50.0*target));
        }
        count++;
      }
      // Process rows according to algorithm
      switch (algo)
      {

      // The values greater than or equal to lowerThreshold match.
      case VTK_THRESHOLD_BYUPPER:
        if (replaceIn && replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceIn)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = (OT)(temp);
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp)
            {
              *outPtr = (OT)(temp);
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        // Replace nothing (straight copy)
        else
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            *outPtr = (OT)(*inPtr);
            outPtr++;			
            inPtr++;
          }
        }
        break;

      // The values less than or equal to the upperThreshold match.
      case VTK_THRESHOLD_BYLOWER:
        if (replaceIn && replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp <= upperThreshold)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceIn)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp <= upperThreshold)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = (OT)(temp);
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp <= upperThreshold)
            {
              *outPtr = (OT)(temp);
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        // Replace nothing (straight copy)
        else
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            *outPtr = (OT)(*inPtr);
            outPtr++;			
            inPtr++;
          }
        }
        break;

      // The values beyond a range (exclusive) match
      case VTK_THRESHOLD_BEYOND:
        if (replaceIn && replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp < lowerThreshold || upperThreshold < temp)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceIn)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp < lowerThreshold || upperThreshold < temp)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = (OT)(temp);
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (temp < lowerThreshold || upperThreshold < temp)
            {
              *outPtr = (OT)(temp);
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        // Replace nothing (straight copy)
        else
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            *outPtr = (OT)(*inPtr);
            outPtr++;			
            inPtr++;
          }
        }
        break;

      // The values in a range (inclusive) match
      case VTK_THRESHOLD_BETWEEN:
        if (replaceIn && replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp && temp <= upperThreshold)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceIn)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp && temp <= upperThreshold)
            {
              *outPtr = inValue;
            }
            else
            {
              *outPtr = (OT)(temp);
            }
            outPtr++;			
            inPtr++;
          }
        }
        else if (replaceOut)
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            temp = *inPtr;
            if (lowerThreshold <= temp && temp <= upperThreshold)
            {
              *outPtr = (OT)(temp);
            }
            else
            {
              *outPtr = outValue;
            }
            outPtr++;			
            inPtr++;
          }
        }
        // Replace nothing (straight copy)
        else
        {
          for (idxR = 0; idxR < rowLength; idxR++) 
          {
            *outPtr = (OT)(*inPtr);
            outPtr++;			
            inPtr++;
          }
        }
        break;
      }//switch

      outPtr += outIncY;
      inPtr += inIncY;
		}
    outPtr += outIncZ;
    inPtr += inIncZ;
	}
}

//----------------------------------------------------------------------------
template <class T>
static void vtkImageThresholdBeyondExecute1(vtkImageThresholdBeyond *self,
				      vtkImageData *inData, T *inPtr,
				      vtkImageData *outData,
				      int outExt[6], int id)
{
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
  switch (outData->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageThresholdBeyondExecute(self, inData, inPtr,
			       outData, (double *)(outPtr),outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageThresholdBeyondExecute(self, inData, inPtr,
			       outData, (float *)(outPtr),outExt, id);
      break;
    case VTK_LONG:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (long *)(outPtr),outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (unsigned long *)(outPtr),outExt, id);
      break;
    case VTK_INT:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (int *)(outPtr),outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (unsigned int *)(outPtr),outExt, id);
      break;
    case VTK_SHORT:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (short *)(outPtr),outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (unsigned short *)(outPtr),outExt, id);
      break;
    case VTK_CHAR:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (char *)(outPtr),outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageThresholdBeyondExecute(self, inData, inPtr, 
			       outData, (unsigned char *)(outPtr),outExt, id);
      break;
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
    }
}



//----------------------------------------------------------------------------
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageThresholdBeyond::ThreadedExecute(vtkImageData *inData, 
					vtkImageData *outData,
					int outExt[6], int id)
{
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  
  switch (inData->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageThresholdBeyondExecute1(this, inData, (double *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageThresholdBeyondExecute1(this, inData, (float *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_LONG:
      vtkImageThresholdBeyondExecute1(this, inData, (long *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageThresholdBeyondExecute1(this, inData, (unsigned long *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_INT:
      vtkImageThresholdBeyondExecute1(this, inData, (int *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageThresholdBeyondExecute1(this, inData, (unsigned int *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_SHORT:
      vtkImageThresholdBeyondExecute1(this, inData, (short *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageThresholdBeyondExecute1(this, inData, (unsigned short *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_CHAR:
      vtkImageThresholdBeyondExecute1(this, inData, (char *)(inPtr), 
			       outData, outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageThresholdBeyondExecute1(this, inData, (unsigned char *)(inPtr), 
			       outData, outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}


void vtkImageThresholdBeyond::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageThreshold::PrintSelf(os,indent);

  os << indent << "Algo: " << this->Algo << "\n";
}

