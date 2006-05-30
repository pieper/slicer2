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
/*==============================================================================
(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
==============================================================================*/
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//               File: vtkIsingActivationThreshold.cxx                        //
//               Date: 05/2006                                                //
//               Author: Carsten Richter                                      //
//                                                                            //
// Description: computes dimensions of activation volume input                //
//              creates segmentation label index depending on segmentation    //
//                volume input                                                //
//              computes number of activation states depending on             //
//                activation volume input                                     //
//              computes number of classes (number of segmentation label x    //
//                activation states)                                          //
//              computes activation labels depending on activation volume     //
//                input and threshold                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "vtkObjectFactory.h"
#include "vtkIsingActivationThreshold.h"

vtkStandardNewMacro(vtkIsingActivationThreshold);

vtkIsingActivationThreshold::vtkIsingActivationThreshold()
{
  this->segLabel = vtkIntArray::New();
  // there is always at least one label
  this->segInput = 1;
  // label values for slicer label map
  this->nonactive = 0;
  this->posactive = 301;
  this->negactive = 300;
}

vtkIsingActivationThreshold::~vtkIsingActivationThreshold()
{
  this->segLabel->Delete();
}

void vtkIsingActivationThreshold::SimpleExecute(vtkImageData *input, vtkImageData *output)
{ 

  this->GetInput(0)->GetDimensions(dims);
  x = dims[0];
  y = dims[1];
  z = dims[2];
  size = x*y*z;
  
  // in case of anatomical label map input
  if (this->NumberOfInputs == 2){
   
    // check if Dimensions of inputs match
    this->GetInput(1)->GetDimensions(dims);
    x2 = dims[0];
    y2 = dims[1];
    z2 = dims[2];
  
    // computation of segmentation label
    if ((x != x2) || (y != y2) || (z != z2)){
      vtkErrorMacro( << "Activation volume and label map differ in dimension. Label map is invalid.");
      segLabel->InsertNextValue(0);
    }
    else{
      vtkIntArray *segLabelCopy = vtkIntArray::New();
      labelValue = (short int *) (GetInput(1)->GetScalarPointer(0,0,0));
      segLabelCopy->InsertNextValue((int)(*labelValue));
      
      enumerator = 1;
      register int i, j, k;     
      for (k=0; k<z; k++)
        for (j=0; j<y; j++)
          for (i=0; i<x; i++){
            insert = 1;
            labelValue = (short int *) (GetInput(1)->GetScalarPointer(i,j,k));
            for (int n=0; n<enumerator; n++){  
              if ((segLabelCopy->GetValue(n)) == ((int)(*labelValue)))
                insert = 0;
            }
            if (insert == 1){
              segLabelCopy->InsertNextValue((int)(*labelValue));
              enumerator++;
            }
          }  
            
      // sorting the labels in increasing order  
      position = 0;
      for (int i=0; i<enumerator; i++){
        segLabel->InsertNextValue(40000);
        for (int n=0; n<enumerator; n++){
          if (segLabelCopy->GetValue(n) < segLabel->GetValue(i)){
            segLabel->SetValue(i,segLabelCopy->GetValue(n));
            position = n;
          }
        }
        segLabelCopy->SetValue(position,40000);
      }
      
      segLabelCopy->Delete();
      segInput = enumerator;
    }           
  }
  else{
    segLabel->InsertNextValue(0);
  }
  
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();
  
  // get the data array from input image 
  vtkFloatArray *inputArray = (vtkFloatArray *)this->GetInput(0)->GetPointData()->GetScalars();
  vtkIntArray *activation = vtkIntArray::New();
  
  // in case of no negative activation, thresholds are set equal
  threshold2 = threshold;
  for (unsigned long int i=0; i<size; i++){
    if (inputArray->GetValue(i) < -threshold){
      threshold2 = -threshold;
      break;
    }
  }
  
  if (thresholdID == 1){
    // case of one single threshold
    if (threshold2 == threshold){
      numActivationStates = 2;
      for (unsigned long int i=0; i<size; i++){
        if (inputArray->GetValue(i) < threshold){
          activation->InsertNextValue(nonactive);
        }
        else{
          activation->InsertNextValue(posactive);
        }
      } 
    }
    else {
      // case of 2 threshold values
      numActivationStates = 3;
      for (unsigned long int i=0; i<size; i++){
        if (inputArray->GetValue(i) < threshold2){
          activation->InsertNextValue(negactive);
        }
        else if (inputArray->GetValue(i) > threshold){
          activation->InsertNextValue(posactive);
        }
        else{
          activation->InsertNextValue(nonactive);
        }  
      }
    }
  }
  else{
    // case of one single threshold
    if (threshold2 == threshold){
      numActivationStates = 2;
      for (unsigned long int i=0; i<size; i++){
        if (inputArray->GetValue(i) < threshold){
          activation->InsertNextValue(0);
        }
        else{
          activation->InsertNextValue(1);
        }
      } 
    }
    else {
      // case of 2 threshold values
      numActivationStates = 3;
      for (unsigned long int i=0; i<size; i++){
        if (inputArray->GetValue(i) < threshold2){
          activation->InsertNextValue(2);
        }
        else if (inputArray->GetValue(i) > threshold){
          activation->InsertNextValue(1);
        }
        else{
          activation->InsertNextValue(0);
        }  
      }
    }
  }
  
  // number of classes
  nType = (numActivationStates*segInput);  
  output->GetPointData()->SetScalars(activation);
  activation->Delete();
}

// If the output image of a filter has different properties from the input image
// we need to explicitly define the ExecuteInformation() method
void vtkIsingActivationThreshold::ExecuteInformation(vtkImageData *input, vtkImageData *output)
{
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();
}
