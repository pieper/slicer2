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
// .NAME vtkImageEMLocalSuperClass
#ifndef __vtkImageEMLocalSuperClass_h
#define __vtkImageEMLocalSuperClass_h 
  
#include <vtkEMLocalSegmentConfigure.h> 

#include "vtkSlicer.h"
#include "vtkImageEMLocalClass.h"


class VTK_EMLOCALSEGMENT_EXPORT vtkImageEMLocalSuperClass : public vtkImageEMGenericClass
{
  public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImageEMLocalSuperClass *New();
  vtkTypeMacro(vtkImageEMLocalSuperClass,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  int           GetNumClasses() {return this->NumClasses;}

  // Description:
  // Defines the tissue class interaction matrix MrfParams(k,i,j)
  // where k = direction [1...6], 
  //       i = class of the current pixel t+1 
  //       j = class of neighbouring pixel t 
  void          SetMarkovMatrix(double value, int k, int j, int i);
 
  int           GetTotalNumberOfProbDataPtr();
  int           GetProbDataPtr(void **PointerList,int index); 
  int           GetProbDataIncYandZ(int* ProbDataIncY,int* ProbDataIncZ,int index);

  void          SetProbDataWeight(float value) {this->ProbDataWeight = value;}
  float         GetProbDataWeight(){return this->ProbDataWeight;} 

  int           GetTotalNumberOfClasses(bool flag); // if flag is set => includes subclasses of type SUPERCLASS
  int           GetAllLabels(short *LabelList, int result,int Max); // Gets all labels from the Substructures
  void          LabelAllSuperClasses(short *TakenLabelList, int Max);

  //BTX
  void**        GetClassList() {return this->ClassList;}
  void*         GetClassListEntry(int i) {return this->ClassList[i];}
  
  classType*    GetClassListType() {return this->ClassListType;}
  classType     GetClassType(void* active);
  //ETX

  vtkImageEMLocalSuperClass* GetParentClass() {return this->ParentClass;}

  int           GetTissueDefinition(int *LabelList, double** LogMu, double ***InvLogCov, int index);

  double        GetMrfParams(int k,int j,int i) {return this->MrfParams[k][j][i];}
  //BTX
  double***     GetMrfParams() {return this->MrfParams;}
  //ETX

  void AddSubClass(vtkImageEMLocalClass* ClassData, int index) {this->AddSubClass((void*) ClassData, CLASS, index); }
  void AddSubClass(vtkImageEMLocalSuperClass* ClassData, int index) {ClassData->ParentClass = this; this->AddSubClass((void*) ClassData, SUPERCLASS, index);}
  

  // Print Functions 
  // Description:
  // Print out the result after how many steps  (-1 == just last result, 0 = No Printing, i> 0 => every i-th slice )
  vtkGetMacro(PrintFrequency, int);
  vtkSetMacro(PrintFrequency, int);

  vtkGetMacro(PrintBias, int);
  vtkSetMacro(PrintBias, int);

  vtkGetMacro(PrintLabelMap, int);
  vtkSetMacro(PrintLabelMap, int);  

  vtkImageEMLocalSuperClass() {this->CreateVariables();}
  ~vtkImageEMLocalSuperClass() {this->DeleteSuperClassVariables();}

protected:
  vtkImageEMLocalSuperClass(const vtkImageEMLocalSuperClass&) {};
  void DeleteSuperClassVariables();
  void CreateVariables();

  void operator=(const vtkImageEMLocalSuperClass&) {};
  void ExecuteData(vtkDataObject *);   

  void AddSubClass(void* ClassData, classType initType, int index);

  int           NumClasses;             // Number of Sub classes -> Importan for dim of Class List
  void **       ClassList;              // List of Sub Classes
  classType*    ClassListType;
  vtkImageEMLocalSuperClass* ParentClass;    // The parent of this super class if ParentClass == NULL => does not have a parent 
  double***     MrfParams;              // Markov Model Parameters: Matrix3D mrfparams(this->NumClasses,this->NumClasses,4);

  int PrintFrequency;    // Print out the result after how many steps  (-1 == just last result, 0 = No Printing, i> 0 => every i-th slice )
  int PrintBias;         // Should the bias be printed too (Only works for GE)
  int PrintLabelMap;     // Print out inbetween label map   

};
#endif











