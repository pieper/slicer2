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
#include "vtkImageMeasureVoxels.h"
#include "vtkImageAccumulateDiscrete.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"

//#include <iomanip.h>

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
  int IncX, IncY, IncZ;
  int rowLength;
  int extent[6];
  int label;
  unsigned long count = 0;
  unsigned long target;
  float origin[3], dimension[3], voxelVolume, volume;
  ofstream file;
  char *filename;

  // set outData same as input (we only care about the histogram)
  outData->CopyAndCastFrom(inData, inData->GetWholeExtent());

  // compute the histogram.
  vtkImageAccumulateDiscrete *accum = vtkImageAccumulateDiscrete::New();
  accum->SetInput(inData);
  accum->Update();
  vtkImageData *histogram;
  histogram = accum->GetOutput();

  // Open file for writing volume measurements
  filename = self->GetFileName();
  if (filename==NULL)
    {
      printf("Execute: Set the filename first");
      return;
    }
  file.open(filename);
  if (file.fail())
    {
    printf("Execute: Could not open file %s", filename);
    return;
    }  

  // find the region to loop over
  histogram->GetExtent(extent);
  rowLength = (extent[1] - extent[0]+1)*inData->GetNumberOfScalarComponents();
  maxY = extent[3] - extent[2]; 
  maxZ = extent[5] - extent[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);
  target++;
  
  // Get increments to march through data 
  histogram->GetContinuousIncrements(extent, IncX, IncY, IncZ);
  int* histPtr = (int *)histogram->GetScalarPointerForExtent(extent);

  // amount input histogram is shifted by
  histogram->GetOrigin(origin);
  //printf("origin: %f %f %f\n", origin[0], origin[1], origin[2]);

  // Get voxel dimensions (in mm)
  inData->GetSpacing(dimension);
  // Convert to voxel volume (in mL)
  voxelVolume = dimension[0]*dimension[1]*dimension[2]/1000;
  // printf("dimensions: %f %f %f, vol: %f \n", dimension[0], dimension[1], dimension[2], voxelVolume);

  // Loop through histogram pixels (really histogram is 1D, only X matters.)
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
        if (*histPtr > 0)
          {
        label = idxR+(int)origin[0];
        volume = *histPtr * voxelVolume;
        // round to 3 decimal places
        char vol2[30];
        sprintf(vol2, "%.3f", volume);
        
        // >> Modified by Attila Tanacs 7/27/01
        // Instead of manipulators, member functions are used.
        //file << setw(5) << label;
        file.width(5);
        file << label;
        //file << setiosflags(ios::right);
        file.setf(ios::right);
        //file.setiosflags(ios::right);
        ////int width = 15 - (label>=10) - (label>=100);
        //file << setw(15)  << vol2 << '\n';
        file.width(15);
        file << vol2 << "\n";
        // << End of modifications 7/27/01
          }
        histPtr++;
      }
    histPtr += IncY;
      }
    histPtr += IncZ;
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

  inPtr  = inData->GetScalarPointer();
  outPtr = (int *)outData->GetScalarPointer();
  
  switch (inData->GetScalarType())
  {
    case VTK_CHAR:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (char *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (unsigned char *)(inPtr), outData, outPtr);
      break;
    case VTK_SHORT:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (short *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (unsigned short *)(inPtr), outData, outPtr);
      break;
    case VTK_INT:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (int *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (unsigned int *)(inPtr), outData, outPtr);
      break;
    case VTK_LONG:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (long *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (unsigned long *)(inPtr), outData, outPtr);
      break;
    case VTK_FLOAT:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (float *)(inPtr), outData, outPtr);
      break;
    case VTK_DOUBLE:
      vtkImageMeasureVoxelsExecute(this, 
              inData, (double *)(inPtr), outData, outPtr);
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

