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
// .NAME vtkImageEditorEffects - Implementation of effects that 
// can be performed on volumes when editing.
// .SECTION Description
//
//  Draw, threshold, clear, change island, measure island, 
//  dilate, erode, and more.
//

#ifndef __vtkImageEditorEffects_h
#define __vtkImageEditorEffects_h

//#include <fstream.h>
#include <stdlib.h>
//#include <iostream.h>

#include "vtkImageEditor.h"
#include "vtkPoints.h"
#include "vtkSlicer.h"

class VTK_SLICER_BASE_EXPORT vtkImageEditorEffects : public vtkImageEditor
{
public:
  static vtkImageEditorEffects *New();
  vtkTypeMacro(vtkImageEditorEffects,vtkImageEditor);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  void Clear();
  void Threshold(float min, float max, float in, float out, 
    int replaceIn, int replaceOut);
  void Draw(int label, vtkPoints *pts, int radius, char *shape);
  void Erode(float fg, float bg, int neighbors, int iterations);
  void Dilate(float fg, float bg, int neighbors, int iterations);
  void ErodeDilate(float fg, float bg, int neighbors, int iterations);
  void DilateErode(float fg, float bg, int neighbors, int iterations);
  void IdentifyIslands(int bg, int fgMin, int fgMax);
  void RemoveIslands(int bg, int fgMin, int fgMax, int minSize);
  void ChangeIsland(int newLabel, int xSeed, int ySeed, int zSeed);
  void MeasureIsland(int xSeed, int ySeed, int zSeed);
  void SaveIsland(int xSeed, int ySeed, int zSeed);
  void ChangeLabel(int inputLabel, int outputLabel);

  void LabelVOI(int c1x, int c1y, int c1z, int c2x, int c2y, int c2z, int method);
  
  vtkGetMacro(IslandSize, int);
  vtkGetMacro(LargestIslandSize, int);

protected:

  vtkImageEditorEffects();
  ~vtkImageEditorEffects();
  vtkImageEditorEffects(const vtkImageEditorEffects&) {};
  void operator=(const vtkImageEditorEffects&) {};

  int IslandSize;
  int LargestIslandSize;
};

#endif


