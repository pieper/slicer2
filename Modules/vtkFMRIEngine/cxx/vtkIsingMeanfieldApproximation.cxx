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
//               File: vtkIsingMeanfieldApproximation.cxx                     //
//               Date: 05/2006                                                //
//               Author: Carsten Richter                                      //
//                                                                            //
// Description: computes for each voxel the posterior class probability       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "vtkObjectFactory.h"
#include "vtkIsingMeanfieldApproximation.h"
#include "vtkCommand.h"

vtkStandardNewMacro(vtkIsingMeanfieldApproximation);

vtkIsingMeanfieldApproximation::vtkIsingMeanfieldApproximation()
{
  this->nonactive = 0;
  this->posactive = 301;
  this->negactive = 300;
  this->logTransitionMatrix = vtkFloatArray::New();
}

vtkIsingMeanfieldApproximation::~vtkIsingMeanfieldApproximation()
{
  this->logTransitionMatrix->Delete();
}

void vtkIsingMeanfieldApproximation::SimpleExecute(vtkImageData *input, vtkImageData *output)
{
  dims[0] = x;
  dims[1] = y;
  dims[2] = z;
  size = x*y*z;

  // in case of anatomical label map input
  if (this->NumberOfInputs == 3){
    segMArray = vtkIntArray::New();
    for (unsigned long int i=0; i<size; i++){
      segMArray->InsertNextValue(0);
    }
    register int i, j, k;     
    for (k=0; k<z; k++)
      for (j=0; j<y; j++)
        for (i=0; i<x; i++){
          labelValue = (short int *) (GetInput(2)->GetScalarPointer(i,j,k));
          segMArray->SetValue((k*x*y)+(j*x)+i,(int)(*labelValue));
        }
    for (int j=0; j<segInput; j++){
      for (unsigned long int i=0; i<size; i++)
        if (segMArray->GetValue(i) == segLabel->GetValue(j))
          segMArray->SetValue(i,j);
    }
  }
  else {
    segMArray = vtkIntArray::New();
    for (unsigned long int i=0; i<size; i++) 
      segMArray->InsertNextValue(0);
  }
  
  vtkIntArray *classArray = (vtkIntArray *)this->GetInput(0)->GetPointData()->GetScalars();
        
  sum = 0;
  for (int i=0; i<(nType*nType); i++)
    sum += (transitionMatrix->GetValue(i));
        
  if (sum == 0){      
    // construction of a matrix indicating the transition strength between classes      
    register int i, j, k;
    for (i=0; i<x; i++)
      for (j=0; j<y; j++)
        for (k=0; k<z; k++){
          if (i != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+(j*x)+i-1);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));
          }
          if (i != x-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+(j*x)+i+1);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));
          }
          if (j != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+((j-1)*x)+i);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));
          }
          if (j != y-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+((j+1)*x)+i);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));       
          }
          if (k != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue(((k-1)*x*y)+(j*x)+i);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));
          }
         if (k != z-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue(((k+1)*x*y)+(j*x)+i);
            transitionMatrix->SetValue((index1*nType)+index2, (transitionMatrix->GetValue((index1*nType)+index2)+1));
          }
        }
        
    // neighborhoods were counted double
    for (int i=0; i<nType; i++)
      if (transitionMatrix->GetValue((i*nType)+i) != 0)
        transitionMatrix->SetValue((i*nType)+i, (transitionMatrix->GetValue((i*nType)+i))/2);
  }  
  
  // in case of existing 0 values in transition matrix, increase all by 1 to prevent log range error   
  for (int i=0; i<(nType*nType); i++)
    if (transitionMatrix->GetValue(i) == 0){
      for (int j=0; j<(nType*nType); j++)             
        transitionMatrix->SetValue(j, (transitionMatrix->GetValue(j))+1);     
      break;
    }
  
  // construction of log transition matrix
  logTransitionMatrix->SetNumberOfValues(nType*nType);
  for (int i=0; i<nType; i++)
    for (int j=0; j<nType; j++){
      logHelp = (float) log((transitionMatrix->GetValue((i*nType)+j))/sqrt(((activationFrequence->GetValue(i))*size)*((activationFrequence->GetValue(j))*size)));                  
      logTransitionMatrix->SetValue((i*nType)+j, logHelp);
    }
  
  vtkFloatArray *probGivenClassArray = (vtkFloatArray *)this->GetInput(1)->GetPointData()->GetScalars();
  vtkFloatArray *outputArray = vtkFloatArray::New();
  vtkIntArray *finalOutput = vtkIntArray::New();
  
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();

  helpArray = new float[nType];

  // initialization of class probability output volume
  for (int n=0; n<nType; n++)
    for (unsigned long int i=0; i<size; i++)
      outputArray->InsertNextValue((1.0/nType));
  
  // meanfield iteration     
  register int i, j, k, n;     
  for (n=0; n<iterations; n++){
    for (k=0; k<z; k++)
      for (j=0; j<y; j++)
        for (i=0; i<x; i++){
          sumHelpArray = 0.0;
          for (int l=0; l<nType; l++){
            eValue = 0.0;          
            if (i != 0){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+(j*x)+i-1))*(logTransitionMatrix->GetValue((l*nType)+s))); 
            }    
            if (i != x-1){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+(j*x)+i+1))*(logTransitionMatrix->GetValue((l*nType)+s)));                             
            }           
            if (j != 0){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+((j-1)*x)+i))*(logTransitionMatrix->GetValue((l*nType)+s))); 
            }           
            if (j != y-1){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+((j+1)*x)+i))*(logTransitionMatrix->GetValue((l*nType)+s)));
            }        
            if (k != 0){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+((k-1)*x*y)+(j*x)+i))*(logTransitionMatrix->GetValue((l*nType)+s))); 
            }    
            if (k != z-1){
              for (int s=0; s<nType; s++)
                eValue += ((outputArray->GetValue((s*size)+((k+1)*x*y)+(j*x)+i))*(logTransitionMatrix->GetValue((l*nType)+s))); 
            }        
                        
            helpArray[l] = (activationFrequence->GetValue(l)) * (probGivenSegM->GetValue((segMArray->GetValue((k*x*y)+(j*x)+i))*nType+l)) * (probGivenClassArray->GetValue((l*size)+(k*x*y)+(j*x)+i)) * exp(eValue);                    
            sumHelpArray += helpArray[l];
          }
          for (int l=0; l<nType; l++){
            outputArray->SetValue((l*size)+(k*x*y)+(j*x)+i, helpArray[l]/sumHelpArray);
          }
        }   
    UpdateProgress(n * (1.0/iterations));   
  }
  
  // creation of activation label map
  for (unsigned long int i=0; i<size; i++){
    max = 0.0;
    posMax = 0;
    for (int n=0; n<nType; n++){
      if ((outputArray->GetValue((n*size)+i)) > max){
        max = (outputArray->GetValue((n*size)+i)); 
        posMax = n;
      }
    }   
    if (numActivationStates == 2){
      if (posMax < (nType/2))
        finalOutput->InsertNextValue(nonactive);
      else
        finalOutput->InsertNextValue(posactive);
    }
    else{
      if (posMax < (nType/3))
        finalOutput->InsertNextValue(nonactive);
      else
        if ((posMax >= (nType/3)) && (posMax < 2*(nType/3)))
          finalOutput->InsertNextValue(posactive);
        else
          finalOutput->InsertNextValue(negactive);
    }
  }     

  output->GetPointData()->SetScalars(finalOutput); 
  if (this->NumberOfInputs != 3)
    segMArray->Delete();
  delete [] helpArray;
  outputArray->Delete(); 
  finalOutput->Delete();     
}



// If the output image of a filter has different properties from the input image
// we need to explicitly define the ExecuteInformation() method
void vtkIsingMeanfieldApproximation::ExecuteInformation(vtkImageData *input, vtkImageData *output)
{
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();
}
