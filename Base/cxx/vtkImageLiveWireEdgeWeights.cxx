/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
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
#include "vtkImageLiveWireEdgeWeights.h"
#include <time.h>
#include "vtkObjectFactory.h"
#include <math.h>

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
  return(exp(-((x-mean)*(x-mean))/(2*var*var))/sqrt(2*3.14159*var));  
}

float GaussianC(float x, float mean, float var)
{
  return(exp(-((x-mean)*(x-mean))/(2*var*var))/sqrt(2*3.14159*var));  
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
  this->FileName = NULL;

  this->MaxEdgeWeight = 255;
  
  this->Difference = 0.25;
  this->InsidePixel = 0.1;
  this->OutsidePixel = 0.1;

  this->NumberOfFeatures = 6;

  this->FeatureSettings = new featureProperties[this->NumberOfFeatures];

  // Lauren ?
  // Really, 3D kernel might make more sense!  
  // but would need 3D input and need to reformat output...
  // for now ignore mask, maybe change class inheritance later

  //this->SetKernelSize(5,5,1);
  this->SetKernelSize(3,3,1);
}


//----------------------------------------------------------------------------
vtkImageLiveWireEdgeWeights::~vtkImageLiveWireEdgeWeights()
{
  if (this->FileName != NULL)
    {
      delete [] this->FileName;
    }

  if (this->FeatureSettings != NULL)
    {
      delete [] this->FeatureSettings;
    }
}


//----------------------------------------------------------------------------
// Description:
// This templated function executes the filter for any type of data.
// For every pixel in the foreground, if a neighbor is in the background,
// then the pixel becomes background.
template <class T>
static void vtkImageLiveWireEdgeWeightsExecute(vtkImageLiveWireEdgeWeights *self,
					      vtkImageData *inData, T *inPtr,
					      vtkImageData *outData,
					      int outExt[6], int id)
{
  int *kernelSize;
  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  int inInc0, inInc1, inInc2;
  int outInc0, outInc1, outInc2;
  T *inPtr0, *inPtr1, *inPtr2;
  T *outPtr0, *outPtr1, *outPtr2;
  int numComps, outIdxC;
  // For looping through hood pixels
  int hoodMin0, hoodMax0, hoodMin1, hoodMax1, hoodMin2, hoodMax2;
  int hoodIdx0, hoodIdx1, hoodIdx2;
  T *hoodPtr0, *hoodPtr1, *hoodPtr2;
  // For looping through the mask.
  float  *hoodCopyPtr, *hoodCopyPtr0, *hoodCopyPtr1, *hoodCopyPtr2;
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

#ifdef LIVEWIRE_TRAINING_EXPERIMENT
  ofstream file;
  char *filename;
  filename = self->GetFileName();
  if (filename==NULL)
    {
      printf("Execute: Set the filename first");
      return;
    }
  file.open(filename);
  if (file.fail())
    {
    printf("Execute: Could not open file %", filename);
    return;
    }  
#endif

  // Get information to march through data
  inData->GetIncrements(inInc0, inInc1, inInc2); 
  self->GetInput()->GetWholeExtent(inImageMin0, inImageMax0, inImageMin1,
				   inImageMax1, inImageMin2, inImageMax2);
  cout << inImageMin2 << " " << inImageMax2 << endl;
  outData->GetIncrements(outInc0, outInc1, outInc2); 
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];
  numComps = outData->GetNumberOfScalarComponents();

  // Lauren do this like regular filter?  (change inheritance?)
  // Neighborhood around current voxel
  self->GetRelativeHoodExtent(hoodMin0, hoodMax0, hoodMin1, 
			      hoodMax1, hoodMin2, hoodMax2);
  cout << hoodMin0<<hoodMax0<<hoodMin1 <<hoodMax1 << hoodMin2 <<hoodMax2 << endl;
  // Set up mask info
  //  maskPtr = (unsigned char *)(self->GetMaskPointer());
  // Lauren use to loop through hoodCopy
  // Lauren fix this!
  self->GetMaskIncrements(maskInc0, maskInc1, maskInc2);

  // in and out should be marching through corresponding pixels.
  inPtr = (T *)(inData->GetScalarPointer(outMin0, outMin1, outMin2));

  target = (unsigned long)(numComps*(outMax2-outMin2+1)*
			   (outMax1-outMin1+1)/50.0);
  target++;

  // Lauren  ??  short??
  // scale factor (edges go from 1 to this number)
  int maxEdge = self->GetMaxEdgeWeight();

  // multipliers set by user
  //float diff = self->GetDifference();
  //float inpix = self->GetInsidePixel();
  //float outpix = self->GetOutsidePixel();

  // temporary storage of features computed at a voxel
  float features[self->GetNumberOfFeatures()];

  // for temporary storage of neighbors for calculations
  float n[self->GetKernelSize()[0]*self->GetKernelSize()[1]*self->GetKernelSize()[2]];

  // loop through components
  for (outIdxC = 0; outIdxC < numComps; ++outIdxC)
    {
      // loop through pixels of output
      outPtr2 = outPtr;
      inPtr2 = inPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++)
	{
	  outPtr1 = outPtr2;
	  inPtr1 = inPtr2;
	  for (outIdx1 = outMin1; 
	       !self->AbortExecute && outIdx1 <= outMax1; outIdx1++)
	    {
	      if (!id) {
		if (!(count%target))
		  self->UpdateProgress(count/(50.0*target));
		count++;
	      }
	      outPtr0 = outPtr1;
	      inPtr0 = inPtr1;
	      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
		{

		  // ---- Neighborhood Operations ---- //

		  // Default output equal to max edge value
		  *outPtr0 = maxEdge;

		  // make sure entire kernel is within boundaries.
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

		      //Lauren leftover from testing 5 like it's 3.
//  		  if (outIdx0 - 1 >= inImageMin0 &&
//  		      outIdx0 + 1 <= inImageMax0 &&
//  		      outIdx1 - 1 >= inImageMin1 &&
//  		      outIdx1 + 1 <= inImageMax1)
//  		    {
		      // loop through neighborhood pixels.
		      // Lauren copy them into n, neighborhood temp array
		      hoodPtr2 = inPtr0 + inInc0*hoodMin0 + inInc1*hoodMin1 
			+ inInc2*hoodMin2;
		      hoodCopyPtr2 = n;
		      for (hoodIdx2 = hoodMin2; hoodIdx2 <= hoodMax2; ++hoodIdx2)
			{
			  hoodPtr1 = hoodPtr2;
			  hoodCopyPtr1 = hoodCopyPtr2;
			  for (hoodIdx1 = hoodMin1; hoodIdx1 <= hoodMax1; ++hoodIdx1)
			    {
			      hoodPtr0 = hoodPtr1;
			      hoodCopyPtr0 = hoodCopyPtr1;
			      for (hoodIdx0 = hoodMin0; hoodIdx0 <= hoodMax0; ++hoodIdx0)
				{
				  *hoodCopyPtr0 = (float)*hoodPtr0;
				  
				  hoodPtr0 += inInc0;
				  hoodCopyPtr0 += maskInc0;
				}//for0
			      hoodPtr1 += inInc1;
			      hoodCopyPtr1 += maskInc1;
			    }//for1
			  hoodPtr2 += inInc2;
			  hoodCopyPtr2 += maskInc2;
			}//for2  
		  
		      // this filter assumes bel
		      // | out | in |
		      //       V
		      // rotate input for all bels.

		      // Compute various features.
		      
		      if (self->GetKernelSize()[0] == 3) 
			{
			  // The "in" features correspond to 
			  // the side of the bel with higher intensity.
			  // inpix-outpix = p-q
			  features[2] = n[3]-n[4];
			  features[3] = .333333*(n[3]+n[0]+n[6]-n[1]-n[4]-n[7]);
			  if (features[3] > 0)
			    {
			      // in pix magnitude = q
			      features[0] = n[4];
			      // out pix magnitude = p
			      features[1] = n[3];
			    }
			  else 
			    {
			      // in pix magnitude = p
			      features[0] = n[3];
			      // out pix magnitude = q
			      features[1] = n[4];
			    }
			      
			  // inpix-outpix = p-q
			  //features[2] = n[3]-n[4];
			  // gradient = (1/3)*(p+t+v-u-q-w)
			  features[3] = .333333*(n[3]+n[0]+n[6]-n[1]-n[4]-n[7]);
			  // gradient = (1/2)*(p+t/2+v/2 -u-q/2-w/2)
			  features[4] = .5*(n[3]+n[0]/2+n[6]/2
					    -n[1]-n[4]/2-n[7]/2);
			  // gradient = (1/4)*(p-u + t-q + p-w + v-q)
			  features[5] = .25*(n[3]-n[1] + n[0]-n[4] + 
					     n[3]-n[7] + n[6]-n[4]);

			}
		      else
			if (self->GetKernelSize()[0] == 5) 
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

#ifdef LIVEWIRE_TRAINING_EXPERIMENT		      
		      // Don't do this for an entire dataset!
		      // Output them to a file. 
		      for (int i=0;i<self->GetNumberOfFeatures();i++)
			{
			  file << features[i] << ' ';
			}
		      file << endl;
#endif
		      
		      // Lauren fix this!

		      // convert features to an edge weight
#define callMemberFunction(object,ptrToMember)  ((object).*(ptrToMember)) 
		      featureProperties *props;
		      float sum = 0;
		      for (int i=0;i<self->GetNumberOfFeatures();i++)
			{
			  //sum += GaussianCost(features[i], 0, 1);
			  props = self->GetFeatureSettings(i);
			  //(props->*Transform)();
			  //sum += (props->*Transform)(features[i]);
			  //sum += callMemberFunction(props,props.Transform);
			  
			  sum += GaussianC(features[i],props->TransformParams[0],props->TransformParams[1]);
			}

		      // each feature is between 0 and 1.  normalize sum to 1 
		      // then multiply by max edge cost.
		      *outPtr0 = (sum/self->GetNumberOfFeatures())*maxEdge;

		      if (*outPtr > maxEdge) 
			{
			  cout << "ERROR in vtkImageLWEdgeWeights: edge cost too high " << *outPtr << endl;
			}
		      
		    }
		  else
		    {
#ifdef LIVEWIRE_TRAINING_EXPERIMENT		      
		      // Output something to file if pix out of bounds
		      // (won't matter unless segmenting region right at edge of img)
		      for (int i=0;i<self->GetNumberOfFeatures();i++)
			{
			  file << 0 << ' ';
			}
		      file << endl;
#endif
		    }
		  // ---- End Neighborhood Operations ---- //
		  
		  inPtr0 += inInc0;
		  outPtr0 += outInc0;
		}//for0
	      inPtr1 += inInc1;
	      outPtr1 += outInc1;
	    }//for1
	  inPtr2 += inInc2;
	  outPtr2 += outInc2;
	}//for2
      inPtr++;
      outPtr++;
    }

#ifdef LIVEWIRE_TRAINING_EXPERIMENT
  file.close();
#endif

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
void vtkImageLiveWireEdgeWeights::ThreadedExecute(vtkImageData *inData, 
						 vtkImageData *outData,
						 int outExt[6], int id)
{

  // Lauren allocate additional outputs here.
  // done like this in vtkImageToImageFilter:
  //    vtkImageData *output = this->GetOutput();
  
  //    output->SetExtent(output->GetUpdateExtent());
  //    output->AllocateScalars();
  //    this->Execute(this->GetInput(), output);

  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  
  switch (inData->GetScalarType())
    {
    case VTK_DOUBLE:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (double *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_FLOAT:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (float *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_LONG:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (long *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_INT:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (int *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_INT:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (unsigned int *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_SHORT:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (short *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (unsigned short *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_CHAR:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (char *)(inPtr), 
					outData, outExt, id);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkImageLiveWireEdgeWeightsExecute(this, inData, (unsigned char *)(inPtr), 
					outData, outExt, id);
      break;
    default:
      vtkErrorMacro(<< "Execute: Unknown input ScalarType");
      return;
    }
}
