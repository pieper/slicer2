/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
  vtkFloatingPointType origin[3], dimension[3];
  double  voxelVolume, volume;
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
  voxelVolume = dimension[0]*dimension[1]*dimension[2]/1000.0;
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
        file << vol2 << "\n" ;
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
void vtkImageMeasureVoxels::ExecuteData(vtkDataObject *)
{

  int outExt[6];

  vtkImageData *inData = this->GetInput(); 
  vtkImageData *outData = this->GetOutput();
    outData->GetWholeExtent(outExt);
    outData->SetExtent(outExt);
    outData->AllocateScalars();

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

