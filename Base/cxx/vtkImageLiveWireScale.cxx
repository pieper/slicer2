/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
#include "vtkImageLiveWireScale.h"
#include "vtkImageAccumulateDiscrete.h"
#include <math.h>
#include <stdlib.h>
#include "vtkObjectFactory.h"
//#include <iomanip.h>
#include <time.h>

//------------------------------------------------------------------------------
vtkImageLiveWireScale* vtkImageLiveWireScale::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLiveWireScale");
  if(ret)
    {
    return (vtkImageLiveWireScale*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLiveWireScale;
}

//----------------------------------------------------------------------------
// Constructor sets default values
vtkImageLiveWireScale::vtkImageLiveWireScale()
{  
  this->ScaleFactor = 1;

  this->UseLookupTable = 0;
  this->UseGaussianLookup = 0;
  this->MeanForGaussianModel = 0;
  this->VarianceForGaussianModel = 1;
  this->UseTransformationFunction = 0;
  this->TransformationFunctionNumber = 0;

  this->UseUpperCutoff = 0;
  this->UpperCutoff = 1;
  this->UseLowerCutoff = 0;
  this->LowerCutoff = 0;

  this->LookupTable = vtkFloatArray::New();
  this->MaxPointsInLookupTableBin = 1;
  this->MinimumBin = 0;
  this->MaximumBin = 1;

  // set by user
  this->LookupPoints = NULL;
}

//----------------------------------------------------------------------------
vtkImageLiveWireScale::~vtkImageLiveWireScale()
{

  if (this->LookupTable != NULL) 
    {
      this->LookupTable->Delete();
    }
  if (this->LookupPoints != NULL) 
    {
      this->LookupPoints->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkImageLiveWireScale::EnlargeOutputUpdateExtents(vtkDataObject *vtkNotUsed(data) )
{
  int wholeExtent[8];
  
  this->GetOutput()->GetWholeExtent(wholeExtent);
  this->GetOutput()->SetUpdateExtent(wholeExtent);
}

//----------------------------------------------------------------------------
float vtkImageLiveWireScale::TransformationFunction(float intensity, float max,
                          float min) 
{
  if (this->UseUpperCutoff)
    {
      max = this->UpperCutoff;
      if (min > this->UpperCutoff) 
    {
      vtkErrorMacro("Oops, min value higher than upper cutoff!");
      min = this->UpperCutoff - 1;
    }
      if (intensity > this->UpperCutoff)
    intensity = this->UpperCutoff;
    }

  if (this->UseLowerCutoff)
    {
      min = this->LowerCutoff;
      if (max < this->LowerCutoff) 
    {
      vtkErrorMacro("Oops, max value lower than lower cutoff!");
      max = this->LowerCutoff + 1;
    }
      if (intensity < this->LowerCutoff)
    intensity = this->LowerCutoff;
    }

  // try to spread the data out in the output range
  float range = max-min;
  float x = intensity - min;
  float x_frac = x/range;

  switch (this->TransformationFunctionNumber)
    {
    case INVERSE_LINEAR_RAMP:
      //return (this->ScaleFactor - (intensity*this->ScaleFactor)/max);
      return (this->ScaleFactor - x_frac*this->ScaleFactor);
      break;
    case ONE_OVER_X:
      // scale * (1/1+x), where x = intensity/max
      // note: This should be done a bit better (min now is scale/2)
      // do 1/(1+x^2)
      //return (this->ScaleFactor*range/(range + x));
      return (this->ScaleFactor/(1 + x_frac*x_frac));
      break;
    default:
      vtkErrorMacro("Oops, no transformation function set!");
      return 0;
      break;
    }

}

float vtkImageLiveWireScale::TableLookup(float intensity, float max,
                       float min) 
{

  // this assumes the table is already built.
  
  // get number of pix w/ this intensity in histogram
  int bin = (int)intensity - this->MinimumBin;
  float numpix = LookupTable->GetValue(bin);  
  // get max number of pixels in any bin
  int binMax = this->MaxPointsInLookupTableBin;

  // "inverted" histogram: more points at this intensity means
  // it should cost less to travel through this pixel.
  // (1 - % pix at this intensity) * scaleFactor
  
  return(this->ScaleFactor*(binMax-numpix)/binMax);
  
}

template <class T>
static void vtkImageLiveWireScaleBuildLookupTable(vtkImageLiveWireScale *self,
                        vtkImageData *inData, 
                        T *inPtr, float max,
                        float min) 
{

  if (self->GetLookupPoints() == NULL) {
    cout 
      << "ERROR in build lookup table: Set LookupPoints before building table"
      << endl;
    self->SetUseLookupTable(0);
    return;
  }
  
  vtkPoints *LookupPoints = self->GetLookupPoints();
  int numberOfPoints = LookupPoints->GetNumberOfPoints();

  if (numberOfPoints == 0) {
    cout 
      << "ERROR in build lookup table: Need more than 0 LookupPoints before building table"
      << endl;
    
    self->SetUseLookupTable(0);
    return;
  }

  vtkFloatArray * LookupTable = self->GetLookupTable();
  LookupTable->Reset();
  
  int minBin = (int)min - 1;
  int maxBin = (int)max + 1;
  int numBins = maxBin - minBin + 1;
  //cout << "min bin: " << minBin << " max bin: " << maxBin 
  //<< " num bins: " << numBins << endl;
  // save for calculation of bins from intensities
  self->SetMinimumBin(minBin);
  self->SetMaximumBin(maxBin);

  float maxPixInBin = 0;

  LookupTable->SetNumberOfValues(numBins);
  int i;
  for (i=0; i<numBins; i++)
    {
      LookupTable->SetValue(i,0);
    }

  // note: may only want to use some of the points later.
  for (i = 0; i < numberOfPoints; i++) 
    {
      // for each point, grab its intensity value and 
      // build a histogram to use as the LUT
      float * point = LookupPoints->GetPoint(i);
      // get intensity at that point.
      T *pix = (T*)inData->GetScalarPointer((int)point[0],
                        (int)point[1],
                        0);
      int intensity = (int)*pix;
      int bin = intensity - minBin;
      
      // get previous bin count
      float numpix = LookupTable->GetValue(bin);

      // if max pixel count in any bin so far
      if (numpix > maxPixInBin)
    maxPixInBin = numpix;

      // increment bin count
      LookupTable->SetValue(bin,numpix+1);

      //cout << "I: " << intensity << " bin: " << bin << "pix " << LookupTable->GetValue(bin)<< endl;
    }

  // dump histogram
//    for (i=0; i<numBins; i++)
//      {
//        cout << i << ": " << LookupTable->GetValue(i) << endl;;
//      }

  // now smooth the histogram
  // need to keep track of max pix
  float maxPixInSmoothedBin = 0;
  vtkFloatArray *tempBins = vtkFloatArray::New();
  tempBins->SetNumberOfValues(numBins);
  for (i=0; i<numBins; i++)
    {
      tempBins->SetValue(i,0);
    }
  
  int offsets[6] = {-3,-2,-1,1,2,3};
  float fractions[6] = {0.2,0.3,0.5,0.5,0.3,0.2};
  // loop through entire histogram
  for (i = 0; i < numBins; i++) 
    {
      float numpix = LookupTable->GetValue(i);

      for (int j = 0; j < 6; j++)
    {
      int bin = i+offsets[j];

      if (bin >= 0 && bin < numBins)
        {
          // add fraction of central bin to this one
          float count = tempBins->GetValue(bin);
          count += numpix*fractions[j];
          tempBins->SetValue(bin,count);

          if (count > maxPixInSmoothedBin)
        maxPixInSmoothedBin = count;
        }
    }
    }

  // now need to add these "extra fractions" onto the real bins
  for (i=0; i<numBins; i++)
    {
      float newval = LookupTable->GetValue(i) + tempBins->GetValue(i);
//        cout << i << ": " << LookupTable->GetValue(i) <<
//         " tB: " << tempBins->GetValue(i) << endl;;
      LookupTable->SetValue(i,newval);
      // dump histogram
      //cout << i << ": " << LookupTable->GetValue(i) << endl;
    }

//    self->SetTotalPointsInLookupTable(numberOfPoints);
  //self->SetMaxPointsInLookupTableBin(maxPixInBin);
  self->SetMaxPointsInLookupTableBin(maxPixInSmoothedBin);
  if (maxPixInSmoothedBin == 0) 
    {
      cout 
    << "ERROR in build lookup table: No points in histogram! Not using it."
    << endl;
      self->SetUseLookupTable(0);
    }

  //tempBins->DebugOn();
  //tempBins->Print(cout);
  tempBins->Delete();
  //cout << "built histogram" << endl;

}

float vtkImageLiveWireScale::GaussianLookup(float intensity, float max,
                      float min) 
{
  // "inverted" Gaussian: closer to avg gives lower cost
  float tmp = intensity - this->MeanForGaussianModel;

  return(this->ScaleFactor*(1 - exp( -(tmp*tmp)/(2*this->VarianceForGaussianModel) ))); 
  
}
template <class T>
static void vtkImageLiveWireScaleBuildGaussianModel(vtkImageLiveWireScale *self,
                        vtkImageData *inData, 
                        T *inPtr, float max,
                        float min) 
{

  if (self->GetLookupPoints() == NULL) {
    cout 
      << "ERROR in build lookup table: Set LookupPoints before building table"
      << endl;
    self->SetUseGaussianLookup(0);
    return;
  }
  
  vtkPoints * LookupPoints = self->GetLookupPoints();
  int numberOfPoints = LookupPoints->GetNumberOfPoints();

  if (numberOfPoints == 0) {
    cout 
      << "ERROR in build lookup table: Need more than 0 LookupPoints before building table"
      << endl;
    
    self->SetUseGaussianLookup(0);
    return;
  }

  // note: may only want to use some of the points later.
  float sum = 0;
  float var = 0;
  for (int i = 0; i < numberOfPoints; i++) 
    {
      // for each point, grab its intensity value and 
      // find the average
      float * point = LookupPoints->GetPoint(i);
      // get intensity at that point.
      T *pix = (T*)inData->GetScalarPointer((int)point[0],
                        (int)point[1],
                        0);
      float intensity = (float)*pix;
      sum += intensity;
      var += intensity*intensity;
    }
  float average = sum/numberOfPoints;
  float variance = var/numberOfPoints - average*average;
  self->SetMeanForGaussianModel(average);
  self->SetVarianceForGaussianModel(variance);
  //cout << "built G model" << endl;

}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageLiveWireScaleExecute(vtkImageLiveWireScale *self,
                      vtkImageData *inData, T *inPtr,
                      vtkImageData *outData, float *outPtr)
{
  int outExt[6];
  unsigned long count = 0;
  unsigned long target;
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  int inInc0, inInc1, inInc2;
  int outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  float *outPtr0, *outPtr1, *outPtr2;
  int inImageMin0, inImageMin1, inImageMin2;
  int inImageMax0, inImageMax1, inImageMax2;
  clock_t tStart, tEnd, tDiff;
  T pix;
  int scaleFactor;

  tStart = clock();
  scaleFactor = self->GetScaleFactor();

  // Get information to march through data
  inData->GetIncrements(inInc0, inInc1, inInc2); 
  self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
                   inImageMax1, inImageMin2, inImageMax2);
  outData->GetIncrements(outInc0, outInc1, outInc2); 
  outData->GetExtent(outExt);
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];
    
  // in and out should be marching through corresponding pixels.
  inPtr = (T *)(inData->GetScalarPointerForExtent(inData->GetExtent()));
  //inPtr = (T *)(inData->GetScalarPointer(outMin0, outMin1, outMin2));

  target = (unsigned long)((outMax2-outMin2+1)*(outMax1-outMin1+1)/50.0);
  target++;

  // note: this code assumes that the image is all >= 0 already. (!!!!!!)
  T max = *inPtr;
  T min = *inPtr;

  // Two loops: first computes max and min, then the second scales each pixel.
  // loop through pixels of input
  inPtr2 = inPtr;
  for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
       !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
    {
      if (!(count%target))
        self->UpdateProgress(count/(50.0*target));
      count++;

      inPtr0 = inPtr1;
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          pix = *inPtr0;

          if (pix > max)
        max = pix;
          else
        if (pix < min)
          min = pix;
          
          inPtr0 += inInc0;
        }//for0
      inPtr1 += inInc1;
    }//for1
      inPtr2 += inInc2;
    }//for2
  //cout <<  "max : " << max << endl;


  // second loops use max, min info to scale input:

  // check if the lookup table is doing okay
  if (self->GetUseLookupTable()) 
    {
      vtkImageLiveWireScaleBuildLookupTable(self, inData, inPtr, max, min);
    }
  if (self->GetUseGaussianLookup()) 
    {
      vtkImageLiveWireScaleBuildGaussianModel(self, inData, inPtr, max, min);
    }

  // Use gaussian if requested
  if (self->GetUseGaussianLookup())
    {
      //cout << "using gaussian lookup" << endl;
      // loop through input and produce output:
      // output = input pixel/max
      outPtr2 = outPtr;
      inPtr2 = inPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
        {
          if (!(count%target)) 
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
      
          outPtr0 = outPtr1;
          inPtr0 = inPtr1;
          for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          *outPtr0 = self->GaussianLookup(*inPtr0, max, min);

          inPtr0 += inInc0;
          outPtr0 += outInc0;
        }//for0
          inPtr1 += inInc1;
          outPtr1 += outInc1;
        }//for1
      inPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2
      
    } // end use gaussian

  // else use LUT
  else if (self->GetUseLookupTable())
    {
      //cout << "using lookup table" << endl;
      // loop through input and produce output:
      // output = input pixel/max
      outPtr2 = outPtr;
      inPtr2 = inPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
        {
          if (!(count%target)) 
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
      
          outPtr0 = outPtr1;
          inPtr0 = inPtr1;
          for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {

          *outPtr0 = self->TableLookup(*inPtr0, max, min);

          inPtr0 += inInc0;
          outPtr0 += outInc0;
        }//for0
          inPtr1 += inInc1;
          outPtr1 += outInc1;
        }//for1
      inPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2
      
    }// end use LUT

  // else try transformation function
  else if (self->GetUseTransformationFunction())
    {
      //cout << "using transformation fcn" << endl;
      // loop through input and produce output:
      // output = input pixel/max
      outPtr2 = outPtr;
      inPtr2 = inPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
        {
          if (!(count%target)) 
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
      
          outPtr0 = outPtr1;
          inPtr0 = inPtr1;
          for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          *outPtr0 = self->TransformationFunction(*inPtr0, max, min);

          inPtr0 += inInc0;
          outPtr0 += outInc0;
        }//for0
          inPtr1 += inInc1;
          outPtr1 += outInc1;
        }//for1
      inPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2
      
    } // end use transformation

  // default: just shift and scale
  else
    {
      //cout << "using default shift and scale" << endl;
      T range = max - min;
      if (range == 0) 
    range = 1;
      
      // loop through input and produce output:
      // output = input pixel/max
      outPtr2 = outPtr;
      inPtr2 = inPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
           !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
        {
          if (!(count%target)) 
        {
          self->UpdateProgress(count/(50.0*target));
        }
          count++;
      
          outPtr0 = outPtr1;
          inPtr0 = inPtr1;
          for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {

          //*outPtr0 = (*inPtr0 * scaleFactor)/max;
          *outPtr0 = (*inPtr0-min) * scaleFactor/range;

          inPtr0 += inInc0;
          outPtr0 += outInc0;
        }//for0
          inPtr1 += inInc1;
          outPtr1 += outInc1;
        }//for1
      inPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2

    } // end if in basic case: no transformation, no LUT

  // ------------------------------------------------------------
  // ERROR checking only
  
  outPtr2 = outPtr;
  inPtr2 = inPtr;
  for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      for (outIdx1 = outMin1; 
       !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
    {
      if (!(count%target)) 
        {
          self->UpdateProgress(count/(50.0*target));
        }
      count++;
      
      outPtr0 = outPtr1;
      inPtr0 = inPtr1;
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          
          if (*outPtr0 > scaleFactor) 
        {
          cout << "ERROR in vtkImageLiveWireScale: pix > scale factor "
               << " pix: " << *outPtr0
               << " s.f: " << scaleFactor
               << endl;
        }
          
          inPtr0 += inInc0;
          outPtr0 += outInc0;
        }//for0
      inPtr1 += inInc1;
      outPtr1 += outInc1;
    }//for1
      inPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2

  // end ERROR checking only
  // ------------------------------------------------------------

  tEnd = clock();
  tDiff = tEnd - tStart;
  //cout << "LW scale time: " << tDiff << endl;
}

    

//----------------------------------------------------------------------------
// This method is passed a input and output Data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkImageLiveWireScale::Execute(vtkImageData *inData, 
                 vtkImageData *outData)
{
  void *inPtr;
  float *outPtr;

  inPtr  = inData->GetScalarPointer();

  //cout << "type: " << outData->GetScalarType() << endl;
  outPtr = (float *)outData->GetScalarPointer();

  switch (inData->GetScalarType())
  {
    case VTK_CHAR:
      vtkImageLiveWireScaleExecute(this, 
              inData, (char *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireScaleExecute(this, 
              inData, (unsigned char *)(inPtr), outData, outPtr);
      break;
    case VTK_SHORT:
      vtkImageLiveWireScaleExecute(this, 
              inData, (short *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireScaleExecute(this, 
              inData, (unsigned short *)(inPtr), outData, outPtr);
      break;
    case VTK_INT:
      vtkImageLiveWireScaleExecute(this, 
              inData, (int *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireScaleExecute(this, 
              inData, (unsigned int *)(inPtr), outData, outPtr);
      break;
    case VTK_LONG:
      vtkImageLiveWireScaleExecute(this, 
              inData, (long *)(inPtr), outData, outPtr);
      break;
    case VTK_UNSIGNED_LONG:
      vtkImageLiveWireScaleExecute(this, 
              inData, (unsigned long *)(inPtr), outData, outPtr);
      break;
    case VTK_FLOAT:
      vtkImageLiveWireScaleExecute(this, 
              inData, (float *)(inPtr), outData, outPtr);
      break;
    case VTK_DOUBLE:
      vtkImageLiveWireScaleExecute(this, 
              inData, (double *)(inPtr), outData, outPtr);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unsupported ScalarType");
      return;
    }
}

//----------------------------------------------------------------------------
void vtkImageLiveWireScale::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
                        vtkImageData *output)
{
  output->SetNumberOfScalarComponents(1);
  output->SetScalarType(VTK_FLOAT);
}

void vtkImageLiveWireScale::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  // numbers
  os << indent << "ScaleFactor: "<< this->ScaleFactor << "\n";
  os << indent << "UpperCutoff: "<< this->UpperCutoff << "\n";
  os << indent << "LowerCutoff: "<< this->LowerCutoff << "\n";
  os << indent << "UseUpperCutoff: "<< this->UseUpperCutoff << "\n";
  os << indent << "UseLowerCutoff: "<< this->UseLowerCutoff << "\n";
  os << indent << "UseLookupTable: "<< this->UseLookupTable << "\n";
  os << indent << "UseGaussianLookup: "<< this->UseGaussianLookup << "\n";
  os << indent << "MeanForGaussianModel: "<< this->MeanForGaussianModel << "\n";
  os << indent << "VarianceForGaussianModel: "<< this->VarianceForGaussianModel << "\n";
  os << indent << "UseTransformationFunction: "<< this->UseTransformationFunction << "\n";
  os << indent << "TransformationFunctionNumber: "<< this->TransformationFunctionNumber << "\n";
  os << indent << "TotalPointsInLookupTable: "<< this->TotalPointsInLookupTable << "\n";
  os << indent << "MaxPointsInLookupTableBin: "<< this->MaxPointsInLookupTableBin << "\n";
  os << indent << "MinimumBin: "<< this->MinimumBin << "\n";
  os << indent << "MaximumBin: "<< this->MaximumBin << "\n";
}

