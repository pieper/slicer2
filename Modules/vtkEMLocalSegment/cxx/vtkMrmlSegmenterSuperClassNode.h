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
// .NAME vtkMrmlSegmenterClassNode - MRML node to represent transformation matrices.
// .SECTION Description
// The output of a rigid-body registration is a rotation and translation 
// expressed mathematically as a transformation matrix.  These transforms 
// can be inserted into MRML files as Segmenter nodes.  Each matrix 
// affects volumes and models that appear below it in the MRML file.  
// Multiple matrices can be concatenated together. 

#ifndef __vtkMrmlSegmenterSuperClassNode_h
#define __vtkMrmlSegmenterSuperClassNode_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkMrmlSegmenterGenericClassNode.h"
#include "vtkSlicer.h"
#include <vtkEMLocalSegmentConfigure.h>

// For the first stage super class is just a hirachical element, where we just define the name
// Extensions for later are planned
// Kilian 07-Oct-02

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

   // Description:
  // Get/Set for Segmenter
  vtkSetMacro(NumClasses, int);
  vtkGetMacro(NumClasses, int);

  // Description:
  // Print out the result after how many steps  (-1 == just last result, 0 = No Printing, i> 0 => every i-th slice )
  vtkGetMacro(PrintFrequency, int);
  vtkSetMacro(PrintFrequency, int);

  vtkGetMacro(PrintBias, int);
  vtkSetMacro(PrintBias, int);

  vtkGetMacro(PrintLabelMap, int);
  vtkSetMacro(PrintLabelMap, int);  


  // Description:
  // Prints out the number of voxels changed from last to this EM iteration
  vtkGetMacro(PrintEMLabelMapConvergence, int);  
  vtkSetMacro(PrintEMLabelMapConvergence, int);  

  // Description:
  // Prints out the difference in percent 
  vtkGetMacro(PrintEMWeightsConvergence, int);
  vtkSetMacro(PrintEMWeightsConvergence, int);

  // Description:  
  // After which criteria should be stopped   
  // 0 = fixed iterations 
  // 1 = Absolut measure 
  // 2 = Relative measure 
  vtkGetMacro(BoundaryStopEMType,int); 
  vtkSetMacro(BoundaryStopEMType,int); 
  
  // Description:  
  // What is the obundary value, note if the number of iterations 
  // extend EMiter than stops than
  vtkGetMacro(BoundaryStopEMValue,float);      
  vtkSetMacro(BoundaryStopEMValue,float); 

  vtkGetMacro(BoundaryStopEMMaxIterations,int); 
  vtkSetMacro(BoundaryStopEMMaxIterations,int); 

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
  vtkGetMacro(BoundaryStopMFAType,int); 
  vtkSetMacro(BoundaryStopMFAType,int); 
  
  // Description:  
  // What is the obundary value, note if the number of iterations 
  // extend MFAiter than stops than
  vtkGetMacro(BoundaryStopMFAValue,float);      
  vtkSetMacro(BoundaryStopMFAValue,float); 

  vtkGetMacro(BoundaryStopMFAMaxIterations,int); 
  vtkSetMacro(BoundaryStopMFAMaxIterations,int); 

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

protected:
  vtkMrmlSegmenterSuperClassNode();
  ~vtkMrmlSegmenterSuperClassNode();
  vtkMrmlSegmenterSuperClassNode(const vtkMrmlSegmenterSuperClassNode&) {};
  void operator=(const vtkMrmlSegmenterSuperClassNode&) {};

  int NumClasses;

  int PrintFrequency;
  int PrintBias;
  int PrintLabelMap;

  int PrintEMLabelMapConvergence;  // Prints out the number of voxels changed from last to this iteration
  int PrintEMWeightsConvergence; // Prints out the difference in percent 

  int BoundaryStopEMType;       // After which criteria should be stopped   
                                // 0 = fixed iterations 
                                // 1 = Absolut measure 
                                // 2 = Relative measure
  float BoundaryStopEMValue;    // What is the obundary value, note if the number of iterations 
                                // extend EMiter than stops than
                                // if (BoundaryStopEMType = 1) than it is percent

  int BoundaryStopEMMaxIterations;

  int PrintMFALabelMapConvergence;  
  int PrintMFAWeightsConvergence; 
  int BoundaryStopMFAType;       
  float BoundaryStopMFAValue;    
  int BoundaryStopMFAMaxIterations;

  int    RegistrationType; 
  int GenerateBackgroundProbability;

};

#endif

