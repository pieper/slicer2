/*=auto=========================================================================
Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to slicer@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy (but NOT distribute) this
software and its documentation for any NON-COMMERCIAL purpose, provided
that existing copyright notices are retained verbatim in all copies.
The authors grant permission to modify this software and its documentation 
for any NON-COMMERCIAL purpose, provided that such modifications are not 
distributed without the explicit consent of the authors and that existing
copyright notices are retained in all copies. Some of the algorithms
implemented by this software are patented, observe all applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
=========================================================================auto=*/
#ifndef __vtkImageEditorEffects_h
#define __vtkImageEditorEffects_h

#include <fstream.h>
#include <stdlib.h>
#include <iostream.h>

#include "vtkImageEditor.h"
#include "vtkPoints.h"

class VTK_EXPORT vtkImageEditorEffects : public vtkImageEditor
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


