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
// .NAME vtkImageConnectivity - Identify and process islands of similar pixels
// .SECTION Description
//  The input data type must be shorts.

#ifndef __vtkImageConnectivity_h
#define __vtkImageConnectivity_h

#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkSlicer.h"

#define CONNECTIVITY_IDENTIFY 1
#define CONNECTIVITY_REMOVE 2
#define CONNECTIVITY_CHANGE 3
#define CONNECTIVITY_MEASURE 4
#define CONNECTIVITY_SAVE 5

class VTK_SLICER_BASE_EXPORT vtkImageConnectivity : public vtkImageToImageFilter
{
public:
    static vtkImageConnectivity *New();
  vtkTypeMacro(vtkImageConnectivity,vtkImageToImageFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

  // Function
  void SetFunction(int func) {
      this->function = func;};
  void SetFunctionToIdentifyIslands() {
      this->SetFunction(CONNECTIVITY_IDENTIFY);};
  void SetFunctionToRemoveIslands() {
      this->SetFunction(CONNECTIVITY_REMOVE);};
  void SetFunctionToChangeIsland() {
      this->SetFunction(CONNECTIVITY_CHANGE);};
  void SetFunctionToMeasureIsland() {
      this->SetFunction(CONNECTIVITY_MEASURE);};
  void SetFunctionToSaveIsland() {
      this->SetFunction(CONNECTIVITY_SAVE);};
  int GetFunction() {return this->function;}
  char* GetFunctionString();

    // outputs of MeasureIsland
    vtkGetMacro(IslandSize, int);
    vtkSetMacro(IslandSize, int);
    vtkGetMacro(LargestIslandSize, int);
    vtkSetMacro(LargestIslandSize, int);

  // Description:
    vtkGetMacro(SliceBySlice, int);
    vtkSetMacro(SliceBySlice, int);
    vtkBooleanMacro(SliceBySlice, int);

  vtkSetVector3Macro(Seed, int);
    vtkGetVector3Macro(Seed, int);

    vtkSetMacro(OutputLabel, int);
    vtkGetMacro(OutputLabel, int);
    vtkSetMacro(MinSize, int);
    vtkGetMacro(MinSize, int);
    vtkSetMacro(Background, short);
    vtkGetMacro(Background, short);
    vtkSetMacro(MinForeground, short);
    vtkGetMacro(MinForeground, short);
    vtkSetMacro(MaxForeground, short);
    vtkGetMacro(MaxForeground, short);

protected:
    vtkImageConnectivity();
    ~vtkImageConnectivity(){};
    vtkImageConnectivity(const vtkImageConnectivity&) {};
    void operator=(const vtkImageConnectivity&) {};

    short Background;
    short MinForeground;
    short MaxForeground;
    int LargestIslandSize;
    int IslandSize;
    int MinSize;
    int OutputLabel;
    int Seed[3];
    int function;
  int SliceBySlice;
    
    void ExecuteData(vtkDataObject *);
};

#endif



