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
#include "vtkImageEMLocalClass.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------
vtkImageEMLocalClass* vtkImageEMLocalClass::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMLocalClass");
  if(ret)
  {
    return (vtkImageEMLocalClass*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMLocalClass;

}


//----------------------------------------------------------------------------
vtkImageEMLocalClass::vtkImageEMLocalClass() { 
  this->LogMu               = NULL;
  this->LogCovariance       = NULL;

  this->ProbDataPtr         = NULL;
  this->ProbDataIncY        = -1; 
  this->ProbDataIncZ        = -1; 
  this->ProbDataWeight      = 0.0;
  
  this->ReferenceStandardPtr= NULL;

  // For Samson Stuff
  this->ShapeParameter      = 0.0;

  // Printing functions 
  this->PrintQuality        = 0;

  // Important to set it - otherwise set to 1 by default  
  this->vtkProcessObject::SetNumberOfInputs(0); 
}


//------------------------------------------------------------------------------
void vtkImageEMLocalClass::DeleteClassVariables(){
  if (this->LogCovariance) {
    for (int y=0; y < this->NumInputImages; y++) {
      delete[] this->LogCovariance[y];
    }
    delete[] this->LogCovariance;
  }
  
  if (this->LogMu) delete[] this->LogMu;
  
  this->LogCovariance       = NULL;
  this->LogMu               = NULL;

  this->ProbDataPtr         = NULL;
  this->ReferenceStandardPtr= NULL;
}

//----------------------------------------------------------------------------
void vtkImageEMLocalClass::PrintSelf(ostream& os,vtkIndent indent) { 
  int x,y;
  os << indent << "------------------------------------------ CLASS ----------------------------------------------" << endl;
  this->vtkImageEMGenericClass::PrintSelf(os,indent);  
  os << indent << "Shape Parameter:         " << this->ShapeParameter << endl;
  os << indent << "ProbDataPtr:             " << this->ProbDataPtr << endl;
  os << indent << "ProbDataIncY:            " << this->ProbDataIncY << endl;
  os << indent << "ProbDataIncZ:            " << this->ProbDataIncZ << endl;

  os << indent << "LogMu:                   ";
  for (x= 0 ; x < this->NumInputImages; x ++) os << this->LogMu[x] << " ";
  os<< endl;

  os << indent << "LogCovariance:           ";
  for (y= 0 ; y < this->NumInputImages; y ++) {
     for (x= 0; x < this->NumInputImages; x++)  os << this->LogCovariance[y][x] << " " ;
     if ( y < (this->NumInputImages-1)) os<< "| ";
  }
  os<< endl;

  os << indent << "ReferenceStandardPtr:    ";
  if (this->ReferenceStandardPtr) {
     os << this->ReferenceStandardPtr << endl;
  } else {os << "(None)" << endl;}

  os << indent << "PrintWeights:            " << this->PrintWeights << endl;
  os << indent << "PrintQuality:            " << this->PrintQuality << endl;  
}

//------------------------------------------------------------------------------
void vtkImageEMLocalClass::SetNumInputImages(int number) {
  int OldNumber = this->NumInputImages;
  this->vtkImageEMGenericClass::SetNumInputImages(number);
  if (OldNumber == number) return;
  this->DeleteClassVariables();

  if (number > 0 ) {
    // Create new space for variables
    this->LogMu               = new double[number];
    this->LogCovariance       = new double*[number];
    for (int z=0; z < number; z++) {
      this->LogCovariance[z] = new double[number];
    }
    for (int z= 0; z < number; z++) {
    this->LogMu[z] = -1;
    memset(this->LogCovariance[z], 0,number*sizeof(double));
    }
  } 
}

//------------------------------------------------------------------------------
void vtkImageEMLocalClass::SetLogMu(double mu, int x){
  if ((x<0) || (x >= this->NumInputImages) || (mu < 0)) {
    vtkEMAddErrorMessage("Error:vtkImageEMLocalClass::SetLogMu: Incorrect input");
    return;
  }
  this->LogMu[x] = mu;
}

//------------------------------------------------------------------------------
void vtkImageEMLocalClass::SetLogCovariance(double value, int y, int x){
  if ((y<0) || (y >= this->NumInputImages) || (x<0) || (x >= this->NumInputImages)) {
    vtkEMAddErrorMessage("Error:vtkImageEMLocalClass::SetLogCovariance: Incorrect input");
    return;
  }
  this->LogCovariance[y][x] = value;
}

//----------------------------------------------------------------------------
void vtkImageEMLocalClass::SetPrintQuality(int init) { 
    if ( (init < 0)  || (init > EMSEGMENT_NUM_OF_QUALITY_MEASURE)) {
      vtkEMAddErrorMessage("The parameter for PrintQuality has be between 0 and "<< EMSEGMENT_NUM_OF_QUALITY_MEASURE - 1 << "!");
      return;
    }
    this->PrintQuality = init;
}

//----------------------------------------------------------------------------
template <class T>
static void vtkImageEMLocalClassAssignProbDataPointer(vtkImageEMLocalClass *self, T *inProbDataPtr, int jump) {
  self->SetProbDataPtr((void*) (inProbDataPtr + jump));
}

//----------------------------------------------------------------------------
// if ImageIndex = 0 than we know it is a probability map
int vtkImageEMLocalClass::CheckAndAssignImageData(vtkImageData *inData, int outExt[6], int ImageIndex) {
  int DataIncY, DataIncZ, blub;
 
  // -----------------------------------------------------
  // 1.) Check Probability Maps
  // -----------------------------------------------------

  // For Mean Shape and EigenVectors we currently only accepts float 
  if (ImageIndex) {
    if (this->CheckInputImage(inData,VTK_FLOAT, ImageIndex+1,outExt)) return 0;
  } else {
    if (this->CheckInputImage(inData,inData->GetScalarType(), 1,outExt)) return 0;
  }

  inData->GetContinuousIncrements(outExt, blub, DataIncY, DataIncZ);
        
  int LengthOfXDim = outExt[1] - outExt[0] + 1 + DataIncY;
  int LengthOfYDim = LengthOfXDim*(outExt[3] - outExt[2] + 1) + DataIncZ;  
  int jump = (this->SegmentationBoundaryMin[0] - 1) + (this->SegmentationBoundaryMin[1] - 1) * LengthOfXDim
         + LengthOfYDim *(this->SegmentationBoundaryMin[2] - 1);

  int BoundaryDataIncY = LengthOfXDim - this->DataDim[0];
  int BoundaryDataIncZ = LengthOfYDim - this->DataDim[1] *LengthOfXDim;


  // =========================================
  // Assign Probability Data
  switch (inData->GetScalarType()) {
      vtkTemplateMacro3(vtkImageEMLocalClassAssignProbDataPointer, this, (VTK_TT*) inData->GetScalarPointerForExtent(outExt), jump);
  default:
      vtkEMAddErrorMessage("Execute: Unknown ScalarType");
      return 0;
  }
  this->ProbDataIncY = BoundaryDataIncY;
  this->ProbDataIncZ = BoundaryDataIncZ;
  this->ProbDataScalarType  = inData->GetScalarType();
    // Meanshape and EigenVectors have to be floats !

  return 1; 
}


//------------------------------------------------------------------------------
int vtkImageEMLocalClass::CheckInputImage(vtkImageData * inData,int DataTypeOrig, int num, int outExt[6]) {
  // Check if InData is defined 
  int inExt[6];
  float DataSpacingNew[3];
  if (inData == 0) {
    vtkEMAddErrorMessage("CheckInputImage: Input "<< num << " must be specified.");
    return 1;
  }

  // Check for Data Type if correct : Remember ProbabilityData all has to be of the same data Type
  if (DataTypeOrig != inData->GetScalarType()) {
    vtkEMAddErrorMessage("CheckInputImage: Input "<< num << " has wrong data type ("<< inData->GetScalarType() << ") instead of " 
          << DataTypeOrig << "! Note: VTK_FLOAT="<< VTK_FLOAT <<", VTK_SHORT=" << VTK_SHORT );
    return 1;
  }


  // Check for dimenstion of InData
  // Could be easily fixed if needed 
  inData->GetWholeExtent(inExt);
  if ((inExt[1] != outExt[1]) || (inExt[0] != outExt[0]) || (inExt[3] != outExt[3]) || (inExt[2] != outExt[2]) || (inExt[5] != outExt[5]) || (inExt[4] != outExt[4])) {
    vtkEMAddErrorMessage("CheckInputImage: Extension of Input Image " << num << ", " << inExt[0] << "," << inExt[1] << "," << inExt[2] << "," << inExt[3] << "," << inExt[4] << "," << inExt[5] 
                  << "is not alligned with output image "  << outExt[0] << "," << outExt[1] << "," << outExt[2] << "," << outExt[3] << "," << outExt[4] << " " << outExt[5]);
    return 1;
  }
  if (inData->GetNumberOfScalarComponents() != 1) { 
    vtkEMAddErrorMessage("CheckInputImage: This filter assumes input to filter is defined with one scalar component. Input Image "<< num  << " has " 
                          << inData->GetNumberOfScalarComponents() << " Can be easily changed !");
    return 1;
  }
  inData->GetSpacing(DataSpacingNew);
  if ((this->DataSpacing[0] !=  DataSpacingNew[0]) || (this->DataSpacing[1] !=  DataSpacingNew[1]) || (this->DataSpacing[2] !=  DataSpacingNew[2])) {
    vtkEMAddErrorMessage("CheckInputImage: Data Spacing of input images is unequal" );
    return 1;
  }
  // Kilian Check for orientation has to be done in TCL !!!!
  return 0;
}

//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageEMLocalClass::ExecuteData(vtkDataObject *)
{
  // cout << "Start vtkImageEMLocalClass::ExecuteData " << endl;
   vtkDebugMacro(<<"ExecuteData()");

   // ==================================================
   // Initial Check Parameters
   this->ResetWarningMessage();

   if (this->GetErrorFlag()) 
     vtkEMAddWarningMessage("The error flag for this module was set with the following messages (the error messages will be reset now:\n"<<this->GetErrorMessages());

   this->ResetErrorMessage();
   this->vtkImageEMGenericClass::ExecuteData(NULL);
   if (this->GetErrorFlag()) return;

   if (this->Label < 0) {
     vtkEMAddErrorMessage("Not all labels are defined for the classes");
     return ;
   } 
   for (int j = 0; j < this->NumInputImages; j++) {
     // Check Mu
     if (this->LogMu[j] < 0) {
       vtkEMAddErrorMessage("Mu[" << j <<"] = " << this->LogMu[j] << " for class with label " << this->Label <<" must be greater than 0!");
       return;
     } 
     for (int k=j+1; k <   NumInputImages; k++) {
       if (this->LogCovariance[j][k] != this->LogCovariance[k][j]) {
     vtkEMAddErrorMessage("Covariance must be symetric for class with label " << this->Label);
     return;
       }
     }
   }

   // ==================================================
   // Define Parameters

   int NumberOfRealInputs = this->vtkProcessObject::GetNumberOfInputs();

   // No inputs defined we do not need to do here anything
   if (NumberOfRealInputs == 0) return;
   // Redefine ImageRelatedClass Parameters   
   vtkImageData **inData  = (vtkImageData **) this->GetInputs();

   int FirstData = 0;
   while (FirstData< NumberOfRealInputs && !inData[FirstData])  FirstData++;
   if (FirstData == NumberOfRealInputs) {
    // This error should not be possible 
    vtkEMAddErrorMessage("No image data defined as input even though vtkProcessObject::GetNumberOfInputs > 0 !");
    return;
   }
 
   this->DataDim[0] = ( this->SegmentationBoundaryMax[0] - this->SegmentationBoundaryMin[0] +1)*inData[FirstData]->GetNumberOfScalarComponents();
   this->DataDim[1] = this->SegmentationBoundaryMax[1] - this->SegmentationBoundaryMin[1] + 1; // outExt[3/2] = Relative Maximum/Minimum Y index  
   this->DataDim[2] = this->SegmentationBoundaryMax[2] - this->SegmentationBoundaryMin[2] + 1;

   int Ext[6];
   inData[FirstData]->GetWholeExtent(Ext);
  
   int XDim= Ext[1] - Ext[0] + 1;
   int YDim= Ext[3] - Ext[2] + 1;

   if (!(this->DataDim[0] * this->DataDim[1] * this->DataDim[2])) {
      vtkEMAddErrorMessage("Input has no points!" );
      return;
   }

   memcpy(this->DataSpacing, inData[FirstData]->GetSpacing(),sizeof(float)*3);

   // ================================================== 
   // Load the images
   
   // Check and set Probability map 
   if (NumberOfRealInputs && inData[0]) {
     if (this->ProbDataWeight > 0.0) {
       if (!this->CheckAndAssignImageData(inData[0], Ext, 0)) return;
     }
   } else {
     if (this->ProbDataWeight > 0.0) {
       vtkEMAddErrorMessage("ProbDataWeight > 0.0 and no Probability Map defined !" );
       return;
     } else {
       vtkEMAddWarningMessage("No probability map is defined for class with Label " << this->Label);
     }
   }
}

