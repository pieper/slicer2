/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkImageSmooth.cxx,v $
  Language:  C++
  Date:      $Date: 2004/02/16 23:04:21 $
  Version:   $Revision: 1.1 $

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen 
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#if defined(__sun)
#include <ieeefp.h>
#endif

#include "vtkImageSmooth.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#define DEBUG
//#undef DEBUG
//----------------------------------------------------------------------------

//vtkCxxRevisionMacro(vtkImageSmooth, "$Revision: 1.1 $");
//vtkStandardNewMacro(vtkImageSmooth);

//----------------------------------------------------------------------------


vtkImageSmooth* vtkImageSmooth::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageSmooth");
  if(ret)
    {
      return (vtkImageSmooth*)ret;
    }

  // If the factory was unable to create the object, then create it here.
  return new vtkImageSmooth;
}

vtkImageSmooth::vtkImageSmooth()
{
  NumberOfIterations = 5;
}

//----------------------------------------------------------------------------

vtkImageSmooth::~vtkImageSmooth()
{
  
}

//----------------------------------------------------------------------------

void vtkImageSmooth::ExecuteInformation(vtkImageData *inData, 
                                     vtkImageData *outData)
{
  this->vtkImageToImageFilter::ExecuteInformation(inData, outData);

  //  if (this->OutputScalarType != -1)
  // {
  // outData->SetScalarType(this->OutputScalarType);
  // }
}

float vtkImageSmooth::Init()
{
    //NumberOfIterations = 10;
    dt = 0.4;
    return 1.0;
}
//----------------------------------------------------------------------------

// This templated function executes the filter for any type of data.

template <class IT, class OT>
static void vtkImageSmoothExecute(vtkImageSmooth *self,
                               vtkImageData *inData, IT *inPtr,
                               vtkImageData *outData, OT *outPtr,
                               int outExt[6], int id)
{

  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int rowLength;
  float *temp_ptr;
  float *edge_ptr;

  float lx,ly,lxx,lyy,lxy,kapa;
  unsigned long shift_xyz;
  unsigned long xyz;
  unsigned long count = 1;
  unsigned long target;
  int x,y,z;

  shift_xyz =  0;
  xyz = 0;
 
  temp_ptr = NULL;
  edge_ptr = NULL;

  // find the region to loop over

  rowLength = (outExt[1] - outExt[0]+1)*inData->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2]; 
  maxZ = outExt[5] - outExt[4];
  target = (unsigned long)((maxZ+1)*(maxY+1)/50.0);;
  target++;
  if(self->Init() == 0.0) {
      //self->vtkErrorMacro("Missing input");
      return;
  }
  // Get increments to march through data 

  temp_ptr = (float*) calloc(maxZ*(maxY+1)*rowLength,sizeof(float));
  if(temp_ptr == NULL)
      return;

  edge_ptr = (float*) calloc(maxZ*(maxY+1)*rowLength,sizeof(float));
  if(edge_ptr == NULL)
      return;

  kapa = 0.0;

 

  for(int iter = 0;!self->AbortExecute && iter < self->NumberOfIterations;iter++)
  {

        for (idxZ = 0; idxZ < maxZ; idxZ++)
         {
            for (idxY = 0;idxY <= maxY; idxY++)
            {
                
                if (!id)
                {
                    if (!(count%target))
                    {
                    self->UpdateProgress(count/(50.0*target));
                    }
                    count++;
                }

                xyz = idxY*rowLength+idxZ*maxY*rowLength;
                for (idxR = 0; idxR < rowLength; idxR++)
                    {

                    /* at the boundaries we need to do some special things
                       so that we have access to the correct data or else we will crash
                       as we will access data not present */
                        if((idxR == 0) || (idxR == rowLength-1) || (idxY == 0) || (idxY == maxY)) 
                        {
                            if((idxR == 0) && (idxY != 0) && (idxY != maxY))
                            {
                                lx = 0.5 * (inPtr[(idxR+1)+xyz] - inPtr[idxR+xyz]);
                                ly = 0.5 * (inPtr[idxR+xyz+rowLength] - inPtr[idxR +xyz-rowLength]);
                                lxx = 2 * inPtr[idxR+1+xyz] - 2 * inPtr[idxR+xyz];
                                lyy = inPtr[idxR+xyz+rowLength] - 2*inPtr[idxR+xyz] + inPtr[idxR+xyz-rowLength];
                                lxy = lxy = 0.25 * (inPtr[idxR+1+xyz+rowLength] - inPtr[idxR+1+xyz-rowLength] -
                                      inPtr[idxR+xyz+rowLength] + inPtr[idxR+xyz-rowLength]);

                                kapa = (ly*ly*lxx - 2*lx*ly*lxy + lx*lx*lyy); 
                                if(kapa < 0)
                                    kapa = -1 * pow(-kapa,1/3);
                                else
                                    kapa = pow(kapa,1/3); 

                                if(iter == 0)
                                    edge_ptr[idxR+xyz] = 1 + lx*lx + ly*ly;

                                temp_ptr[idxR+xyz] = kapa; 

                                continue;
                            }

                            if(idxR == rowLength-1)
                            {
                                lx = 0.5 * (inPtr[idxR+xyz] - inPtr[idxR-1+xyz]);
                                ly = 0.5 * (inPtr[idxR+xyz+rowLength] - inPtr[idxR +xyz-rowLength]);
                                lxx = 2 * inPtr[idxR-1+xyz] - 2 * inPtr[idxR+xyz];
                                lyy = inPtr[idxR+xyz+rowLength] - 2*inPtr[idxR+xyz] + inPtr[idxR+xyz-rowLength];
                                lxy = lxy = 0.25 * (inPtr[idxR+xyz+rowLength] - inPtr[idxR+xyz-rowLength] -
                                      inPtr[idxR-1+xyz+rowLength] + inPtr[idxR-1+xyz-rowLength]);
                                kapa = (ly*ly*lxx - 2*lx*ly*lxy + lx*lx*lyy); 
                                if(kapa < 0)
                                    kapa = -1 * pow(-kapa,1/3);
                                else
                                    kapa = pow(kapa,1/3); 

                                if(iter == 0)
                                    edge_ptr[idxR+xyz] = 1 + lx*lx + ly*ly;

                                temp_ptr[idxR+xyz] = kapa; 
                                continue;
                            }

                            if(idxY == 0)
                            {
                                lx = 0.5 * (inPtr[(idxR+1)+xyz] - inPtr[idxR-1+xyz]);
                                ly = 0.5 * (inPtr[idxR+xyz+rowLength] - inPtr[idxR+xyz]);
                                lxx = inPtr[(idxR+1)+xyz] - 2 * inPtr[idxR+xyz] + inPtr[idxR-1+xyz];
                                lyy = inPtr[idxR+xyz+rowLength] - inPtr[idxR+xyz];
                                lxy = 0.25 * (inPtr[idxR+1+xyz+rowLength] - inPtr[idxR+1+xyz] -
                                      inPtr[idxR-1+xyz+rowLength] + inPtr[idxR-1+xyz]);
                                kapa = (ly*ly*lxx - 2*lx*ly*lxy + lx*lx*lyy); 
                                if(kapa < 0)
                                    kapa = -1 * pow(-kapa,1/3);
                                else
                                    kapa = pow(kapa,1/3); 

                                if(iter == 0)
                                    edge_ptr[idxR+xyz] = 1 + lx*lx + ly*ly;

                                temp_ptr[idxR+xyz] = kapa; 
                                continue;
                            }


                            if(idxY == maxY)
                             {
                                lx = 0.5 * (inPtr[(idxR+1)+xyz] - inPtr[idxR-1+xyz]);
                                ly = 0.5 * (inPtr[idxR+xyz] - inPtr[idxR+xyz-rowLength]);
                                lxx = inPtr[(idxR+1)+xyz] - 2 * inPtr[idxR+xyz] + inPtr[idxR-1+xyz];
                                lyy = -1*inPtr[idxR+xyz] + inPtr[idxR+xyz-rowLength];
                                lxy = 0.25 * (inPtr[idxR+1+xyz] - inPtr[idxR+1+xyz-rowLength] -
                                      inPtr[idxR-1+xyz] + inPtr[idxR-1+xyz-rowLength]);
                                kapa = (ly*ly*lxx - 2*lx*ly*lxy + lx*lx*lyy); 
                                if(kapa < 0)
                                    kapa = -1 * pow(-kapa,1/3);
                                else
                                    kapa = pow(kapa,1/3); 

                                if(iter == 0)
                                    edge_ptr[idxR+xyz] = 1 + lx*lx + ly*ly;

                                temp_ptr[idxR+xyz] = kapa; 
                                continue;
                            }

                            

                            continue;
                        }

                        lx = 0.5 * (inPtr[(idxR+1)+xyz] - inPtr[idxR-1+xyz]);

                        ly = 0.5 * (inPtr[idxR+xyz+rowLength] - inPtr[idxR+xyz-rowLength]);

                        lxx = inPtr[(idxR+1)+xyz] - 2 * inPtr[idxR+xyz] + inPtr[idxR-1+xyz];

                        
                        lyy = inPtr[idxR+xyz+rowLength] - 2*inPtr[idxR+xyz] + inPtr[idxR+xyz-rowLength];

                        

                        lxy = 0.25 * (inPtr[idxR+1+xyz+rowLength] - inPtr[idxR+1+xyz-rowLength] -
                                      inPtr[idxR-1+xyz+rowLength] + inPtr[idxR-1+xyz-rowLength]);

                        
                        
                        kapa = (ly*ly*lxx - 2*lx*ly*lxy + lx*lx*lyy); 
                        if(kapa < 0)
                            kapa = -1 * pow(-kapa,1/3);
                        else
                            kapa = pow(kapa,1/3); 

                        if(iter == 0)
                           edge_ptr[idxR+xyz] = 1 + lx*lx + ly*ly;

                        temp_ptr[idxR+xyz] = kapa; 
                    
            
                          }
                /*
              #ifdef DEBUG
                fprintf(fp,"Completed %d Rows\n",idxY);
                #endif */

            } //for idxY
            /*
        #ifdef DEBUG
            fprintf(fp,"Completed %d frames\n",idxZ);
            #endif */
         }

   /* update our image from the smoothed data */
        for(z = 0; z < maxZ;z++)
        {
            for(y = 0;y<=maxY;y++)
            {
                shift_xyz = y*rowLength+z*maxY*rowLength;
                for(x=0;x<rowLength;x++)
                    inPtr[x+shift_xyz] = (IT)(inPtr[x+shift_xyz] + 
                         self->dt * (temp_ptr[x+shift_xyz]/edge_ptr[x+shift_xyz]));
            }
        }
            
  } //numberof iterations

  outData->CopyAndCastFrom(inData,outExt);

  if(!temp_ptr)
      free(temp_ptr);

  if(!edge_ptr)
      free(edge_ptr);
  /*
#ifdef DEBUG
  if(!fp)
     fclose(fp);
#endif
  */

return;

}


//----------------------------------------------------------------------------

template <class T>
static void vtkImageSmoothExecute1(vtkImageSmooth *self,
                                vtkImageData *inData, T *inPtr,
                                vtkImageData *outData,
                                int outExt[6], int id)
{
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
    switch (outData->GetScalarType())
    {
      vtkTemplateMacro7(vtkImageSmoothExecute, self, inData, inPtr,
                      outData, (VTK_TT *)(outPtr),outExt, id);
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

void vtkImageSmooth::ThreadedExecute(vtkImageData *inData, 
                                  vtkImageData *outData,
                                  int outExt[6], int id)
{
  int inExt[6];
  float ret = 0.0;
  void *inPtr = inData->GetScalarPointerForExtent(outExt);

  //extend our image for computations
    this->ComputeInputUpdateExtent(inExt,outExt);
    
    switch (inData->GetScalarType())
    {
      vtkTemplateMacro6(vtkImageSmoothExecute1, this, 
                    inData, (VTK_TT *)(inPtr), outData, outExt, id);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    } 
}

//----------------------------------------------------------------------------
void vtkImageSmooth::ComputeInputUpdateExtent(int inExt[6], 
                int outExt[6])
{
  int *wholeExtent;
  int idx, border;

  border = 2;

  // copy
  memcpy((void *)inExt, (void *)outExt, 6 * sizeof(int));
  // Expand filtered axes
  wholeExtent = this->GetInput()->GetWholeExtent();

  for (idx = 0; idx < 3; ++idx)
    {
    inExt[idx*2] -= border;
    if (inExt[idx*2] < wholeExtent[idx*2])
      {
      inExt[idx*2] = wholeExtent[idx*2];
      }

    inExt[idx*2+1] += border;
    if (inExt[idx*2+1] > wholeExtent[idx*2+1])
      {
      inExt[idx*2+1] = wholeExtent[idx*2+1];
      }
    }

}


void vtkImageSmooth::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  // os << indent << "Smooth: " << this->Smooth << "\n";
  // os << indent << "Output Scalar Type: " << this->OutputScalarType << "\n";
}

