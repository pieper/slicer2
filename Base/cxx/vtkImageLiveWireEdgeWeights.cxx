/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

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
#include "vtkImageLiveWireEdgeWeights.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include <math.h>
#include <time.h>

featureProperties::featureProperties()
{
  this->Transform = &featureProperties::GaussianCost;
  this->NumberOfParams = 2;
  this->TransformParams = new float[this->NumberOfParams];
  this->TransformParams[0] = 0;
  this->TransformParams[1] = 1;
  this->Weight = 1;
}

featureProperties::~featureProperties()
{
  if (this->TransformParams != NULL) 
    {
      delete [] this->TransformParams;
    }
}

float featureProperties::GaussianCost(float x)
{
  float mean = this->TransformParams[0];
  float var = this->TransformParams[1];
  return(exp(-((x-mean)*(x-mean))/(2*var))/sqrt(6.28318*var));  
}

// inverse "Gaussian": low cost for good edges
inline float GaussianC(float x, float mean, float var)
{
  // This is the time bottleneck of this filter.
  // So only bother to compute the gaussian if this is a 
  // "good feature" (with a value close to the mean).
  // Else return the max value of 1.
  //    float tmp = x-mean;
  //    if (tmp < var)
  //      return(1 - exp(-(tmp*tmp)/(2*var))/sqrt(6.28318*var));  
  //    else
  //      return 1;

  // we need between 0 and 1 always, so:
  // forget about the scale factor.  the feature weight does this.
  // so just do the e^-((x-u)^2/2*sigma^2)

  float tmp = x-mean;
  return(1 - exp( -(tmp*tmp)/(2*var) ));  

}

//------------------------------------------------------------------------------
vtkImageLiveWireEdgeWeights* vtkImageLiveWireEdgeWeights::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageLiveWireEdgeWeights");
  if(ret)
    {
      return (vtkImageLiveWireEdgeWeights*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageLiveWireEdgeWeights;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageLiveWireEdgeWeights::vtkImageLiveWireEdgeWeights()
{
  // all inputs 
  this->NumberOfRequiredInputs = 1;
  this->NumberOfInputs = 0;

  this->FileName = NULL;
  this->TrainingFileName = NULL;

  this->MaxEdgeWeight = 255;
  this->EdgeDirection = DOWN_EDGE;

  this->NumberOfFeatures = 6;

  this->FeatureSettings = new featureProperties[this->NumberOfFeatures];

  // Lauren try 3D kernel someday (need 3D input)
  this->Neighborhood = 3; // 3x3 neighborhood

  this->TrainingMode = 0;
  this->TrainingComputeRunningTotals = 0;
  this->NumberOfTrainingPoints = 0;
  this->RunningNumberOfTrainingPoints = 0;
  this->TrainingAverages = new float[this->NumberOfFeatures];
  this->TrainingVariances = new float[this->NumberOfFeatures];
}


//----------------------------------------------------------------------------
vtkImageLiveWireEdgeWeights::~vtkImageLiveWireEdgeWeights()
{
  if (this->FileName != NULL)
    {
      delete [] this->FileName;
    }

  if (this->TrainingFileName != NULL)
    {
      delete [] this->TrainingFileName;
    }

  if (this->FeatureSettings != NULL)
    {
      delete [] this->FeatureSettings;
    }

  if (this->TrainingAverages != NULL)
    {
      delete [] this->TrainingAverages;
    }
  if (this->TrainingVariances != NULL)
    {
      delete [] this->TrainingVariances;
    }
}

//----------------------------------------------------------------------------
// Description:
// Dump training settings to a file
void vtkImageLiveWireEdgeWeights::WriteFeatureSettings()
{
  ofstream file;

  if (this->TrainingFileName)
    {
      file.open(this->TrainingFileName);
      if (file.fail())
	{
	  vtkErrorMacro("Could not open file %" << this->TrainingFileName);
	  return;
	}  
    }
  else 
    {
      vtkErrorMacro("FileName has not been set");
      return;
    }

  // output the features  
  for (int i=0; i < this->NumberOfFeatures; i++)
    {
      file << this->GetWeightForFeature(i) << ' ' 
	   << this->TrainingAverages[i]    << ' '  
	   << this->TrainingVariances[i]   << endl;
    }
  
  file.close();
}


//----------------------------------------------------------------------------
// Description:
// Output training settings to a file (already opened)
void vtkImageLiveWireEdgeWeights::AppendFeatureSettings(ofstream& of)
{

  // output the features
  
  for (int i=0; i < this->NumberOfFeatures; i++)
    {
      of << this->GetWeightForFeature(i) << ' ' 
	 << this->GetParamForFeature(i,0)    << ' '  
	 << this->GetParamForFeature(i,1)   << endl;
    }

}

//----------------------------------------------------------------------------
// Description:
// Output training settings to a file (already opened)
void vtkImageLiveWireEdgeWeights::GetFeatureSettingsString(char *settings)
{
  char set[10];
  int count = 0;

  // append the features
  for (int i=0; i < this->NumberOfFeatures; i++)
    {
      snprintf(set, 10, "%9f.4", this->GetWeightForFeature(i));
      strcat(settings, set);

      snprintf(set, 10, "%9f.4", this->GetParamForFeature(i,0));
      strcat(settings, set);

      snprintf(set, 10, "%9f.4", this->GetParamForFeature(i,1));
      strcat(settings, set);
    }
}

//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
// For every pixel in the foreground, if a neighbor is in the background,
// then the pixel becomes background.
template <class T>
static void vtkImageLiveWireEdgeWeightsExecute(vtkImageLiveWireEdgeWeights *self,
					      vtkImageData **inDatas, T **inPtrs,
					      vtkImageData *outData,
					      int outExt[6], int id)
{
  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  int inInc0, inInc1, inInc2;
  int outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  // pointers to training data image
  T *inTPtr0, *inTPtr1, *inTPtr2;
  T *outPtr0, *outPtr1, *outPtr2;
  // For looping through hood pixels
  int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
  int hoodIdx0, hoodIdx1, hoodIdx2;
  int offsetPtr0, offsetPtr1, offsetPtr2;
  int  *nPtr, *nPtr0, *nPtr1, *nPtr2;
  // Lauren should loop through hoodCopy nicer!
  int maskInc0, maskInc1, maskInc2;
  // The extent of the whole input image
  int inImageMin0, inImageMin1, inImageMin2;
  int inImageMax0, inImageMax1, inImageMax2;
  // Other
  T pix;
  T *outPtr = (T*)outData->GetScalarPointerForExtent(outExt);
  unsigned long count = 0;
  unsigned long target;

  clock_t tStart, tEnd, tDiff;
  tStart = clock();
  
  // how many features to compute per voxel
  int numFeatures = self->GetNumberOfFeatures();
  
  // Get information to march through data
  inDatas[0]->GetIncrements(inInc0, inInc1, inInc2); 
  self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
				   inImageMax1, inImageMin2, inImageMax2);
  //cout << inImageMin2 << " " << inImageMax2 << endl;
  outData->GetIncrements(outInc0, outInc1, outInc2); 
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];

  // Neighborhood around current voxel
  //----- Lauren this is hard-coded for Neighborhood == 3 now: -----
  hoodMin0 = - 1;
  hoodMin1 = - 1;
  hoodMin2 = 0;

  hoodMax0 = 1;
  hoodMax1 = 1;
  hoodMax2 = 0;
  //cout << hoodMin0<<hoodMax0<<hoodMin1 <<hoodMax1 << hoodMin2 <<hoodMax2 << endl;
  // Set up mask info
  //  maskPtr = (unsigned char *)(self->GetMaskPointer());
  // Lauren use to loop through hoodCopy
  // Lauren fix this!
  maskInc0 = 1;
  maskInc1 = 3;
  maskInc2 = 9;

  // in and out should be marching through corresponding pixels.
  target = (unsigned long)((outMax2-outMin2+1)*
			   (outMax1-outMin1+1)/50.0);
  target++;


  // set up filter directionality
  // ----------------------------------------------
  // From paper, layout of neighborhood is:
  // t | u
  //   ^
  // p | q  (p,q = the "bel" whose upward edge we are computing)
  // v | w
  // I want clockwise segmentation, so "q" should be inside.

  int neighborhoodWidth = self->GetNeighborhood();

  int t,u,p,q,v,w;
  if (neighborhoodWidth == 3) 
    {
      // compute these edges at each pixel, x:
      //
      //  ->  ^
      //  | x |
      //  V  <-
      //
      // then the right and down arrows belong to the upper left corner
      // and the other two to the lower right corner.
      // so shifting output images will give the correct:
      //
      //    ^
      //  <-|->
      //    V
      // where each pixel has the correct weight for 
      // all of one corner's outward paths
      // (segmentation is done on the borders between pixels)
      
      // neighborhood looks like:
      // 0 1 2
      // 3 4 5
      // 6 7 8
      // rotate neighborhood for all edges.
      
      switch (self->GetEdgeDirection()) 
	{
	case UP_EDGE:
	  {
	    //      | 7       | 8       |
	    // bel: | 4 (out) | 5 (in)  |
	    //                V
	    //      | 6       | 7       |
	    //
	    // this seems to point down, but so does vtk's y-axis.
	    t = 7; 
	    u = 8;
	    p = 4;
	    q = 5;
	    v = 1;
	    w = 2;
	    break;
	  }
	case DOWN_EDGE:
	  {
	    //      | 6       | 7      |
	    // bel:           ^
	    //      | 3 (in)  | 4 (in) |
	    //      | 0       | 1      |
	    //
	    t = 1; 
	    u = 0;
	    p = 4;
	    q = 3;
	    v = 7;
	    w = 6;
	    break;
	  }
	case LEFT_EDGE:
	  {
	    //  6  bel: 7 (out)  8
	    //         <----
	    //  3       4 (in)   5
	    t = 3; 
	    u = 6;
	    p = 4;
	    q = 7;
	    v = 5;
	    w = 8;
	    break;
	  }
	case RIGHT_EDGE:
	  {
	    // 3  bel: 4 (out)   5
	    //         ---->
	    // 0       1 (in)    2
	    t = 5; 
	    u = 2;
	    p = 4;
	    q = 1;
	    v = 3;
	    w = 0;
	    break;
	  }
	default:
	  {
	    cout << "ERROR: bad edge direction" << endl;
	    return;
	  }
	}
      
    }

  // offsets to index into neighboring pixels
  int *n = new int[neighborhoodWidth*neighborhoodWidth];

  // loop through neighborhood indices and record offsets 
  // from inPtr0 in n[] array
  offsetPtr2 = inInc0*hoodMin0 + inInc1*hoodMin1 + inInc2*hoodMin2;
  nPtr2 = n;
  for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
    {
      offsetPtr1 = offsetPtr2;
      nPtr1 = nPtr2;
      for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
	{
	  offsetPtr0 = offsetPtr1;
	  nPtr0 = nPtr1;
	  for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
	    {
	      // store offset into n array
	      *nPtr0 = offsetPtr0;	      
	      offsetPtr0 += inInc0;
	      nPtr0++;	      
	    }//for0
	  offsetPtr1 += inInc1;
	  nPtr1 += neighborhoodWidth;
	}//for1
      offsetPtr2 += inInc2;
      nPtr2 += neighborhoodWidth*neighborhoodWidth;
    }//for2  
  
  // scale factor (edges go from 1 to this number)
  int maxEdge = self->GetMaxEdgeWeight();

  // temporary storage of features computed at a voxel
  float *features = new float[numFeatures];

  // storage of training data
  float *average = self->GetTrainingAverages();      
  float *variance = self->GetTrainingVariances();      
  int numberOfTrainingPoints = 0;

  // if we are computing training data
  if (self->GetTrainingMode()) 
    {
      // if we are not computing features over multiple slices
      if (!self->GetRunningNumberOfTrainingPoints()) 
	{
	  memset(average,0,numFeatures*sizeof(float));
	  // don't allow 0 variance to be calculated (will break Gaussian).
	  for (int i = 0; i<numFeatures; i++)
	    variance[i] = 0.01;
	}
    }
  // compute normalization factor
  float sumOfWeights = 0;
  for (int i = 0; i < numFeatures; i++) 
    {
      sumOfWeights += self->GetWeightForFeature(i);
    }

  float testMax = -1;
  float testMin = 256;
  float testMax2 = -1;
  float testMin2 = 256;

  // loop through pixels of output
  outPtr2 = outPtr;
  inPtr2 = inPtrs[0];
  inTPtr2 = inPtrs[2];
  for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
    {
      outPtr1 = outPtr2;
      inPtr1 = inPtr2;
      inTPtr1 = inTPtr2;
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
	  inPtr0 = inPtr1;
	  inTPtr0 = inTPtr1;
	  for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
	    {
	      //cout << "(" << outIdx0 << "," << outIdx1 <<"," << outIdx2 <<")" << endl;

	      // ---- Neighborhood Operations ---- //

	      // make sure *entire* kernel is within boundaries.
	      // Note this means that a 3-D kernel won't 
	      // work on 2-D input!
	      // Lauren this is faster but better other way?
	      if (outIdx0 + hoodMin0 >= inImageMin0 &&
		  outIdx0 + hoodMax0 <= inImageMax0 &&
		  outIdx1 + hoodMin1 >= inImageMin1 &&
		  outIdx1 + hoodMax1 <= inImageMax1 &&
		  outIdx2 + hoodMin2 >= inImageMin2 &&
		  outIdx2 + hoodMax2 <= inImageMax2)
		{
		  // Lauren!  if gradient doesn't agree with desired one,
		  // this is when q/p should be switched!  so 
		  // need to keep track of *desired* gradient!  It is not correct right now!

		  // FEATURES
		  // (Please document all new features here.)
		  // 0: in pix magnitude = q OR p, depending on gradient dir
		  // 1: out pix magnitude = p OR q, "
		  // 2: outpix-inpix = p-q
		  // 3: gradient = (1/3)*(p+t+v-u-q-w)				
		  // 4: gradient = (1/2)*(p+t/2+v/2 -u-q/2-w/2)
		  // 5: gradient = (1/4)*(p-u + t-q + p-w + v-q)
		  // ----------------------------------------------
		  if (neighborhoodWidth == 3) 
		    {
		      T *ptr = inPtr0;
		      // Compute various features:			  
		      features[2] = *(ptr+n[p]) - *(ptr+n[q]);

		      //  		  cout << n[p] << endl;
		      //  			  cout << n[q] << endl;
		      //  			  cout << n[t] << endl;
		      //  			  cout << n[u] << endl;
		      //  			  cout << n[v] << endl;
		      //  			  cout << n[w] << endl;

		      features[3] = .333333*(*(ptr+n[p])+*(ptr+n[t])+*(ptr+n[v])-*(ptr+n[u])-*(ptr+n[q])-*(ptr+n[w]));


		      features[0] = *(ptr+n[q]); // "in"
		      features[1] = *(ptr+n[p]);

		      // Lauren: if f3 > 0 actually the 'inside' pixel 
		      // is darker.  so the below code is backwards:
		      // switching is only good for consistency if we 
		      // don't know the desired gradient, it seems...

		      // "in" corresponds to side of bel with higher intensity//  .
//  		      if (features[3] > 0)
//  			{
//  			  features[0] = *(ptr+n[q]); // "in"
//  			  features[1] = *(ptr+n[p]);
//  			}
//  		      else 
//  			{
//  			  features[0] = *(ptr+n[p]);
//  			  features[1] = *(ptr+n[q]);
//  			}

		      features[4] = .5*(*(ptr+n[p])+*(ptr+n[t])/2+*(ptr+n[v])/2
					-*(ptr+n[u])-*(ptr+n[q])/2-*(ptr+n[w])/2);
		      features[5] = .25*(*(ptr+n[p])-*(ptr+n[u]) + *(ptr+n[t])-*(ptr+n[q]) + 
					 *(ptr+n[p])-*(ptr+n[w]) + *(ptr+n[v])-*(ptr+n[q]));
		    }
		  else
		    if (neighborhoodWidth == 5) 
		      {
			// compute same stuff as above smaller kernel
			    
			// in pix magnitude = p
			features[0] = n[12];
			// out pix magnitude = q
			features[1] = n[11];
			// inpix-outpix = p-q
			features[2] = n[12]-n[11];
			// gradient = (1/3)*(p+t+v-u-q-w)
			features[3] = .333333*(n[11]+n[6]+n[16]
					       -n[7]-n[12]-n[17]);
			// gradient = (1/2)*(p+t/2+v/2 -u-q/2-w/2)
			features[4] = .5*(n[11]+n[6]/2+n[16]/2
					  -n[7]-n[12]/2-n[17]/2);
			// gradient = (1/4)*(p-u + t-q + p-w + v-q)
			features[5] = .25*(n[11]-n[7] + n[6]-n[12] + 
					   n[11]-n[17] + n[16]-n[12]);

			// compute larger kernel size things
			// 2 in pix avg magnitude
			features[6] = .5*(n[12] + n[13]);
			// 2 out pix avg magnitude
			features[7] = .5*(n[10] + n[11]);
			// 4 in pix avg magnitude
			features[8] = .25*(n[12]+n[13]+n[8]+n[18]);
			// 4 out pix avg magnitude
			features[9] = .25*(n[10]+n[11]+n[5]+n[15]);

			// weird: sort of std. dev (in)
			features[10] = n[8]+n[13]+n[18]-3*n[12];
			features[11] = n[5]+n[10]+n[15]-3*n[11];

			// variances...
			// 2 in pix var
			float m = features[6];
			features[12] = .5*((n[12]-m)*(n[12]-m) + 
					   (n[13]-m)*(n[13]-m));
			// 2 out pix var
			m = features[9];
			features[13] = .5*((n[10]-m)*(n[10]-m) + 
					   (n[11]-m)*(n[11]-m));
			// 4 in pix var
			m = features[8];
			features[14] = .25*((n[12]-m)*(n[12]-m) + 
					    (n[13]-m)*(n[13]-m) +
					    (n[8]-m)*(n[8]-m) +
					    (n[18]-m)*(n[18]-m));

			// 4 out pix var
			m = features[9];
			features[15] = .25*((n[10]-m)*(n[10]-m) + 
					    (n[11]-m)*(n[11]-m) +
					    (n[5]-m)*(n[5]-m) +
					    (n[15]-m)*(n[15]-m));
			// Lauren need to do in/out depending on gradient dir!
			    
			//also 3 pix mean, var?
			    
						
		      }			

		  // Lauren 3D kernel someday!!!!!!

		  // If we are in training mode, need to compute
		  // average values of each feature over certain
		  // points in the image
		  if (self->GetTrainingMode()) 
		    {
		      float point[3];
		      point[0] = outIdx0;
		      point[1] = outIdx1;
		      point[2] = 0;
		      
		      // if this (inside, or 'q') pixel is in the
		      // segmented area, and its neighboring
		      // outside, or 'p', pixel, is NOT.
		      if (*(inTPtr0 + n[q]) == 1 && *(inTPtr0 + n[p]) == 0){
			//cout << "point: " << point[0] << " " << point[1];
			for (int i=0;i<numFeatures;i++)
			  {
			    // train at this spot
			    average[i] += features[i];
			    variance[i] += (features[i])*(features[i]);
			  }		
			numberOfTrainingPoints++;
		      }	  
		    }

		  // convert features to an edge weight
		  featureProperties *props;
		  float sum = 0;
		  for (int i=0;i<numFeatures;i++)
		    {
		      props = self->GetFeatureSettings(i);

		      // don't compute if weight is 0
		      if (props->Weight != 0) 
			{
			  //sum += props->Weight*GaussianC(features[i],props->TransformParams[0],props->TransformParams[1]);

			  // junk for testing:
			  float tmp2 = GaussianC(features[i],props->TransformParams[0],props->TransformParams[1]);
			  if (tmp2 > 1)
			    cout << "feature " << i << " too large: " << tmp2 << endl;
			  
			  float tmp = props->Weight*tmp2;
			  if (tmp < testMin) 
			    testMin = tmp;
			  else
			    if (tmp > testMax)
			      testMax = tmp;
			 
			  sum += tmp;
			  
			}
		    }
		  // each feature is between 0 and its Weight.  
		  // normalize sum to 1 and multiply by max edge cost.
		  *outPtr0 = (sum*maxEdge/sumOfWeights);
		  float tmp = (sum*maxEdge/sumOfWeights);
		  if (tmp < testMin2) 
		    testMin2 = tmp;
		  
		  if (tmp > testMax2)
		    testMax2 = tmp;
		  

		  if (*outPtr0 > maxEdge) 
		    {
		      cout << "ERROR in vtkImageLWEdgeWeights: edge cost too high " << *outPtr0 << endl;
		    }
		      
		}  // end neighborhood operations
	      else
		{
		  // handle boundaries: default output equal to max edge value
		  *outPtr0 = maxEdge;
		}

	      // ---- End Neighborhood Operations ---- //
		  
	      inPtr0 += inInc0;
	      inTPtr0 += inInc0;
	      outPtr0 += outInc0;
	    }//for0
	  inPtr1 += inInc1;
	  inTPtr1 += inInc1;
	  outPtr1 += outInc1;
	}//for1
      inPtr2 += inInc2;
      inTPtr2 += inInc2;
      outPtr2 += outInc2;
    }//for2

  if (n != NULL) 
    {
      delete [] n;
    }
  if (features != NULL) 
    {
      delete [] features;
    }

  if (self->GetTrainingMode()) 
    {
      // increment the running number of points
      int numPoints = self->GetRunningNumberOfTrainingPoints();
      numPoints += numberOfTrainingPoints;
      self->SetRunningNumberOfTrainingPoints(numPoints);

      cout << "total points: " << numPoints << "num points: " << numberOfTrainingPoints << endl;

      // if we are not doing running totals, finish computing averages
      // (this could also be the last slice of a running total)
      if (!self->GetTrainingComputeRunningTotals())
	{
	  // if we trained on any points
	  if (numPoints > 0) {
	    
	    // then divide by total number of pixels
	    for (int i=0;i<numFeatures;i++)
	      {
		average[i] = average[i]/numPoints;
		cout << "avg: " << average[i] << " ";
		variance[i] = variance[i]/numPoints - (average[i])*(average[i]);
		cout << "var: " << variance[i] << " ";
	      }			  
	    cout << endl;
	    
	    // set the total number of points used to compute the averages
	    self->SetNumberOfTrainingPoints(numPoints);
	    
	    // clear the running total
	    self->SetRunningNumberOfTrainingPoints(0);
	    
	    // set this filter's settings to the trained ones.
	    // Lauren do weight if train it later
	    for (int f=0;f<numFeatures;f++)
	      {
		self->SetParamForFeature(f,0,average[f]);
		self->SetParamForFeature(f,1,variance[f]);
	      }
	  }
	  else {
	    cout << "No contour points to train on!" << endl;
	  }
	}
    }

  cout << "min: " << testMin << " max: " << testMax << " min2: " << testMin2 << " max2: " << testMax2 << endl;
  tEnd = clock();
  tDiff = tEnd - tStart;
  cout << "time: " << tDiff << endl;
}

//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageLiveWireEdgeWeights::ThreadedExecute(vtkImageData **inDatas, 
						 vtkImageData *outData,
						 int outExt[6], int id)
{

  // Lauren allocate additional outputs here.
  // done like this in vtkImageToImageFilter:
  //    vtkImageData *output = this->GetOutput();
  
  //    output->SetExtent(output->GetUpdateExtent());
  //    output->AllocateScalars();
  //    this->Execute(this->GetInput(), output);

  void *inPtrs[3], *outPtr;
  inPtrs[0] = inDatas[0]->GetScalarPointerForExtent(inDatas[0]->GetExtent());

  if (this->NumberOfInputs > 1)
    {
      inPtrs[1] = inDatas[1]->GetScalarPointerForExtent(inDatas[1]->GetExtent());
    }
  if (this->NumberOfInputs > 2)
    {
      inPtrs[2] = inDatas[2]->GetScalarPointerForExtent(inDatas[2]->GetExtent());
    }
  outPtr = outData->GetScalarPointerForExtent(outData->GetExtent());

  switch (inDatas[0]->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (double **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (float **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_LONG:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (long **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_INT:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (int **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (unsigned int **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_SHORT:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (short **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (unsigned short **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_CHAR:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (char **)(inPtrs), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireEdgeWeightsExecute(this, inDatas, (unsigned char **)(inPtrs), 
					outData, outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }

}

// Lauren check inputs are same type and size
//----------------------------------------------------------------------------
// Make sure both the inputs are the same size. Doesn't really change 
// the output. Just performs a sanity check
void vtkImageLiveWireEdgeWeights::ExecuteInformation(vtkImageData **inputs,
						     vtkImageData *vtkNotUsed(output))
{
  int *in1Ext, *in2Ext;

  // we require that all inputs have been set.
  if (this->NumberOfInputs < this->NumberOfRequiredInputs)
    {
      vtkErrorMacro(<< "ExecuteInformation: Expected " << this->NumberOfRequiredInputs << " inputs, got only " << this->NumberOfInputs);
      return;      
    }
  
  // Lauren fix this to check all extents are the same size.
  in1Ext = inputs[0]->GetWholeExtent();

  if (this->NumberOfInputs > 1)
    {
      in2Ext = inputs[1]->GetWholeExtent();
    }
  else
    return;

  if (in1Ext[0] != in2Ext[0] || in1Ext[1] != in2Ext[1] || 
      in1Ext[2] != in2Ext[2] || in1Ext[3] != in2Ext[3] || 
      in1Ext[4] != in2Ext[4] || in1Ext[5] != in2Ext[5])
    {
      vtkErrorMacro("ExecuteInformation: Inputs are not the same size.");
      return;
    }
}
