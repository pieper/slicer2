/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
// .NAME vtkMrmlSegmenterClassNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Segmenter nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

//                                                                    vtkMrmlSegmenterAtlasClassNode
//                                                                                  ||
//                                                                                  \/
//                                                                   |-> vtkMrmlSegmenterClassNode
//                                                                   |
//  vtkMrmlNode -> vtkMrmlSegmenterAtlasGenericClassNode -> vtkMrmlSegmenterGenericClassNode
//                                                                   |
//                                                                   |-> vtkMrmlSegmenterSuperClassNode
//                                                                                  /\
//                                                                                  ||
//                                                                   vtkMrmlSegmenterAtlasSuperClassNode

#ifndef __vtkMrmlSegmenterSuperClassNode_h
#define __vtkMrmlSegmenterSuperClassNode_h

#include "vtkMrmlSegmenterGenericClassNode.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>
#include "vtkMrmlSegmenterAtlasSuperClassNode.h"

class VTK_EMLOCALSEGMENT_EXPORT vtkMrmlSegmenterSuperClassNode : public vtkMrmlSegmenterGenericClassNode
{
public:
  static vtkMrmlSegmenterSuperClassNode *New();
  vtkTypeMacro(vtkMrmlSegmenterSuperClassNode,vtkMrmlNode);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Write the node's attributes to a MRML file in XML format
  void Write(ofstream& of, int indent);

  //--------------------------------------------------------------------------
  // Utility Functions
  //--------------------------------------------------------------------------

  // Description:
  // Copy the node's attributes to this object
  void Copy(vtkMrmlNode *node);

  int  GetNumClasses() {return AtlasNode->GetNumClasses();}
  void SetNumClasses(int init) {AtlasNode->SetNumClasses(init);}

  // Description:
  // Print out the result after how many steps  (-1 == just last result, 0 = No Printing, i> 0 => every i-th slice )
  int  GetPrintFrequency() {return AtlasNode->GetPrintFrequency();}
  void SetPrintFrequency(int init) {AtlasNode->SetPrintFrequency(init);}

  int  GetPrintBias() {return AtlasNode->GetPrintBias();}
  void SetPrintBias(int init) {AtlasNode->SetPrintBias(init);}

  int  GetPrintLabelMap() {return AtlasNode->GetPrintLabelMap();}
  void SetPrintLabelMap(int init) {AtlasNode->SetPrintLabelMap(init);}

 // Description:
  // Prints out the shape  cost at each voxel 
  vtkGetMacro(PrintShapeSimularityMeasure, int);
  vtkSetMacro(PrintShapeSimularityMeasure, int);

  // Description:
  // Prints out the number of voxels changed from last to this EM iteration
  vtkGetMacro(PrintEMLabelMapConvergence, int);  
  vtkSetMacro(PrintEMLabelMapConvergence, int);  

  // Description:
  // Prints out the difference in percent 
  vtkGetMacro(PrintEMWeightsConvergence, int);
  vtkSetMacro(PrintEMWeightsConvergence, int);

 // Description:
  // Prints out the number of voxels changed from last to this MFA iteration
  vtkGetMacro(PrintMFALabelMapConvergence, int);  
  vtkSetMacro(PrintMFALabelMapConvergence, int);  

  // Description:
  // Prints out the difference in percent 
  vtkGetMacro(PrintMFAWeightsConvergence, int);
  vtkSetMacro(PrintMFAWeightsConvergence, int);

  // Description:  
  // After which criteria should be stopped   
  // 0 = fixed iterations 
  // 1 = Absolut measure 
  // 2 = Relative measure 
  int  GetStopEMType() {return AtlasNode->GetStopEMType();}
  void SetStopEMType(int init) {AtlasNode->SetStopEMType(init);}
  
  // Description:  
  // What is the obundary value, note if the number of iterations 
  // extend EMiter than stops than
  float GetStopEMValue() {return AtlasNode->GetStopEMValue();}
  void SetStopEMValue(float init) {AtlasNode->SetStopEMValue(init);}

  int  GetStopEMMaxIter() {return AtlasNode->GetStopEMMaxIter();}
  void SetStopEMMaxIter(int init) {AtlasNode->SetStopEMMaxIter(init);}

  // Description:  
  // After which criteria should be stopped   
  // 0 = fixed iterations 
  // 1 = Absolut measure 
  // 2 = Relative measure 
  int  GetStopMFAType() {return AtlasNode->GetStopMFAType();}
  void SetStopMFAType(int init) {AtlasNode->SetStopMFAType(init);}

  // Description:  
  // What is the obundary value, note if the number of iterations 
  // extend MFAiter than stops than
  float  GetStopMFAValue() {return AtlasNode->GetStopMFAValue();}
  void SetStopMFAValue(float init) {AtlasNode->SetStopMFAValue(init);}

  int  GetStopMFAMaxIter() {return AtlasNode->GetStopMFAMaxIter();}
  void SetStopMFAMaxIter(int init) {AtlasNode->SetStopMFAMaxIter(init);}

  // Description:
  // Kilian: Jan06: InitialBias_FilePrefix allows initializing a bias field with a precomputed one 
  // - carefull Bias Field has to be in little Endian  - needed it for debugging
  char* GetInitialBiasFilePrefix() {return AtlasNode->GetInitialBiasFilePrefix();}
  void  SetInitialBiasFilePrefix(char* init) { AtlasNode->SetInitialBiasFilePrefix(init);}

  // Description:
  // Kilian: Jan06: This allows you to "jump" over the hirarchical segmentation level by providing an already existing 
  // labelmap of the region of interes 
  char* GetPredefinedLabelMapPrefix()           { return AtlasNode->GetPredefinedLabelMapPrefix();}
  void  SetPredefinedLabelMapPrefix(char* init) { AtlasNode->SetPredefinedLabelMapPrefix(init);}
  
  // Description:
  // Kilian Jan06: This is necessary so we can load in predefined labelmaps with different anatomical structures
  // If we cannot define the label of the superclass it would change everytime we change the tree structure
  int  GetPredefinedLabelID() {return AtlasNode->GetPredefinedLabelID();}
  void SetPredefinedLabelID(int init) {AtlasNode->SetPredefinedLabelID(init);}

  // Description:
  // This variable allows to control the influence of the LocalPrior spatially for all substructures 
  // Note: this variable is applied to all the subclasses during the segmentation bc the subclasses define the local Prior
  char* GetLocalPriorSpatialWeightName() {return AtlasNode->GetLocalPriorSpatialWeightName();}
  void  SetLocalPriorSpatialWeightName(char* init) {AtlasNode->SetLocalPriorSpatialWeightName(init);}

  // Description:
  // You can stop the bias calculation after a certain number of iterations
  // By default it is set to -1 which means it never stops
  vtkGetMacro(StopBiasCalculation,int); 
  vtkSetMacro(StopBiasCalculation,int); 

  // Description:
  // Activation of Registration within EM algorithm of atlas to image space 
  vtkGetMacro(RegistrationType, int);
  vtkSetMacro(RegistrationType, int);

  // Description:
  // If the flag is defined the spatial distribution of the first class will be automatically generated. 
  // In specifics the spatial distribution at voxel x is defined as 
  // spd(x) = NumberOfTrainingSamples - sum_{all other srructures dependent on the supercals} spd_struct(x) 
  vtkGetMacro(GenerateBackgroundProbability,int);      
  vtkSetMacro(GenerateBackgroundProbability,int);      


  // Description:
  // This variable can have three settings :
  // 0 = The PCA Model is generated over all structures together
  // 1 = Each structure has its own PCA model defined 
  // 2 = Do not maximize over the shape setting  - just use the current setting 
  vtkGetMacro(PCAShapeModelType,int); 
  vtkSetMacro(PCAShapeModelType,int); 

  // Desciption:
  // This flag is for the registration cost function. By default all subclasses are seen as one. 
  // In some cases this causes a loss of contrast within the cost function so that the registration is not as reliable, 
  // e.g. when we define two SuperClasses (FG and BG) which are defined as outside the brain as BG and everything inside the brain as FG, 
  // than we cannot use the ventricles wont be used for the alignment. Hoewever in many cases this structure drives the registration soley so that 
  /// our method is not as rebust. For this specific case we would set the flag for FG and do not set it for BG !
  vtkGetMacro(RegistrationIndependentSubClassFlag,int);      
  vtkSetMacro(RegistrationIndependentSubClassFlag,int);      
 
protected:
  vtkMrmlSegmenterSuperClassNode();
  ~vtkMrmlSegmenterSuperClassNode(){this->AtlasNode->Delete();};
  vtkMrmlSegmenterSuperClassNode(const vtkMrmlSegmenterSuperClassNode&) {};
  void operator=(const vtkMrmlSegmenterSuperClassNode&) {};

  vtkMrmlSegmenterAtlasSuperClassNode *AtlasNode;

  int PrintShapeSimularityMeasure; // Prints out the shape cost at each voxel 
  int PrintEMLabelMapConvergence;  // Prints out the number of voxels changed from last to this iteration
  int PrintEMWeightsConvergence;   // Prints out the difference in percent 
  int PrintMFALabelMapConvergence;  
  int PrintMFAWeightsConvergence; 

  int StopBiasCalculation;
  int RegistrationType; 
  int GenerateBackgroundProbability;
  int PCAShapeModelType;
  int RegistrationIndependentSubClassFlag;
};

#endif

