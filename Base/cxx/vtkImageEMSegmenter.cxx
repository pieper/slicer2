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
#include "vtkImageEMSegmenter.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkImageEMSegmenter* vtkImageEMSegmenter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMSegmenter");
  if(ret)
  {
    return (vtkImageEMSegmenter*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMSegmenter;
}

//----------------------------------------------------------------------------
vtkImageEMSegmenter::vtkImageEMSegmenter()
{
  this->NumClasses = 2;       // Number of Classes - has to be set to 0 bc currently nothing is set
  this->NumIter = 1;          // Number of EM-MRF Iterations
  this->NumRegIter = 1;       // Number of Regularizing Iterations
  this->Alpha = 0.7;          // 0 <= alpha <= 1. Be carefull - has great influence over outcome 
  this->SmoothingWidth = 11;  // Width for Gausian to regularize weights
  this->SmoothingSigma = 5;   // Sigma paramter for regularizing Gaussian
  this->StartSlice = 1;       // First Slide to be segmented
  this->EndSlice = 1;         // Last Slide to be segmented  
  this->ImgTestNo = -1;       // Segment an image test picture (-1 => No, > 0 =>certain Test pictures)
  this->ImgTestDivision = -1; // Number of divisions/colors of the picture
  this->ImgTestPixel = -1;    // Pixel lenght on one diviosn (pixel == -1 => max pixel length for devision)
  this->PrintIntermediateResults = 0 ;   // Print intermediate results in a Matlab File (No = 0, Yes = 1)
  this->PrintIntermediateSlice = 1 ;     // Print out the result of which slice  
  this->PrintIntermediateFrequency = 1 ; // Print out the result after how many steps
}

//----------------------------------------------------------------------------
void vtkImageEMSegmenter::PrintSelf(ostream& os)
{
}

// To chage anything about output us this executed before Thread

//----------------------------------------------------------------------------
//void vtkImageAccumulate::ExecuteInformation(vtkImageData *vtkNotUsed(input), 
//					    vtkImageData *output)
//{
//  output->SetWholeExtent(this->ComponentExtent); // size of output
//  output->SetOrigin(this->ComponentOrigin);
//  output->SetSpacing(this->ComponentSpacing);
//  output->SetNumberOfScalarComponents(1);
//  output->SetScalarType(VTK_INT);      // type of output float, integer 
//}



//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkImageEMSegmenterExecute(vtkImageEMSegmenter *self,vtkImageData *in1Data, T *in1Ptr,
				  vtkImageData *outData, T *outPtr,int outExt[6], int id)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;
  int rowLength;
  int StartSlice = self->get_StartSlice();
  int EndSlice = self->get_EndSlice();
  int StartEndSlice; 

  // find the region to loop over
  rowLength = (outExt[1] - outExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  maxY = outExt[3] - outExt[2] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
  maxZ = outExt[5] - outExt[4] + 1; // outExt[5/4] = Relative Maximum/Minimum Slice index  

  // Making sure values are ste correctly
  if (1 > StartSlice) StartSlice = 1;
  if (maxZ < EndSlice) EndSlice = maxZ; 
  else if (StartSlice > EndSlice) EndSlice = StartSlice -1;  
  StartEndSlice = EndSlice - StartSlice + 1;

  // Get increments to march through data 
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  // The Slices of the images -> dynaminc has to be that way otherwise can only use for very restricted number of ways 

  // 1.) Read thorugh beginning slides we do not want to read through for input and output
  for (idxZ = 1; idxZ < StartSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = 0;
	in1Ptr++;
	outPtr++;
      }
      in1Ptr += inIncY;
      outPtr += outIncY;
    }
    in1Ptr += inIncZ;
    outPtr += outIncZ; 
  }
  // 2.) Run algorithm
  self->vtkImageEMAlgorithm(in1Ptr, outPtr,rowLength,maxY,StartEndSlice, inIncY, inIncZ, outIncY, outIncZ);
  // 3.) Run through the end slides that were not segmented
  outPtr += ((rowLength+outIncY)*maxY + outIncZ)*StartEndSlice;      
  StartEndSlice = maxZ - EndSlice + 1;
  for (idxZ = 1; idxZ < StartEndSlice; idxZ++) {
    for (idxY = 0; idxY < maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
	*outPtr = 0;
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ; 
  }
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.

void vtkImageEMSegmenter::ThreadedExecute(vtkImageData *inData, vtkImageData *outData,int outExt[6], int id)
{
  void *inPtr;
  void *outPtr;
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
		<< ", outData = " << outData);
  

  if (inData == NULL)
    {
    vtkErrorMacro(<< "Input " << 0 << " must be specified.");
    return;
    }

  // Samson suggest:
  // make sure the input has only one component
  // set your output to be unsigned shorts.
  // make sure it works on a parallel machine
  // make sure your input and output sizes are the same
  // figure out how to use the output data directly rather than copying over.

  inPtr = inData->GetScalarPointerForExtent(outExt);
  outPtr = outData->GetScalarPointerForExtent(outExt);
  
  // I added ask Lauren
  if (inData->GetScalarType() != outData->GetScalarType()) {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
                  << ", must match out ScalarType " << outData->GetScalarType());
    return;
  }
  
  switch (inData->GetScalarType()) {
    vtkTemplateMacro7(vtkImageEMSegmenterExecute, this, inData, (VTK_TT *)(inPtr), 
                      outData, (VTK_TT *)(outPtr),outExt, id);
  default:
    vtkErrorMacro(<< "Execute: Unknown ScalarType");
    return;
  }
}


//----------------------------------------------------------------------------
// We need to create a special SetNumClass, bc whenver NumClasses are set, we 
// also have to set arrays which depend on the number of classes e.g. prob
//----------------------------------------------------------------------------

void vtkImageEMSegmenter::SetNumClasses(int NumberOfClasses)
{
  this->NumClasses = NumberOfClasses;
  // Delete Old instances if they existed
  this->Mu.Resize(this->NumClasses,-1.0);
  this->Sigma.Resize(this->NumClasses,-1.0); 
  this->Prob.Resize(this->NumClasses,-1.0);
  this->MrfParams.Resize(6,this->NumClasses,this->NumClasses,-1.0);
  this->Label.Resize(this->NumClasses,-1.0);
}
//---------------------------------------------------------------------
// Checks of all paramters are declared  
// 1  = eveything correct
// -1 = Dimension of Volume not correct
// -2 = Not all Mu defined correctly
// -3 = Not all Sigma defined correctly
// -4 = Not all Prob defined correctly
// -5 = Not all MrfParams defined correctly
// -6 = Not all Classes have labels defined

int vtkImageEMSegmenter::checkValues(vtkImageEMMatrix3D Volume)
{
  int i=0;
  if (Volume.get_dim(1) != (this->EndSlice - this->StartSlice + 1)) {
    cout << "vtkImageEMSegmenter:checkValues:  Volume does not have the right dimension !" << endl;
    return -1;
  }
  while (i < this->NumClasses) {
    i++;
    if (this->Mu[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Mu[" << i <<"] = " << this->Mu[i] << " must be greater than 0!" << endl;
      return -2;
    }
    if (this->Sigma[i] < 0)  {
      cout << "vtkImageEMSegmenter:checkValues:  Sigma[" << i <<"] = " << this->Sigma[i] << " is not greater than 0!" << endl;
      return -3;
    }
    if ((this->Prob[i] < 0) || (this->Prob[i] > 1.0)) {
      cout << "vtkImageEMSegmenter:checkValues:  Prob[" << i <<"] = " << this->Prob[i] << " is not between 0 and 1!" << endl;
      return -4;
    }
    if (this->Label[i] < 0) {
      cout << "vtkImageEMSegmenter:checkValues:  Color[" << i <<"] = " << this->Label[i] << " is not defined" << endl;
      return -6;
    }
    // Kilian: For testing log uncommented
    //  for (j = 1; j <= this->NumClasses; j++) {
//        for (k = 1; k <= 6; k++) {
//  	if ((this->MrfParams[k][j][i] < 0) || (this->MrfParams[k][j][i] > 1)) {
//  	  cout << "vtkImageEMSegmenter:checkValues:  MrfParams[" << k <<"] [" << j <<"] [" << i <<"] = " << this->MrfParams[k][j][i] 
//  	       << " is not between 0 and 1!" << endl;
//  	  return -5;
//  	}
//        }
//      }
  }
  return 1;
}



//----------------------------------------------------------------------------
// Define the arrays 
//----------------------------------------------------------------------------
void vtkImageEMSegmenter::SetProbability(double prob, int index) {
  if ((index<1) || (index > this->NumClasses) ||
      (prob < 0) || (prob > 1)) {
    vtkImageEMError("vtkImageEMSegmenter::SetProbability: Incorrect input");
    return;
  }
  this->Prob[index] = prob;
}

void vtkImageEMSegmenter::SetMu(double mu, int index){
  if ((index<1) || (index > this->NumClasses) ||
      (mu < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetMu: Incorrect input");
    return;
  }
  this->Mu[index] = mu;
}

void vtkImageEMSegmenter::SetSigma(double sigma, int index){
  if ((index<1) || (index > this->NumClasses) || (sigma < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetSigma: Incorrect input");
    return;
  }
  this->Sigma[index] = sigma;
}

void vtkImageEMSegmenter::SetLabel(int label, int index){
  if ((index<1) || (index > this->NumClasses) || (label < 0)) {
    vtkImageEMError("vtkImageEMSegmenter::SetColor: Incorrect input");
    return;
  }
  this->Label[index] = label;
}

void vtkImageEMSegmenter::SetMarkovMatrix(double value, int k, int j, int i) {
  if ((j<1) || (j > this->NumClasses) ||
      (i<1) || (i > this->NumClasses) ||
      (k<1) || (k > 6))
   {  
     // Kilian: Test Log Prob
     //  ||
     // (value < 0) || (value > 1)) {
    vtkImageEMError("vtkImageEMSegmenter::SetMarkovMatrix: Incorrect input");
    return;
  }
  this->MrfParams[k][j][i] = value;
}; 

//----------------------------------------------------------------------------
// The EM Algorithm based on Tina Kapur PhD Thesis and the Matlab 
// implementation from Carl-Fredrik Westin, Mats Bjornemo and Anders Brun
template <class T>
  void vtkImageEMSegmenter::vtkImageEMAlgorithm(T *in1Ptr, T *outPtr,int imgX,int imgY, int  NumSlices, int inIncY, int inIncZ, int outIncY, int outIncZ ) {
  cout << "vtkImageEMAlgorithm: Initialize Variables" << endl;
  // ------------------------------------------------------
  // General Variables 
  // ------------------------------------------------------
  int i,j,k,l,index;
  // Image is actually a 3Dim Array im(Y,X,Slice) 
  // imS=[imgY,imgX,NumSlices] 
  // int NumSlices = this->EndSlice - this->StartSlice + 1;
  int imgYPlus= imgY+1, imgXPlus= imgX+1, NumSlicesPlus= NumSlices +1;
  int imgXY   = imgX*imgY; 
  int imgXYPlus = imgXY +1;
  int imgProd = imgXY*NumSlices;
  int imgProdPlus = imgProd+1;
  int NumClassPlus = this->NumClasses+1;
  int iter,regiter;
 
  // ------------------------------------------------------
  // Read The Image - E Step Variable
  // ------------------------------------------------------
  // I like to see my pictures the right way around !
  int ImageMax = ((int)*in1Ptr);
  double *Y = new double[imgProd];
  Y--;
  for (k = 0; k < NumSlices ; k++) { 
    for (j = imgY; j > 0; j--) {
      index  =  imgXY*k+j;
      for (i = 0; i < imgX; i++) {
	Y[index] = log(double(* in1Ptr) + 1);
        if (int(*in1Ptr) > ImageMax) ImageMax = int(*in1Ptr);
	index += imgY;
	in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }

  // ------------------------------------------------------
  // Setting up Class Distribution 
  // Calculate the mean of the log(greyvalue+1), 
  // currently we have the mean over the grey scale values !
  // ------------------------------------------------------
  int iMax = ImageMax+750;
  vtkImageEMVector MuLog(this->NumClasses,0.0) ; 
  vtkImageEMVector SigmaLog(this->NumClasses,0.0) ; 
  vtkImageEMVector ProbSum(this->NumClasses,0.0);
  vtkImageEMVector LogPlus(iMax) ;
  vtkImageEMMatrix ProbMatrix(this->NumClasses,iMax);
  // The following is the same in Matlab as sum(p.*log(x+1)) with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 1; i < iMax; i++) {
    LogPlus[i] = log(i);
    for (k=1; k < NumClassPlus; k++) {
      ProbMatrix[k][i] = Gauss(i-1,this->Mu[k],this->Sigma[k]);
      MuLog[k] +=  ProbMatrix[k][i]*LogPlus[i];
      ProbSum[k] += ProbMatrix[k][i];
    }
  } 
  // Normalize Mu over psum  
  for (k=1; k < NumClassPlus; k++) {
      MuLog[k] /= ProbSum[k];
  }

  // The following is the same in Matlab as sqrt(sum(p.*(log(x+1)-mulog).*(log(x+1)-mulog))/psum)
  // with x =[0:iMax-1] and p =Gauss(x,mu,sigma) 
  for (i = 1; i < iMax; i++) {
    for (k=1; k < NumClassPlus; k++)
      SigmaLog[k] +=  ProbMatrix[k][i]*pow(LogPlus[i]-MuLog[k],2);
  } 
  // Take the sqrt
  for (k=1; k < NumClassPlus; k++) {
      SigmaLog[k] =  sqrt(SigmaLog[k] / ProbSum[k]);
  }

  // Calculate the inverse variance
  vtkImageEMVector ivar(this->NumClasses);
  for (i=1;i < NumClassPlus;i++) {
    ivar[i] = 1/(SigmaLog[i]*SigmaLog[i]); // inverse variance, needed in emseg
  }

  // ------------------------------------------------------------
  // EM-MF Variables 
  // ------------------------------------------------------------
  vtkImageEMMatrix3D b_m(NumSlices,imgY,imgX,0.0);      // bias for emseg+mrf
  vtkImageEMMatrix   w_m(imgProd,this->NumClasses,0.0); // weights or posteriors at each iteration
  vtkImageEMVector cY_M(imgProd);                       // cY_M correct log intensity
  // ------------------------------------------------------------
  // MF Step Variables 
  // ------------------------------------------------------------
  vtkImageEMVector NormVe(6);
  double normRow;                                  // Norm the rows  
  vtkImageEMVector wxp(this->NumClasses),wxn(this->NumClasses),wyn(this->NumClasses),wyp(this->NumClasses),
                   wzn(this->NumClasses),wzp(this->NumClasses);
  double mp;  

  // ------------------------------------------------------------
  // M Step Variables 
  // ------------------------------------------------------------
  double lbound = (-(this->SmoothingWidth-1)/2); // upper bound = - lbound
  vtkImageEMVector skern(this->SmoothingWidth); 
  for (i=0; i < this->SmoothingWidth; i++) {
     skern[i+1] = Gauss(lbound + i ,0,this->SmoothingSigma);
  }
  vtkImageEMMatrix3D r_m(NumSlices,imgY,imgX),   // weighted residuals
                     iv_m(NumSlices,imgY,imgX);  // weighted inverse covariances

  // ------------------------------------------------------------
  // Start Algorithm 
  // ------------------------------------------------------------
  for (iter=1; iter <= this->NumIter;iter++){
    cout << "vtkImageEMAlgorithm: "<< iter << ". Estep " << endl;
    // -----------------------------------------------------------
    // E-Step
    // -----------------------------------------------------------
    // cY_m = abs(Y - b_m(:));  // corrected log intensities
    for (k=1; k< NumSlicesPlus; k++) {
      index = (k-1)*imgXY;
      for (i=1; i < imgXPlus; i++) {
        for (j=1; j < imgYPlus; j++)
	  cY_M[index+j] = abs(Y[index+j] - b_m[k][j][i]);

        index +=imgY;
      }
    }
    // Ininitialize first iteration
    // This is the EM Algorithm with out MF Part -> The Regulizing part is the part where 
    // the MF part is added
 
    if (iter == 1) { 
      for (k=1; k < imgProdPlus; k++) {  
        normRow = 0;
        for (j=1; j < NumClassPlus; j++) {
	  w_m[k][j] = this->Prob[j]*(exp(cY_M[k])*Gauss(cY_M[k], MuLog[j],SigmaLog[j]));
          normRow += w_m[k][j];
	}
        // Normalize Rows and find Maxium of every Row and write it in Matrix
        for (j=1; j < NumClassPlus; j++) {w_m[k][j] /= normRow;}
      }
    }
    // -----------------------------------------------------------
    // regularize weights using a mean-field approximation
    // -----------------------------------------------------------
    for (regiter=1; regiter <= this->NumRegIter; regiter++) {
      cout << "vtkImageEMAlgorithm: "<< regiter << ". MFA Iteration" << endl;
      // -----------------------------------------------------------
      // neighbouring field values
      // -----------------------------------------------------------
      // The different Matrisses are defined as follow:
      // Matrix[i] = Neighbour in a certain direction
      // In Tcl TK defined in this order: West North Up East South Down 
      // this->MrfParams[i] :  
      // i = 4 pixel and east neighbour
      // i = 1 pixel and west neighbour
      // i = 5 pixel and south neighbour
      // i = 2 pixel and north neighbour
      // i = 3 pixel and previous neighbour (up)
      // i = 6 pixel and next neighbour (down)
      index = 0; 
      for(j = 0; j<  NumSlices; j++) {
        for(l = 1 ; l< imgXYPlus; l++) {
	  NormVe[1] =NormVe[2]= NormVe[3]=NormVe[4]=NormVe[5]=NormVe[6]=0;
	  index ++;
	  for (i=1;i<NumClassPlus ;i++){
	    wxp[i]=wxn[i]=wyn[i]=wyp[i]=wzn[i]=wzp[i]=0;

	    for (k=1;k<NumClassPlus ;k++){
	      // f(i,j-1,k)
              if (l > imgY)           wxn[i] += w_m[index-imgY][k]*this->MrfParams[4][k][i];
	      else                    wxn[i] += w_m[index][k]*this->MrfParams[4][k][i]; 
	      // f(i,j+1,k)
	      if (l < imgXYPlus-imgY) wxp[i] += w_m[imgY+index][k]*this->MrfParams[1][k][i]; 
	      else                    wxp[i] += w_m[index][k]*this->MrfParams[1][k][i];
	      // f(i-1,j,k)
	      if ((l-1)%imgY)         wyn[i] += w_m[index-1][k]*this->MrfParams[5][k][i];  
	      else                    wyn[i] +=  w_m[index][k]*this->MrfParams[5][k][i];   	      	      
	      // f(i+1,j,k)
	      if (l%imgY)             wyp[i] += w_m[1+index][k]*this->MrfParams[2][k][i];
	      else                    wyp[i] += w_m[index][k]*this->MrfParams[2][k][i];
	      // f(i,j,k-1)
	      if (j > 0)              wzn[i] += w_m[index-imgXY][k]*this->MrfParams[6][k][i]; 
	      else                    wzn[i] += w_m[index][k]*this->MrfParams[6][k][i];  
	      // f(i,j,k+1)
	      if (j < (NumSlices -1)) wzp[i] += w_m[index+imgXY][k]*this->MrfParams[3][k][i]; 
	      else                    wzp[i] += w_m[index][k]*this->MrfParams[3][k][i]; 

	    }
	    NormVe[1] += wxn[i];
	    NormVe[2] += wxp[i];
	    NormVe[3] += wyn[i];
	    NormVe[4] += wyp[i];
	    NormVe[5] += wzn[i];
	    NormVe[6] += wzp[i];
	  }
	  // mp = (ones(prod(imS),1)*p).*(1-alpha + alpha*wxn).*(1- alpha +alpha*wxp)...
	  //.*(1- alpha + alpha*wyp).*(1-alpha + alpha*wyn).*(1- alpha + alpha*wzp).*(1-alpha + alpha*wzn);
	  // w have to be normalized !
          normRow = 0;
	  for (i=1; i<NumClassPlus; i++) {
	    mp = this->Prob[i]*
	                     (1-this->Alpha+this->Alpha*(wxp[i]/NormVe[2]))*(1-this->Alpha+this->Alpha*(wxn[i]/NormVe[1]))
			    *(1-this->Alpha+this->Alpha*(wyp[i]/NormVe[4]))*(1-this->Alpha+this->Alpha*(wyn[i]/NormVe[3]))
			    *(1-this->Alpha+this->Alpha*(wzp[i]/NormVe[6]))*(1-this->Alpha+this->Alpha*(wzn[i]/NormVe[5]));
	    // w_m(:,i) = mp(:,i).*(exp(cY_m).*Gauss(cY_m, mu(i)*ones(size(X)),sigma(i)));
	    // w_m = w_m ./ repmat(eps +sum(w_m,2),[1,num_classes]);
	    // mp is not normalized right now  to be normalized !
	      w_m[index][i] = mp*(exp(cY_M[index])*Gauss(cY_M[index], MuLog[i],SigmaLog[i]));
	      normRow += w_m[index][i];
	  }        
	  for (i=1; i<NumClassPlus; i++) {
	      w_m[index][i] /= normRow;       
	  }
	}
      }
    }

    if ((this->PrintIntermediateResults) && ((iter%this->PrintIntermediateFrequency) == 0)){
	cout << "vtkImageEMAlgorithm: Print intermediate result " << endl;
	this->DeterminLabelMap(outPtr, w_m, imgX,imgY, NumSlices,imgXY, NumClassPlus, outIncY, outIncZ);
	PrintMatlabGraphResults(iter,this->PrintIntermediateSlice,1, imgXY, imgY, imgX,outPtr, outIncY, outIncZ, w_m, b_m);
	cout << "vtkImageEMAlgorithm: Return to Algorithm " << endl;
    } 

    if (iter < this->NumIter) {
      cout << "vtkImageEMAlgorithm: Mstep " << endl;
      // -----------------------------------------------------------
      // M-step
      // -----------------------------------------------------------
      // compute weighted residuals 
      // r_m  = (w_m.*(repmat(cY_M,[1 num_classes]) - repmat(mu,[prod(imS) 1])))*(ivar)';
      // iv_m = w_m * ivar';
      index = 0;
      for (k = 1; k<NumSlicesPlus;k++){
	for (j = 1; j<imgXPlus;j++){
	  for (i = 1; i<imgYPlus;i++){
	    index ++;
	    r_m[k][i][j] = 0;
	    iv_m[k][i][j] =0;
	    for (l=1; l<NumClassPlus; l++) {
	      r_m[k][i][j] += w_m[index][l]*(cY_M[index] - MuLog[l])*ivar[l];
	      iv_m[k][i][j] += w_m[index][l] * ivar[l];
	    }
	  }
	}
      }
  
      //  smooth residuals and inv covariances - 3D
      // w(k) = sum(u(j)*v(k+1-j))
      r_m.smoothConv(skern);
      iv_m.smoothConv(skern);

      // estimate the smoothed bias
      // transform r (smoothed weighted residuals) by iv (smoother inv covariances)
      // b_m = r_m./iv_m ;
      for (k = 1; k<NumSlicesPlus;k++){
	for (i = 1; i<imgYPlus;i++){
	  for (j = 1; j<imgXPlus;j++){
	    if (!iv_m[k][i][j]) iv_m[k][i][j]= 2.2204e-14;
	    b_m[k][i][j] = r_m[k][i][j] / iv_m[k][i][j];
	  }
	}      
      }
    } else {
      // -----------------------------------------------------------
      // Copy results to Image
      // -----------------------------------------------------------
      // Find out the maximum propability assigned to a certain class 
      // and assign that pixel to that class
      if ((!this->PrintIntermediateResults) || ((iter%this->PrintIntermediateFrequency) != 0))
	this->DeterminLabelMap(outPtr, w_m, imgX,imgY, NumSlices,imgXY, NumClassPlus,outIncY, outIncZ);
    }    
  }
  Y++;
  delete []Y;
  cout << "vtkImageEMAlgorithm: Finished " << endl;

}
 
inline double vtkImageEMSegmenter::Gauss(double x,double m,double s) {
  if  (s == 0 ) s = 1e-20;
  return (1 / (sqrt(2*3.14159265358979)*s)*exp(-(x-m)*(x-m)/(2*pow(s,2))));
}
 
void vtkImageEMSegmenter::TestConv() { 
  // vtkImageEMVector skern(this->SmoothingWidth); 
  vtkImageEMVector skern(3); 
  int x=5,y=5,z=5;
  vtkImageEMMatrix3D r_m3D(z,y,x,0.0), resultY(z,y,x,0.0), resultX(z,y,x,0.0); 

  int i,k,l;
  for (k=1; k <=z; k++) {
    for (i=1; i <=y; i++)
      for (l=1; l <=x; l++)
      r_m3D[k][i][l] = 1.0; 
  }
  for (i=1; i <4; i++)
    skern[i] = i;
  
  //for (i=0; i < this->SmoothingWidth; i++) {
  //   skern[i+1] = Gauss(lbound + i ,0,this->SmoothingSigma);
  //}

  // First: convolut in Y Direction 
  cout << "Y Convolution" << endl;
  for (i = 1; i <= z; i++){
    resultY[i].conv(r_m3D[i],skern); 
    resultY[i].PrintMatrix();
  }
  cout << "X convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    resultX[i].convT(resultY[i],skern); 
    resultX[i].PrintMatrix();
  }
  // Third: in Z direction
  resultY.conv(resultX,skern);
  cout << "Z convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    resultY[i].PrintMatrix();
  }
  r_m3D.smoothConv(skern);
  cout << "Alltogether convolution" << endl;
  // Second: convolut in X Direction 
  for (i = 1; i <= z; i++) {
    r_m3D[i].PrintMatrix();
  }
}

// -----------------------------------------------------------
// Print out results into a file "EMSegmResult<name>.m"
// Results can be display with Matlab Program listed below 
// -----------------------------------------------------------
template <class T>  void vtkImageEMSegmenter::PrintMatlabGraphResults(int iter,int slice,int FullProgram,int imgXY, int imgY, int imgX,T *outPtr,int outIncY, int outIncZ, vtkImageEMMatrix w_m,vtkImageEMMatrix3D b_m) {
  char filename[40];
  char weightname[40];
  int appendFlag = 0;
  int y,x,c, index;

  vtkImageEMVector weight(imgXY);
  vtkImageEMMatrix ind_Matrix (imgY,imgX);

  // Go to the position of 'slide'
  outPtr += ((imgX+outIncY)*imgY + outIncZ)*(slice - 1);
  imgX++;
  for (y = imgY; y > 0; y--) {
    for (x = 1; x < imgX; x++) {
	ind_Matrix[y][x] = double(* outPtr);
	outPtr++;
      }
      outPtr += outIncY;
  }

  sprintf(filename,"EMSegmResult%dImage.m",iter);
  ind_Matrix.WriteMatrixToFile(filename,"Image");  

  for (c = 1; c <= w_m.get_dim(2); c++ ) {
    weight.setVector(w_m,imgXY*(slice-1)+1,imgXY*slice,c);
    sprintf(weightname,"weight(%d,:)",c);
    sprintf(filename,"EMSegmResult%dWeight%d.m",iter,c);
    weight.WriteVectorToFile(filename,weightname);
  }
  sprintf(filename,"EMSegmResult%db_m.m",iter);
  b_m[slice].WriteMatrixToFile(filename,"b_m");

  if (FullProgram) {
    sprintf(filename,"EMSegmResult%d.m",iter);
    FILE *f = (strcmp(filename,"-"))?fopen(filename,((appendFlag)?"a":"w")):stdout;
    if ( f == NULL ) {
      cerr << "Could not open file " << filename << "\n";
      return;
    }

    fprintf(f,"%% (c) Copyright 2001 Massachusetts Institute of Technology\n");
    fprintf(f,"%% Result of the %d iteration of the EM Algorithm\n",iter);
    fprintf(f,"EMSegmResult%dImage\n",iter);
    fprintf(f,"figure(%d1)\n",iter);
    fprintf(f,"set(%d1, 'position', [10 445 300 300]);\n",iter);
    fprintf(f,"imagesc([Image(:,:)]);\n");
    fprintf(f,"title('Label Map - Iteration %d');\n", iter);
    // fprintf(f,"colormap([0 0 1; 0 1 0; 1 0 0; 1 1 0;]);\n");
    fprintf(f,"colormap('default');\n");
    fprintf(f,"\n");

    fprintf(f,"figure(%d2);\n",iter);
    fprintf(f,"set(%d2, 'position', [10 10 1210 360]);\n",iter);
    fprintf(f,"colormap(gray); \n");
    fprintf(f,"dwm=[];\n");
    for (c = 1; c <= w_m.get_dim(2); c++ ) 
      fprintf(f,"EMSegmResult%dWeight%d\n",iter,c); 
    fprintf(f,"for t=1:%d\n",w_m.get_dim(2));
    fprintf(f,"  temp_m=reshape(weight(t,:),[%d %d]);\n",imgY, imgX); 
    fprintf(f,"  dwm = [dwm,  temp_m(:,:)];\n");
    fprintf(f,"end\n");
    fprintf(f,"imagesc([dwm]);\n");
    fprintf(f,"title('Weights - Iteration %d');\n", iter);
    fprintf(f,"\n");

    fprintf(f,"EMSegmResult%db_m\n",iter);
    fprintf(f,"figure(%d3);\n",iter);
    fprintf(f,"set(%d3, 'position', [600 445 300 300]);\n",iter);
    fprintf(f,"colormap(gray);\n"); 
    fprintf(f,"imagesc(b_m(:,:));\n");
    fprintf(f,"title('Bias - Iteration %d');\n",iter);
    fflush(f);
    fclose(f);
  }
}

template <class T> void vtkImageEMSegmenter::DeterminLabelMap(T *outPtr, vtkImageEMMatrix w_m, int imgX,int imgY, int  NumSlices, int imgXY,  int NumClassPlus, int outIncY, int outIncZ) { 
  int idxZ,idxY, idxR,l,MaxProbIndex,index;
  for (idxZ = 0; idxZ < NumSlices ; idxZ++) {
    for (idxY = imgY; idxY > 0; idxY--) {
      index  = imgXY*idxZ+idxY;
      for (idxR = 0; idxR < imgX; idxR++) {
	MaxProbIndex = 1;
	for (l=2; l<NumClassPlus; l++) {
	  if (w_m[index][l] > w_m[index][MaxProbIndex]) MaxProbIndex = l;
	}
	*outPtr = (T) (this->Label[MaxProbIndex]);

	index += imgY;
	outPtr++;
      }
      outPtr += outIncY;
    }
    outPtr += outIncZ;
  }
}

