/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageConnectivity.h,v $
  Date:      $Date: 2006/02/14 20:40:11 $
  Version:   $Revision: 1.14 $

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
  const char* GetFunctionString();

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
    vtkImageConnectivity(const vtkImageConnectivity&);
    void operator=(const vtkImageConnectivity&);

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



