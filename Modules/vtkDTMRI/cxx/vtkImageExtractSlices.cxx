#include "vtkImageExtractSlices.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
vtkImageExtractSlices* vtkImageExtractSlices::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageExtractSlices");
  if(ret)
    {
      return (vtkImageExtractSlices*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageExtractSlices;
}


//----------------------------------------------------------------------------
vtkImageExtractSlices::vtkImageExtractSlices()
{
  // default settings amount to a copy of the data
  this->SliceOffset = 0;
  this->SlicePeriod = 1;   
  this->Mode = 0;
  // only allow 1 thread, this is not threaded
  this->NumberOfThreads = 1;

}

//----------------------------------------------------------------------------
void vtkImageExtractSlices::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
  os << indent << "SliceOffset: "<< this->SliceOffset<<endl;
  os << indent << "SlicePeriod: "<< this->SlicePeriod<<endl;
  if (this->Mode == MODESLICE)
    os << indent << "Mode to Slice "<<endl;
  else
    os << indent << "Mode to Volume "<<endl;

}


//----------------------------------------------------------------------------
void vtkImageExtractSlices::ExecuteInformation(vtkImageData *input, 
                           vtkImageData *output)
{
  int *inExt, outExt[6], totalInSlices;

  if (input == NULL)
    {
      vtkErrorMacro("No input");
      return;
    }

  inExt = input->GetWholeExtent();
  memcpy (outExt, inExt, 6 * sizeof (int));

  // change output extent to reflect the 
  // total number of slices we will output,
  // given the entire input dataset
  if (this->Mode == MODESLICE) {
    totalInSlices = inExt[5] - inExt[4];
    outExt[5] = outExt[4] + (totalInSlices-this->SliceOffset)/this->SlicePeriod;
    vtkDebugMacro("setting out ext to " << outExt[5]);
    output->SetWholeExtent(outExt);
  }
  
  if(this->Mode == MODEVOLUME) {
    totalInSlices = inExt[5] - inExt[4] + 1;
  if(fmod(totalInSlices,this->SlicePeriod)!=0) {
     vtkErrorMacro("We cannot run");
     return;
  }
  outExt[5] = outExt[4] - 1 + totalInSlices/this->SlicePeriod;
  vtkDebugMacro("setting out ext to " << outExt[5]);
  output->SetWholeExtent(outExt); 
  }

}

void vtkImageExtractSlices::ComputeInputUpdateExtent(int inExt[6], 
                             int outExt[6])
{
  int totalOutSlices;

  // set the input to the extent we need to look at 
  // to calculate the requested output.

  // init to the whole extent
  this->GetInput()->GetWholeExtent(inExt);

  if (this->Mode == MODESLICE)
  { 
    // change input extent to just be what is needed to 
    // generate the currently requested output
    // where do we start in the input?
    inExt[4] = (outExt[4]*this->SlicePeriod) + this->SliceOffset;
    // how far do we go?
    totalOutSlices = (outExt[5] - outExt[4] + 1);
    // num periods is out slices - 1
    inExt[5] = inExt[4] + (totalOutSlices-1)*this->SlicePeriod;
  }
  //cout << "in ext is  " << inExt[4] << " " << inExt[5] << endl;
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageExtractSlicesExecute(vtkImageExtractSlices *self,
                     vtkImageData *inData, 
                     T * inPtr, int inExt[6], 
                     vtkImageData *outData, 
                     T * outPtr, int outExt[6])
{
  int idxX, idxY, idxZ;
  int maxX, maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  unsigned long count = 0;
  unsigned long target;
  int slice, extract, period, offset ;
  int sliceSize;
  int numSlices;
  // find the region to loop over: loop over entire input
  // and generate a (possibly) smaller output
  maxX = inExt[1] - inExt[0];
  maxY = inExt[3] - inExt[2]; 
  maxZ = inExt[5] - inExt[4];
  target = (unsigned long)(outData->GetNumberOfScalarComponents()*
               (maxZ+1)*(maxY+1)/50.0);
  target++;

  // Get increments to march through image data 
  inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);

  // information for extracting the slices
  period = self->GetSlicePeriod();
  offset = self->GetSliceOffset();
  
  // size of a whole slice for skipping
  sliceSize = (maxX+1)*(maxY+1);
  numSlices = (maxZ + 1)/period;

  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
      // either extract this slice from the input, or skip it.
      slice = inExt[4] + idxZ;
      if (self->GetMode() == MODESLICE)
        {
        extract = (fmod(slice,period) == offset);
        }
      else
        {
        extract = (int(slice/numSlices) == offset);
        } 
     //cout <<"slice " << slice << " grab " << extract << endl;

      if (extract) 
    {
      // copy desired slices to output
      for (idxY = 0; !self->AbortExecute && idxY <= maxY; idxY++)
        {
              if (!(count%target)) 
                {
                  self->UpdateProgress(count/(50.0*target) 
                                       + (maxZ+1)*(maxY+1));
                }
              count++;

          for (idxX = 0; idxX <= maxX; idxX++)
        {
          // Pixel operation
          *outPtr = *inPtr;
        
          inPtr++;
          outPtr++;
        }
          outPtr += outIncY;
          inPtr += inIncY;
        }
    }
      else {
    // just increment the pointer and skip the slice
    inPtr+=sliceSize;
      }
      outPtr += outIncZ;
      inPtr += inIncZ;
    }
}

//----------------------------------------------------------------------------
// This method is passed a input and output regions, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the regions data types.
void vtkImageExtractSlices::ThreadedExecute(vtkImageData *inData, 
                                    vtkImageData *outData,
                                    int outExt[6], int id)
{
  int inExt[6];

  vtkDebugMacro("in threaded execute");

  inData->GetExtent(inExt);

  void *inPtr = inData->GetScalarPointerForExtent(inExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  // this filter expects 1 scalar component input
  if (inData->GetNumberOfScalarComponents() != 1)
    {
      vtkErrorMacro(<< "Execute: input has " << 
      inData->GetNumberOfScalarComponents() << 
      " instead of 1 scalar component");
      return;
    }
  
  
  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
      vtkErrorMacro(<< "Execute: input ScalarType (" << 
      inData->GetScalarType() << 
      "), must match output ScalarType (" << outData->GetScalarType() 
      << ")");
      return;
    }
  
  // call Execute method 
  switch (inData->GetScalarType())
    {
      vtkTemplateMacro7(vtkImageExtractSlicesExecute, this, 
            inData, (VTK_TT *)(inPtr), inExt,
            outData, (VTK_TT *)(outPtr), outExt);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
  
}









