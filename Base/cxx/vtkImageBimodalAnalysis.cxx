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
/*=============================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to gering@ai.mit.edu.
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
=============================================================================*/
#include "vtkImageBimodalAnalysis.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageBimodalAnalysis* vtkImageBimodalAnalysis::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageBimodalAnalysis");
  if(ret)
  {
    return (vtkImageBimodalAnalysis*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageBimodalAnalysis;
}

//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageBimodalAnalysis::vtkImageBimodalAnalysis()
{
  this->SetModalityToCT();
}

//----------------------------------------------------------------------------
void vtkImageBimodalAnalysis::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
					    vtkImageData *output)
{
  output->SetScalarType(VTK_FLOAT);
}

//----------------------------------------------------------------------------
// Get ALL of the input.
void vtkImageBimodalAnalysis::ComputeRequiredInputUpdateExtent(int inExt[6], 
							  int outExt[6])
{
  int *wholeExtent;

  outExt = outExt;
  wholeExtent = this->GetInput()->GetWholeExtent();
  memcpy(inExt, wholeExtent, 6*sizeof(int));
}

//----------------------------------------------------------------------------
void vtkImageBimodalAnalysis::ModifyOutputUpdateExtent()
{
  int wholeExtent[8];
  
  // Filter superclass has no control of intercept cache update.
  // a work around
  if (this->Bypass)
  {
    return;
  }
  this->GetOutput()->GetWholeExtent(wholeExtent);
  this->GetOutput()->SetUpdateExtent(wholeExtent);
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageBimodalAnalysisExecute(vtkImageBimodalAnalysis *self,
				      vtkImageData *inData, T *inPtr,
				      vtkImageData *outData, float *outPtr)
{
  int x, k, offset, clipExt[6];
  int min0, max0, min1, max1, min2, max2;
  int noise = 1, width = 5, hwidth=2;
  float fwidth = 1.0 / 5.0;
  T tmp, minSignal, maxSignal;
  float sum, wsum;
  int ct = (self->GetModality() == VTK_BIMODAL_MODALITY_CT) ? 1 : 0;
  int centroid, noiseCentroid, trough, window, threshold, min, max;
  float origin[3], spacing[3];
 
  // Process x dimension only
  outData->GetExtent(min0, max0, min1, max1, min2, max2);
  inData->GetOrigin(origin);
  inData->GetSpacing(spacing);
  offset = (int)origin[0];

  // Zero output
  memset((void *)outPtr, 0, (max0-min0+1)*sizeof(int));

  // For CT data, ignore -32768
  if (ct)
  {
    min0 = 1;
  }

  // Find min
  min = x = min0;
  while (!inPtr[x] && x <= max0)
  {
    x++;
  }
  if (x <= max0)
  {
    min = x;
  }

  // Find max
  max = x = max0;
  while (!inPtr[x] && x >= min0)
  {
    x--;
  }
  if (x >= min0)
  {
    max = x;
  }

  // Smooth
  for (x = min; x <= max; x++)
  {
    for (k=0; k < width; k++) 
    {
      outPtr[x] += (float)inPtr[x+k];
    }
    outPtr[x] *= fwidth;
  }

  // Find first trough of smoothed histogram
  x = min;
  trough = min-1;
  noise = 1;
  while (x < max && trough < min)
  {
    if (noise)
    {
      if (outPtr[x] > outPtr[x+1])
      {
        if (x > min)
        {
          noise = 0;
        }
      }
    }
    else 
    {
      if (outPtr[x] < outPtr[x+1])
      {
        trough = x;
      }
    }
    x++;
  }

  // Compute centroid of the histogram that PRECEEDS the trough
  // (noise lobe)
  wsum = sum = 0;
  for (x=min; x <= trough; x++)
  {
    tmp = inPtr[x];
    wsum += (float)x*tmp;
    sum  += (float)  tmp;
  }
  if (sum)
  {
	  noiseCentroid = (int)(wsum / sum);
  }
  else
  {
    noiseCentroid = trough;
  }

  // Compute centroid of the histogram that FOLLOWS the trough
  // (signal lobe, and not noise lobe)
  wsum = sum = 0;
  minSignal = maxSignal = inPtr[trough];
  for (x=trough; x <= max; x++)
  {
    tmp = inPtr[x];
    if (tmp > maxSignal)
    {
      maxSignal = tmp;
    }
    else if (tmp < minSignal)
    {
      minSignal = tmp;
    }
    wsum += (float)x*tmp;
    sum  += (float)  tmp;
  }
  if (sum)
  {
	  centroid = (int)(wsum / sum);
  }
  else
  {
    centroid = trough;
  }

  // Threshold
  threshold = trough;
  
  // Compute the window as twice the width as the smaller half
  // of the signal lobe
  if (centroid - noiseCentroid < max - centroid)
  {
    window = (centroid - noiseCentroid)*2;
  }
  else 
  {
    window = (max - centroid)*2;
  }

  // Record findings
  self->SetThreshold(threshold + offset);
  self->SetMin(min + offset);
  self->SetMax(max + offset);
  self->SetLevel(centroid + offset);
  self->SetWindow(window);
  self->SetSignalRange(minSignal, maxSignal);

  outData->GetExtent(clipExt);
  clipExt[0] = min;
  clipExt[1] = max;
  self->SetClipExtent(clipExt);
}

	

//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkImageBimodalAnalysis::Execute(vtkImageData *inData, 
				 vtkImageData *outData)
{
  void *inPtr;
  float *outPtr;
  
  inPtr  = inData->GetScalarPointer();
  outPtr = (float *)outData->GetScalarPointer();
  
  // Components turned into x, y and z
  int c = this->GetInput()->GetNumberOfScalarComponents();
  if (c > 1)
  {
    vtkErrorMacro("This filter requires 1 scalar component, not " << c);
    return;
  }
  
  // this filter expects that output is type float.
  if (outData->GetScalarType() != VTK_FLOAT)
  {
    vtkErrorMacro(<< "Execute: out ScalarType " << outData->GetScalarType()
		  << " must be float\n");
    return;
  }
  
  switch (inData->GetScalarType())
  {
    case VTK_CHAR:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (char *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (unsigned char *)(inPtr), outData, outPtr);
      break;
    case VTK_SHORT:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (short *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (unsigned short *)(inPtr), outData, outPtr);
      break;
    case VTK_INT:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (int *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (unsigned int *)(inPtr), outData, outPtr);
      break;
    case VTK_LONG:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (long *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (unsigned long *)(inPtr), outData, outPtr);
      break;
    case VTK_FLOAT:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (float *)(inPtr), outData, outPtr);
      break;
    case VTK_DOUBLE:
      vtkImageBimodalAnalysisExecute(this, 
			  inData, (double *)(inPtr), outData, outPtr);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unsupported ScalarType");
      return;
    }
}


void vtkImageBimodalAnalysis::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

}

