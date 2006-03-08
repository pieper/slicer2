/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageThermalMap.cxx,v $
  Date:      $Date: 2006/03/08 15:15:12 $
  Version:   $Revision: 1.1.2.1 $

=========================================================================auto=*/
#include "vtkImageThermalMap.h"
#include "vtkObjectFactory.h"
#include <math.h>


//------------------------------------------------------------------------------
vtkImageThermalMap* vtkImageThermalMap::New()
{
    // First try to create the object from the vtkObjectFactory
    vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageThermalMap");
    if(ret)
    {
        return (vtkImageThermalMap*)ret;
    }
    // If the factory was unable to create the object, then create it here.
    return new vtkImageThermalMap;
}


//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageThermalMap::vtkImageThermalMap()
{
    // all inputs 
    this->NumberOfRequiredInputs = 4;
    this->NumberOfInputs = 0;
}


//----------------------------------------------------------------------------
vtkImageThermalMap::~vtkImageThermalMap()
{
}


//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageThermalMapExecute(
        vtkImageThermalMap *self,
        vtkImageData **inDatas, 
        T **inPtrs,
        vtkImageData *outData,
        int outExt[6], 
        int id)
{
    // For looping though output (and input) pixels.
    int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
    int outIdx0, outIdx1, outIdx2;
    int inInc0, inInc1, inInc2;
    int outInc0, outInc1, outInc2;
    T **inPtrs0, **inPtrs1, **inPtrs2; 
    float *outPtr0, *outPtr1, *outPtr2;

    // The extent of the whole input image
    int inImageMin0, inImageMin1, inImageMin2;
    int inImageMax0, inImageMax1, inImageMax2;
    float *outPtr = (float*) outData->GetScalarPointerForExtent(outExt);

    unsigned long count = 0;
    unsigned long target;
    int numberOfInputs;

    // Get information to march through data
    // all indatas are the same type, so use the same increments
    inDatas[0]->GetIncrements(inInc0, inInc1, inInc2); 

    // march through all inputs using array of input pointers.
    numberOfInputs = self->GetNumberOfInputs();
    inPtrs0 = new T* [numberOfInputs];
    inPtrs1 = new T* [numberOfInputs];
    inPtrs2 = new T* [numberOfInputs];

    self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
            inImageMax1, inImageMin2, inImageMax2);

    outData->GetIncrements(outInc0, outInc1, outInc2); 
    outMin0 = outExt[0];   outMax0 = outExt[1];
    outMin1 = outExt[2];   outMax1 = outExt[3];
    outMin2 = outExt[4];   outMax2 = outExt[5];

    // in and out should be marching through corresponding pixels.
    target = (unsigned long)((outMax2-outMin2+1)*
            (outMax1-outMin1+1)/50.0);
    target++;


    // loop through pixels of output (and all inputs)
    int i = 0;
    outPtr2 = outPtr;
    for (i = 0; i < numberOfInputs; i++) {
        inPtrs2[i] = inPtrs[i];
    }

    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
        outPtr1 = outPtr2;
        for (i = 0; i < numberOfInputs; i++) {
            inPtrs1[i] = inPtrs2[i];
        }
        for (outIdx1 = outMin1; 
                !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
        {
            if (!id) 
            {
                if (!(count%target))
                {
                    self->UpdateProgress(count/(50.0*target));
                }
                count++;
            }

            outPtr0 = outPtr1;
            for (i = 0; i < numberOfInputs; i++) {
                inPtrs0[i] = inPtrs1[i];
            }
            for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
            {
                // compute thermal volume
                float t  = 0.0;
                float pi = 3.1415926535897931;

                float r1 = (*inPtrs0[0]);
                float i1 = (*inPtrs0[1]);
                float r2 = (*inPtrs0[2]);
                float i2 = (*inPtrs0[3]);
                float xx = r1 * r2 + i1 * i2;
                float yy = i1 * r2 - r1 * i2;

                if (xx != 0) 
                {
                    // atan2 the same as matlab:
                    t = atan2(yy,xx);
                } 
                else 
                {
                    if (yy > 0) 
                    {
                        t = pi / 2;
                    } 
                    else if (yy < 0) 
                    {
                        t = -1 * pi / 2;
                    }
                }

                t = t * -1000.0;
                *outPtr0 = t;

                for (i = 0; i < numberOfInputs; i++) {
                    inPtrs0[i] += inInc0;
                }
                outPtr0 += outInc0;
            }//for0
            for (i = 0; i < numberOfInputs; i++) {
                inPtrs1[i] += inInc1;
            }
            outPtr1 += outInc1;
        }//for1
        for (i = 0; i < numberOfInputs; i++) {
            inPtrs2[i] += inInc2;
        }
        outPtr2 += outInc2;
    }//for2

}


//----------------------------------------------------------------------------
// Description:
// This method is passed input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageThermalMap::ThreadedExecute(
        vtkImageData **inDatas, 
        vtkImageData *outData,
        int outExt[6], 
        int id)
{
    void **inPtrs = new void* [this->NumberOfInputs];

    for (int i = 0; i < this->NumberOfInputs; i++) 
    {
        inPtrs[i] = inDatas[i]->GetScalarPointerForExtent(inDatas[i]->GetExtent());
    }

    switch (inDatas[0]->GetScalarType())
    {
        case VTK_SHORT:
            vtkImageThermalMapExecute(
                this, 
                inDatas, 
                (short **)(inPtrs), 
                outData, 
                outExt, 
                id);
            break;
        default:
            vtkErrorMacro(<< "Execute: Bad input ScalarType, short needed");
            return;
    }

    delete [] inPtrs;
}


//----------------------------------------------------------------------------
// Make sure all the inputs are the same size. Doesn't really change 
// the output. Just performs a sanity check
void vtkImageThermalMap::ExecuteInformation(
        vtkImageData **inputs,
        vtkImageData *output)
{
    int *in1Ext, *in2Ext;

    // we require that all inputs have been set.
    if (this->NumberOfInputs < this->NumberOfRequiredInputs)
    {
        vtkErrorMacro(<< "ExecuteInformation: Expected " 
                      << this->NumberOfRequiredInputs 
                      << " inputs, got only " 
                      << this->NumberOfInputs);
        return;      
    }

    // Check that all extents are the same.
    in1Ext = inputs[0]->GetWholeExtent();
    for (int i = 1; i < this->NumberOfInputs; i++) 
    {
        in2Ext = inputs[i]->GetWholeExtent();

        if (in1Ext[0] != in2Ext[0] || 
            in1Ext[1] != in2Ext[1] || 
            in1Ext[2] != in2Ext[2] || 
            in1Ext[3] != in2Ext[3] || 
            in1Ext[4] != in2Ext[4] || 
            in1Ext[5] != in2Ext[5])
        {
            vtkErrorMacro(<< "ExecuteInformation: Inputs 0 and " 
                          << i 
                          << " are not the same size. " 
                          << in1Ext[0] << " " 
                          << in1Ext[1] << " " 
                          << in1Ext[2] << " " 
                          << in1Ext[3] << " vs: "
                          << in2Ext[0] << " " 
                          << in2Ext[1] << " " 
                          << in2Ext[2] << " " 
                          << in2Ext[3] );
            return;
        }
    }

    // we like floats
    output->SetNumberOfScalarComponents(1);
    output->SetScalarType(VTK_FLOAT);
}


void vtkImageThermalMap::PrintSelf(
        ostream& os, 
        vtkIndent indent)
{
    vtkImageMultipleInputFilter::PrintSelf(os,indent);
}

