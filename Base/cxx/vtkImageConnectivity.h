/*=auto=========================================================================

(c) Copyright 2002 Massachusetts Institute of Technology

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
// .NAME vtkImageConnectivity - Identify and process islands of similar pixels
// .SECTION Description
//  The input data type must be shorts.

#ifndef __vtkImageConnectivity_h
#define __vtkImageConnectivity_h

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
    vtkSetMacro(Background, float);
    vtkGetMacro(Background, float);
    vtkSetMacro(MinForeground, float);
    vtkGetMacro(MinForeground, float);
    vtkSetMacro(MaxForeground, float);
    vtkGetMacro(MaxForeground, float);

protected:
    vtkImageConnectivity();
    ~vtkImageConnectivity(){};
    vtkImageConnectivity(const vtkImageConnectivity&) {};
    void operator=(const vtkImageConnectivity&) {};

    float Background;
    float MinForeground;
    float MaxForeground;
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



