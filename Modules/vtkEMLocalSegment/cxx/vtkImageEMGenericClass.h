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
// .NAME vtkImageEMGenericClass
// Definnition of all variables owned by both vtkImageEMSuperClass and vtkImageEMClass
//                      vtkImageEMGenericClass
//                        /                  \  
//           vtkImageEMSuperClass <- - - vtkImageEMClass

#ifndef __vtkImageEMGenericClass_h
#define __vtkImageEMGenericClass_h 
  
#include <vtkEMLocalSegmentConfigure.h> 

#include "vtkSlicer.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkImageData.h"
#include "vtkOStreamWrapper.h"

#ifndef EM_VTK_OLD_SETTINGS
#if (VTK_MAJOR_VERSION == 4 && (VTK_MINOR_VERSION >= 3 || (VTK_MINOR_VERSION == 2 && VTK_BUILD_VERSION > 5)))
  #define EM_VTK_OLD_SETTINGS 0 
#else
  #define EM_VTK_OLD_SETTINGS 1
#endif
#endif

// For Super Class and sub class
enum classType {CLASS, SUPERCLASS};

//BTX
// Class for  capturing different protocols throughout the segmentation process
class  VTK_EMLOCALSEGMENT_EXPORT ProtocolMessages {
public:
  int GetFlag() {return this->Flag;}
  char* GetMessages();
  void ResetParameters();
  // This is for programs that use a constant char as input
  void AddMessage(const char* os);

  ~ProtocolMessages() { this->DeleteMessage();}
  ProtocolMessages() {this->Message = new vtkOStrStreamWrapper; this->Flag = 0;}

  vtkOStrStreamWrapper *Message;
  int Flag; 
private: 
  void DeleteMessage();
};

// Needed for convenience so we can just enter things with << 

#define vtkEMAddMessageNoOutput(MessageNoOutputPtr, xout)        \
   {                                                          \
     vtkOStreamWrapper::EndlType endl;                        \
     vtkOStreamWrapper::UseEndl(endl);                        \
     MessageNoOutputPtr->Message->rdbuf()->freeze(0);         \
     (*MessageNoOutputPtr->Message) <<  xout << "\n";         \
     MessageNoOutputPtr->Flag =  1;                           \
   }


#define vtkEMAddMessage(output,MessagePtr, x)                 \
   {                                                          \
     vtkEMAddMessageNoOutput(MessagePtr, x)                   \
     output << x << "\n";                                     \
   }

// Message is not printed out 
#define vtkEMJustAddMessage(x) {        \
   vtkEMAddMessageNoOutput((&this->ErrorMessage), x) ; \
 }

#define vtkEMAddErrorMessage(x) {\
   vtkEMAddMessage(cerr, (&this->ErrorMessage), "- Error: " << x) ; \
 }

#define vtkEMAddErrorMessageSelf(x) {\
   vtkEMAddMessage(cerr,self->GetErrorMessagePtr(), "- Error: " << x); \
}

#define vtkEMAddWarningMessage(x) {\
   vtkEMAddMessage(cout, (&this->WarningMessage), "- Warning: " << x) ; \
 }

#define vtkEMAddWarningMessageSelf(x) {\
   vtkEMAddMessage(cout,self->GetWarningMessagePtr(), "- Warning: " << x); \
}
//ETX


class VTK_EMLOCALSEGMENT_EXPORT vtkImageEMGenericClass : public vtkImageMultipleInputFilter
{
  public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImageEMGenericClass *New();
  vtkTypeMacro(vtkImageEMGenericClass,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Global Tissue Probability
  vtkSetMacro(TissueProbability, double);
  vtkGetMacro(TissueProbability, double);

  // Description:
  // How much influence should the LocalPriorData have in the segmentation process 
  // 0   = no Probability data is used => if class is from type EMClass no ProbDataPtr will be loaded
  // 1.0 = complete trust is in ProbDataPtr => Whenever the ProbDataPtr = 0 => voxel will not be assigned to tissue class
  vtkSetMacro(ProbDataWeight,float);
  vtkGetMacro(ProbDataWeight,float);

  // Description
  // Scalar type of ProbData 
  vtkGetMacro(ProbDataScalarType,int);

  // Description:
  // Labels of classes. Please not that label for super classes are set automatically
  vtkGetMacro(Label,short);

  // Description:
  // Important for several input parameters such as size of LogCovariance
  // How much weight is given to each channel  
  // 0   = input is ignored 
  // 1.0 = input is fully considered  
  void SetInputChannelWeights(float val, int x);
  //BTX
  float* GetInputChannelWeights() {return this->InputChannelWeights;}
  //ETX

  // Description:
  // Number of input images for the segmentation - Has to be defined before defining any class specific setting 
  // Otherwise they get deleted
  // Be carefull: this is just the number of images not attlases, 
  // e.g. I have 5 tissue classes and 3 Inputs (T1, T2, SPGR) -> NumInputImages = 3
  void SetNumInputImages(int number);
  vtkGetMacro(NumInputImages,int);

  //Description:
  // Dimension of image Data 
  vtkGetVector3Macro(DataDim,int);

  //Description:
  // Spacing of image data
  vtkGetVector3Macro(DataSpacing, float);                 

  // Description:
  // Define the Segmentation areae.g. have the image of size 256x256x124
  // now you want to segment a box of 70x 70 x 70 around the center of the image
  // -> Define  SegmentationBoundaryMin((256-70)/2,(256-70)/2,(124-70)/2)
  //            SegmentationBoundaryMax((256+70)/2,(256+70)/2,(124+70)/2)  
  vtkSetVector3Macro(SegmentationBoundaryMin,int);
  vtkSetVector3Macro(SegmentationBoundaryMax,int);

  vtkGetVector3Macro(SegmentationBoundaryMin,int);
  vtkGetVector3Macro(SegmentationBoundaryMax,int);

  // Description:
  // Make sure older version get an error message
  void SetInputIndex(vtkImageData *image, int index)  {
    vtkErrorMacro(<< "In the new version SetInputIndex is disabled! Please look into vtkImageEM*Class.h to find out how to set the given parameters !");
    return;
  }

  // =============================
  // For Message Protocol
  // So we can also enter streams for functions outside vtk
  char* GetErrorMessages() {return this->ErrorMessage.GetMessages(); }
  int GetErrorFlag() {return  this->ErrorMessage.GetFlag();}
  void ResetErrorMessage() {this->ErrorMessage.ResetParameters();}

  //BTX
  ProtocolMessages* GetErrorMessagePtr(){return &this->ErrorMessage;}
  //ETX

  char* GetWarningMessages() {return this->WarningMessage.GetMessages(); }
  int GetWarningFlag() {return  this->WarningMessage.GetFlag();}
  void ResetWarningMessage() {this->WarningMessage.ResetParameters();}

  //BTX
  ProtocolMessages* GetWarningMessagePtr(){return &this->WarningMessage;}
  //ETX

  // Description:
  // Print out Weights (1 = Normal 2=as shorts normed to 1000)   
  vtkSetMacro(PrintWeights,int);
  vtkGetMacro(PrintWeights,int);

protected:
  vtkImageEMGenericClass();
  vtkImageEMGenericClass(const vtkImageEMGenericClass&) {};
  ~vtkImageEMGenericClass(){this->DeleteVariables(); UpdateBlubber->Delete();} 

  // Disable this function
  // vtkImageData* GetOutput() {return NULL;}

  void DeleteVariables(); 
  void operator=(const vtkImageEMGenericClass&) {};
  void ThreadedExecute(vtkImageData **inData, vtkImageData *outData,int outExt[6], int id) {};
 
  // Checks if all the parameters are setrectly
  // We do not have any input here
  //void  ExecuteData(vtkDataObject *) {this->Execute();}
  //void  Execute();
  void  ExecuteData(vtkDataObject *) ;

  float  ProbDataWeight;              // How much influence should the LocalPriorData have in the segmentation process 
  int    ProbDataScalarType;          // Scalar Type of ProbData

  double TissueProbability;           // Global Tissue Probability
  short  Label;
  float* InputChannelWeights;         // You can define different weights of input channels
  int    NumInputImages;              // Important for several input parameters such as size of LogCovariance 

  int    DataDim[3];                  // Dimension of image Data 
  float  DataSpacing[3];              // Spacing of image data
  int    SegmentationBoundaryMin[3];  // Boundary Box for the area of interest for the segmentation
  int    SegmentationBoundaryMax[3];  // Boundary Box for the area of interest for the segmentation 
  
  ProtocolMessages ErrorMessage;      // Lists all the error messges -> allows them to be displayed in tcl too 
  ProtocolMessages WarningMessage;    // Lists all the error messges -> allows them to be displayed in tcl too 
  vtkImageData *UpdateBlubber;        // We are just creating this parameter so that the Update Pipeline works properly 

  int PrintWeights;                   // Print out Weights (1 = Normal 2=as shorts normed to 1000)   

};

#endif











