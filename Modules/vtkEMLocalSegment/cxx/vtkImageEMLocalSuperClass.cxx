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
#include "vtkImageEMLocalSuperClass.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------
vtkImageEMLocalSuperClass* vtkImageEMLocalSuperClass::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEMLocalSuperClass");
  if(ret)
  {
    return (vtkImageEMLocalSuperClass*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEMLocalSuperClass;

}


//------------------------------------------------------------------------------
// Define Procedures for vtkImageEMLocalSuperClass
//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::CreateVariables() {
  this->NumClasses          = 0;  
  this->ClassList           = NULL;
  this->ClassListType       = NULL;
  this->MrfParams           = NULL;
  this->ParentClass         = NULL;
  this->PrintFrequency      = 0;
  this->PrintBias           = 0;
  this->PrintLabelMap       = 0;
  
}

//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::DeleteSuperClassVariables() {
  int z,y;
  if (this->MrfParams) {
    for (z=0; z< 6; z ++) { 
      for (y=0; y < this->NumClasses; y++) delete[] this->MrfParams[z][y];
      delete[] this->MrfParams[z];
    }
    delete[] this->MrfParams;
  }
  if (this->ClassList)     delete[] this->ClassList; 
  if (this->ClassListType) delete[] this->ClassListType;  

  this->MrfParams           = NULL;
  this->ClassList           = NULL;
  this->ClassListType       = NULL;
  this->ParentClass         = NULL;
  this->NumClasses    = 0;
}
//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::AddSubClass(void* ClassData, classType initType, int index)
{
  if (index < 0) {
    vtkEMAddErrorMessage("Index is not set correctly");
    return;
  }
  // Only if input images are changing  - automatically updated works  
  // this->Modified();

  if (this->NumClasses <= index) {
    vtkImageEMLocalSuperClass *parent = this->ParentClass;
    classType*  oldClassListType;
    void**      oldClassList;
    int         oldNumClasses = this->NumClasses;
    // Delete and remeber old class list
    if (oldNumClasses) {
      oldClassListType = new classType[oldNumClasses];
      oldClassList     = new void*[oldNumClasses];
      for (int i = 0; i <oldNumClasses; i++) {
    oldClassList[i] = this->ClassList[i];
    oldClassListType[i] = this->ClassListType[i];
      }
      this->DeleteSuperClassVariables();
    }
    // Create  New Classes 
    this->ClassListType = new classType[index+1];
    this->ClassList     = new void*[index+1];
    for (int i = 0; i <= index; i++) this->ClassList[i] = NULL; 
    
    int z,y;
    this->MrfParams = new double**[6];
    for (z=0; z < 6; z++) {
      this->MrfParams[z] = new double*[index+1];
      for (y=0;y <= index; y ++) 
    this->MrfParams[z][y] = new double[index+1];
    }

    for (z=0; z < 6; z++) { 
      for (y=0;y <= index; y ++) memset(this->MrfParams[z][y], 0,NumClasses*sizeof(double)); 
    }

 
    // Copy over old values and delete them 
    if (oldNumClasses) {
      // Copy everything
      this->ParentClass = parent;
      for (int i = 0; i < oldNumClasses; i++) {
        this->ClassList[i] =  oldClassList[i];
        this->ClassListType[i] =  oldClassListType[i];
      }
      // Delete 
      delete[]  oldClassList;
      delete[]  oldClassListType;

    }
    this->NumClasses = index + 1;
  }
  this->ClassList[index] = ClassData;
  this->ClassListType[index] = initType;
}

//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::SetMarkovMatrix(double value, int k, int j, int i) {
  if ((j<0) || (j >= this->NumClasses) ||
      (i<0) || (i >= this->NumClasses) ||
      (k<0) || (k > 5) || (value < 0) || (value > 1)) {
    vtkEMAddErrorMessage("Error:vtkImageEMLocalSuperClass::SetMarkovMatrix Incorrect input: " << value << " " << k << " " <<  j << " " << i << " " << this->NumClasses);
    return;
  }
  this->MrfParams[k][j][i] = value;
}

//------------------------------------------------------------------------------
// if flag is set => includes subclasses of type SUPERCLASS
int vtkImageEMLocalSuperClass::GetTotalNumberOfClasses(bool flag) {
  int result = 0;
  for (int i=0;  i< this->NumClasses; i++) {
    if (this->ClassListType[i] == SUPERCLASS) {
      result += ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetTotalNumberOfClasses(flag); 
      if (flag == true) result ++;
    }  else { result ++;}
  }
  return result;
}

//------------------------------------------------------------------------------
// Returns a list of all labels (excluding the once with -1 and double once) in numerical order ,e.g. 0 1 4 5  
// Make sure LabelList is  with 0 -> otherwise won't get right list 
int vtkImageEMLocalSuperClass::GetAllLabels(short *LabelList, int result, int Max) {
  int i,j,k, label;
  for (i=0;  i< this->NumClasses; i++) {
    if (this->ClassListType[i] == SUPERCLASS) {
      result = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetAllLabels(LabelList,result,Max);
    } else {
      j= 0;
      label = ((vtkImageEMLocalClass*) this->ClassList[i])->GetLabel();
      if (label > -1) { 
    if (result) while ((j< result) && (LabelList[j] < label)) j++;
    // Make sure not doubles are in it 
    if ((j== result) || LabelList[j] > label) {
      result ++;
      if (j < Max) {
        k = result -1;
        while (k>j) {
          LabelList[k] =  LabelList[k-1];
          k--;
        }
        LabelList[j] = label;
      }
    }
      }
    }
  }
  return result;
}

//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::LabelAllSuperClasses(short *TakenLabelList, int Max) {
  int i,j,k;
  short label=1;
  for (i=0;  i < this->NumClasses; i++) {
    if (this->ClassListType[i] == SUPERCLASS) {
      ((vtkImageEMLocalSuperClass*) this->ClassList[i])->LabelAllSuperClasses(TakenLabelList,Max);
    
      for (j = 0; j <Max; j++){
    while ((label > TakenLabelList[j]) && ( j < Max)) j++;
    // Write here the section this does not work right now 
    if (label == TakenLabelList[j]) label++;
        else { 
      if (label < TakenLabelList[j]) {
        k = Max -1;
        while (k>j) {TakenLabelList[k] =  TakenLabelList[k-1];k--;}
      } 
      // Otherwise it has to be a zero becuase the list is ordered numerical
      TakenLabelList[j] = label;
      ((vtkImageEMLocalSuperClass*) this->ClassList[i])->Label = label; 
      j= Max; 
    }
      }
    }
  }
}

//------------------------------------------------------------------------------
int vtkImageEMLocalSuperClass::GetTotalNumberOfProbDataPtr() {
  int result = 0;
  for (int i=0;  i< this->NumClasses; i++) {
    if (this->ClassListType[i] == SUPERCLASS) {
      result += ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetTotalNumberOfProbDataPtr(); 
    } else { 
      if (((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataWeight() >  0.0) result++;
    }
  }
  return result;
}

//------------------------------------------------------------------------------
int vtkImageEMLocalSuperClass::GetProbDataPtrFlag() {
  for (int i = 0; i < this->NumClasses; i++) {
    if (this->ClassListType[i] == CLASS) {
      if (((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataPtr()) return 1;
    } else {
      if (((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataPtrFlag()) return 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
int vtkImageEMLocalSuperClass::GetProbDataPtr(void **PointerList, int index) {
 int i;
  for (i = 0; i <this->NumClasses; i++) {
    if (this->ClassListType[i] == CLASS) {
      PointerList[index] = ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataPtr();
      index ++;
    } else index = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataPtr(PointerList,index); 
  }
  return index;
} 

//------------------------------------------------------------------------------
int vtkImageEMLocalSuperClass::GetProbDataIncYandZ(int* ProbDataIncY,int* ProbDataIncZ,int index) {
  int i;
  for (i = 0; i <this->NumClasses; i++) {
    if (this->ClassListType[i] == CLASS) {
      if (((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataWeight() > 0.0) {
        ProbDataIncY[index] = ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataIncY();
        ProbDataIncZ[index] = ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataIncZ();
        index ++;
      }
    } else index = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataIncYandZ(ProbDataIncY,ProbDataIncZ,index); 
  }
  return index;
} 

//------------------------------------------------------------------------------
int vtkImageEMLocalSuperClass::GetTissueDefinition(int *LabelList,double** LogMu, double ***LogCov, int index) {
  int i;
  for (i = 0; i < this->NumClasses; i++) {
    if (this->ClassListType[i] == CLASS) {
      LogMu[index]         = ((vtkImageEMLocalClass*) this->ClassList[i])->GetLogMu();
      LogCov[index]        = ((vtkImageEMLocalClass*) this->ClassList[i])->GetLogCovariance();
      LabelList[index]     = ((vtkImageEMLocalClass*) this->ClassList[i])->GetLabel();
      index ++;
    } else index = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetTissueDefinition(LabelList,LogMu, LogCov,index);
  }
  return index;
} 

//------------------------------------------------------------------------------
classType vtkImageEMLocalSuperClass::GetClassType(void* active) {
  int i = 0; 
  while ((i < this->NumClasses) && (active != this->ClassList[i])) i++;
  if (i == this->NumClasses) return CLASS;
  return this->ClassListType[i];                     
}

//------------------------------------------------------------------------------
void vtkImageEMLocalSuperClass::PrintSelf(ostream& os,vtkIndent indent) {
  os << indent << "---------------------------------------- SUPERCLASS ----------------------------------------------" << endl;
  this->vtkImageEMGenericClass::PrintSelf(os,indent); 
  os << indent << "NumClasses:              " << this->NumClasses << endl;
  os << indent << "PrintFrequency:          " << this->PrintFrequency << endl;
  os << indent << "PrintBias:               " << this->PrintBias<< endl;
  os << indent << "PrintLabelMap:           " << this->PrintLabelMap << endl;

  char** Directions= new char*[6];
  Directions[0] = "West "; Directions[1] = "North"; Directions[2] = "Up   "; Directions[3] = "East "; Directions[4] = "South"; Directions[5] = "Down ";
  os << indent << "MrfParams:               " << endl;
  for (int z=0; z < 6; z++) { 
    os << indent << "   " << Directions[z] << ":    ";   
    for (int y=0;y < this->NumClasses; y ++) {
      if (y) os << "| ";
      for (int x=0;x < this->NumClasses; x ++) os << this->MrfParams[z][y][x] << " " ;
    }
    os << endl;
  }
  // If you uncomment the following it gives you a seg fault 
  // for (int z=0; z < 6; z++) delete[] Directions[z];
  delete[] Directions; 
  for (int i =0; i < this->NumClasses; i++) {
    if (this->ClassListType[i] == CLASS) ((vtkImageEMLocalClass*)this->ClassList[i])->PrintSelf(os,indent.GetNextIndent());
    else ((vtkImageEMLocalSuperClass*)this->ClassList[i])->PrintSelf(os,indent.GetNextIndent());
  }
}


//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageEMLocalSuperClass::ExecuteData(vtkDataObject *)
{
  // cout << "start vtkImageEMLocalSuperClass::ExecuteData" << endl; 
   vtkDebugMacro(<<"ExecuteData()"); 
   // ==================================================
   // Check existence of subclasses and update them 
   this->ResetWarningMessage();
   if (this->GetErrorFlag()) 
     vtkEMAddWarningMessage("The error flag for this module was set with the following messages (the error messages will be reset now:\n"<<this->GetErrorMessages());

   this->ResetErrorMessage();
   this->vtkImageEMGenericClass::ExecuteData(NULL);
   // Error Occured
   // Kilian change this 
   if (this->GetErrorFlag()) return;

   if (!this->NumClasses) {
     vtkEMAddErrorMessage("No sub classes defined!");
     return;
   }

   
   {
     int ProbDataPtrIndex = -1; 

     for (int i = 0; i <this->NumClasses; i++) {
       if (!this->ClassList[i]) {
         vtkEMAddErrorMessage("Class with index "<< i <<" is not defined!");
         return;
       }

       if (this->ClassListType[i] == CLASS) {
         ((vtkImageEMLocalClass*) this->ClassList[i])->Update();
         // Add Messages to own errror messages 
         if (((vtkImageEMLocalClass*) this->ClassList[i])->GetErrorFlag()) {
       vtkEMJustAddErrorMessage(((vtkImageEMLocalClass*) this->ClassList[i])->GetErrorMessages());
       return;
         }
     if ((ProbDataPtrIndex < 0) && ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataPtr())  ProbDataPtrIndex = i;

       } else {
         ((vtkImageEMLocalSuperClass*) this->ClassList[i])->Update();
         // Add Messages to own errror messages 
         if (((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetErrorFlag()) {
       vtkEMJustAddErrorMessage(((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetErrorMessages());
       return;
         }

     if ((ProbDataPtrIndex < 0) &&  ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataPtrFlag()) ProbDataPtrIndex = i;
       }
     }
     if ((ProbDataPtrIndex < 0) && (this->ProbDataWeight > 0.0) ) { 
       this->ProbDataWeight = 0.0; 
       vtkEMAddWarningMessage("No PropDataPtr defined for any sub classes  => ProbDataWeight is set to 0! "); 
     }

     // ==================================================
     // Set values 
     if (ProbDataPtrIndex > -1) {
       int index = ProbDataPtrIndex;
       if (this->ClassListType[index] == CLASS) {
     memcpy(this->SegmentationBoundaryMax,((vtkImageEMLocalClass*) this->ClassList[index])->GetSegmentationBoundaryMax(),sizeof(int)*3);
     memcpy(this->SegmentationBoundaryMin,((vtkImageEMLocalClass*) this->ClassList[index])->GetSegmentationBoundaryMin(),sizeof(int)*3);
     memcpy(this->DataDim,((vtkImageEMLocalClass*) this->ClassList[index])->GetDataDim(),sizeof(int)*3);
     memcpy(this->DataSpacing,((vtkImageEMLocalClass*) this->ClassList[index])->GetDataSpacing(),sizeof(float)*3);
       }
       else {
     memcpy(this->SegmentationBoundaryMax,((vtkImageEMLocalSuperClass*) this->ClassList[index])->GetSegmentationBoundaryMax(),sizeof(int)*3);
     memcpy(this->SegmentationBoundaryMin,((vtkImageEMLocalSuperClass*) this->ClassList[index])->GetSegmentationBoundaryMin(),sizeof(int)*3);
     memcpy(this->DataDim,((vtkImageEMLocalSuperClass*) this->ClassList[index])->GetDataDim(),sizeof(int)*3);
     memcpy(this->DataSpacing,((vtkImageEMLocalSuperClass*) this->ClassList[index])->GetDataSpacing(),sizeof(float)*3);
       }
     }
   } 
   // Look for the first ProbData entry and then define scalar type accordingly
   int i;
   for (i = 0; i <this->NumClasses; i++) {
     if (this->ClassListType[i] == CLASS) {
       this->ProbDataScalarType = ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataScalarType(); 
       if (this->ProbDataScalarType > -1) i = this->NumClasses;
     } else {
       this->ProbDataScalarType = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataScalarType(); 
       if (this->ProbDataScalarType > -1) i = this->NumClasses;
     }
   }
     
   // ==================================================
   // Check own values
   for (i=0;i <  this->NumClasses; i++) {
    for (int j = 0; j < this->NumClasses; j++) {
      for (int k = 0; k < 6; k++) {
    if ((this->MrfParams[k][j][i] < 0) || (this->MrfParams[k][j][i] > 1)) {
      vtkEMAddErrorMessage("MrfParams[" << k <<"] [" << j<<"] [" << i <<"] = " << this->MrfParams[k][j][i] << " is not between 0 and 1 !");
      return;
    }
      }
    }
   }

   // ==================================================
   // Check values of subclasses
   for (i = 0; i <this->NumClasses; i++) {
     if (this->ClassListType[i] == CLASS) {
       if (((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataPtr()) {
         // Check if any input data was defined 
     
     if ((((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataScalarType() > -1) && (this->ProbDataScalarType != ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataScalarType())) {
       vtkEMAddErrorMessage( "ProbDataScalarType of  class "<< i << " is of type "<< ((vtkImageEMLocalClass*) this->ClassList[i])->GetProbDataScalarType() 
               << ", which is inconsistent with this super class' scalar type "<< this->ProbDataScalarType << "! Note: VTK_FLOAT="<< VTK_FLOAT <<", VTK_SHORT=" << VTK_SHORT);
       return;
     }

     int* max = ((vtkImageEMLocalClass*) this->ClassList[i])->GetSegmentationBoundaryMax();
     if (memcmp(this->SegmentationBoundaryMax,max,sizeof(int)*3)) {
       vtkEMAddErrorMessage( "SegmentationBoundaryMax of  class "<< i << "( "<< max[0] << "," << max[1] << "," << max[2] 
                 << ") is inconsistent with this super class' ones ("<< this->SegmentationBoundaryMax[0] <<","<<this->SegmentationBoundaryMax[1] 
                 << "," << this->SegmentationBoundaryMax[2] <<")");
       return; 
     }

     int* min = ((vtkImageEMLocalClass*) this->ClassList[i])->GetSegmentationBoundaryMin();
     if (memcmp(this->SegmentationBoundaryMin,min,sizeof(int)*3)) {
       vtkEMAddErrorMessage( "SegmentationBoundaryMin of  class "<< i << "( "<< min[0] << "," << min[1] << "," << min[2] 
                 << ") is inconsistent with this super class' ones ("<< this->SegmentationBoundaryMin[0] <<","<<this->SegmentationBoundaryMin[1] 
                 << "," << this->SegmentationBoundaryMin[2] <<")");
       return; 
     }

     int* dim = ((vtkImageEMLocalClass*) this->ClassList[i])->GetDataDim();
     if (memcmp(this->DataDim,dim,sizeof(int)*3)) {
       vtkEMAddErrorMessage("DataDim of class "<< i << "( "<< dim[0] << "," << dim[1] << "," << dim[2] 
                << ") is inconsistent with this super class' ones ("<< this->DataDim[0] <<","<<this->DataDim[1] << "," << this->DataDim[2] <<")");
       return; 
     }
     float* spacing = ((vtkImageEMLocalClass*) this->ClassList[i])->GetDataSpacing();
     if (memcmp(this->DataSpacing,spacing,sizeof(float)*3)) {
       vtkEMAddErrorMessage("DataSpacing of class "<< i << " ("<< spacing[0] << "," << spacing[1] << "," << spacing[2] 
                << ") is inconsistent with this super class' ones ("<< this->DataSpacing[0] <<","<<this->DataSpacing[1] << "," << this->DataSpacing[2] <<")");
       return; 
     }
       } 
     } else {
       // If data is part of the subclasses - check dimension
       if (    ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataPtrFlag()) {

     if ((((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataScalarType() > -1) 
            && (this->ProbDataScalarType != ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataScalarType())) {
       vtkEMAddErrorMessage( "ProbDataScalarType of class "<< i << " is of type "<< ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetProbDataScalarType() 
                 << ", which is inconsistent with this super class' scalar type "<< this->ProbDataScalarType 
                                 << "! Note: VTK_FLOAT="<< VTK_FLOAT <<", VTK_SHORT=" << VTK_SHORT);
       return;
     }


     int* max = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetSegmentationBoundaryMax();
     if (memcmp(this->SegmentationBoundaryMax,max,sizeof(int)*3)) {
       vtkEMAddErrorMessage( "SegmentationBoundaryMax of class "<< i << "( "<< max[0] << "," << max[1] << "," << max[2] 
                       << ") is inconsistent with this super class' ones ("<< this->SegmentationBoundaryMax[0] <<","
                       << this->SegmentationBoundaryMax[1] << "," << this->SegmentationBoundaryMax[2] <<")");
       return; 
     }

     int* min = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetSegmentationBoundaryMin();
     if (memcmp(this->SegmentationBoundaryMin,min,sizeof(int)*3)) {
       vtkEMAddErrorMessage( "SegmentationBoundaryMin of  class "<< i << "( "<< min[0] << "," << min[1] << "," << min[2] 
                 << ") is inconsistent with this super class' ones ("<< this->SegmentationBoundaryMin[0] <<","
                 << this->SegmentationBoundaryMin[1] << "," << this->SegmentationBoundaryMin[2] <<")");
       return; 
     }

     int* dim = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetDataDim();
     if (memcmp(this->DataDim,dim,sizeof(int)*3)) {
       vtkEMAddErrorMessage( "DataDim of class "<< i << "( "<< dim[0] << "," << dim[1] << "," << dim[2] 
                       << ") is inconsistent with this super class' ones ("<< this->DataDim[0] <<","<<this->DataDim[1] << "," << this->DataDim[2] <<")");
       return; 
     }

     float* spacing = ((vtkImageEMLocalSuperClass*) this->ClassList[i])->GetDataSpacing();
     if (memcmp(this->DataSpacing,spacing,sizeof(float)*3)) {
       vtkEMAddErrorMessage( "DataSpacing of class "<< i << " ("<< spacing[0] << "," << spacing[1] << "," << spacing[2] 
                       << ") is inconsistent with this super class' ones ("<< this->DataSpacing[0] <<","<<this->DataSpacing[1] << "," << this->DataSpacing[2] <<")");
       return; 
     }

       } 
     }
   }
}
