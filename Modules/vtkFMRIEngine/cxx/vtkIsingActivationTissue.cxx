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
//               File: vtkIsingActivationTissue.cxx                           //
//               Date: 05/2006                                                //
//               Author: Carsten Richter                                      //
//                                                                            //
// Description: computes class label map depending on segmentation volume and //
//                activation volume input                                     //
//              computes class frequence                                      //
//                class = (non state * segmentation labels) +                 // 
//                        (pos state * segmentation labels) +                 // 
//                        (neg state * segmentation labels)                   // 
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "vtkObjectFactory.h"
#include "vtkIsingActivationTissue.h"

vtkStandardNewMacro(vtkIsingActivationTissue);

vtkIsingActivationTissue::vtkIsingActivationTissue()
{
  this->activationFrequence = vtkFloatArray::New();
}

vtkIsingActivationTissue::~vtkIsingActivationTissue()
{
  this->activationFrequence->Delete();
}

void vtkIsingActivationTissue::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  dims[0] = x;
  dims[1] = y;
  dims[2] = z;
  size = x*y*z;
  
  // initialization of activation Frequence
  for (int i=0; i<nType; i++){
    activationFrequence->InsertNextValue(0.0);
  }
  
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetNumberOfScalarComponents(1);
  output->AllocateScalars();

  // in case of anatomical label map input
  if (this->NumberOfInputs == 2){
    segMArray = vtkIntArray::New();
    for (unsigned long int i=0; i<size; i++){
      segMArray->InsertNextValue(0);
    }
    register int i, j, k;     
    for (k=0; k<z; k++)
      for (j=0; j<y; j++)
        for (i=0; i<x; i++){
          labelValue = (short int *) (GetInput(1)->GetScalarPointer(i,j,k));
          segMArray->SetValue((k*x*y)+(j*x)+i,(int)(*labelValue));
        }
    for (int j=0; j<segInput; j++){
      if ((segLabel->GetValue(j)) == greyValue){
        nGreyValue = j;
      }
      for (unsigned long int i=0; i<size; i++){
        if ((segMArray->GetValue(i)) == (segLabel->GetValue(j))){
          segMArray->SetValue(i,j);  
        }
      }
    }
  }
 
  vtkIntArray *activation = (vtkIntArray *)this->GetInput(0)->GetPointData()->GetScalars();
  vtkIntArray *outputArray = vtkIntArray::New();
  
  // calculation of class volume
  // class = (non state * segmentation labels) +                 
  //         (pos state * segmentation labels) +                 
  //         (neg state * segmentation labels)    
  nonpp = 0.0;
  pospp = 0.0;
  negpp = 0.0;     
  for (unsigned long int i=0; i<size; i++){
    if (this->NumberOfInputs == 2){
      classIndex = ((activation->GetValue(i)) * segInput) + (segMArray->GetValue(i));
      outputArray->InsertNextValue(classIndex);
      if ((segMArray->GetValue(i)) == nGreyValue){
        if ((activation->GetValue(i)) == 0){
          nonpp += 1.0;
        }
        if ((activation->GetValue(i)) == 1){
          pospp += 1.0;
        }
        if ((activation->GetValue(i)) == 2){
          negpp += 1.0;
        }
      }
    }
    else {
      classIndex = activation->GetValue(i);
      outputArray->InsertNextValue(classIndex);
    }
    activationFrequence->SetValue(classIndex, activationFrequence->GetValue(classIndex) + 1);
  }      
  
  // calculation of activation probability of grey matter
  sumpp = nonpp+pospp+negpp;
  if (sumpp != 0.0){
    nonpp = nonpp/(1.0*(sumpp));
    pospp = pospp/(1.0*(sumpp));
    negpp = negpp/(1.0*(sumpp));
  }
  
  // calculation of class frequence
  for (int i=0; i<nType; i++){
    if (activationFrequence->GetValue(i) > 0.0)
      activationFrequence->SetValue(i, (activationFrequence->GetValue(i))/(float)size);  
    else
      activationFrequence->SetValue(i, 1.0/size);   
  }
  output->GetPointData()->SetScalars(outputArray);
  outputArray->Delete();      
}
