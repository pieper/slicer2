/*=auto=========================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
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
// .NAME vtkImageEditor - Writes images to files.
// .SECTION Description
// vtkImageEditor writes images to files with any data type. The data type of
// the file is the same scalar type as the input.  The dimensionality
// determines whether the data will be written in one or multiple files.
// This class is used as the superclass of most image writing classes 
// such as vtkBMPWriter etc. It supports streaming.

#ifndef __vtkImageEditor_h
#define __vtkImageEditor_h

#include <iostream.h>
#include <fstream.h>
#include "vtkProcessObject.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkImageReplaceRegion.h"
#include "vtkImageReformatIJK.h"
#include "vtkImageCopy.h"
#include "vtkIntArray.h"

//  Dimension
#define EDITOR_DIM_SINGLE 1
#define EDITOR_DIM_MULTI  2
#define EDITOR_DIM_3D     3

class VTK_EXPORT vtkImageEditor : public vtkProcessObject
{
public:
  static vtkImageEditor *New();
  const char *GetClassName() {return "vtkImageEditor";};
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set/Get the input object from the image pipeline.
  void SetInput(vtkImageData *input);
  vtkImageData *GetInput();

  // Description:
  // Dimension
  vtkSetMacro(Dimension, int);
  vtkGetMacro(Dimension, int);
  void SetDimensionToSingle() {this->SetDimension(EDITOR_DIM_SINGLE);};
  void SetDimensionToMulti()  {this->SetDimension(EDITOR_DIM_MULTI);};
  void SetDimensionTo3D()     {this->SetDimension(EDITOR_DIM_3D);};
  char* GetDimensionString();

  // Description:
  vtkGetMacro(UseInput, int);
  vtkSetMacro(UseInput, int);
  vtkBooleanMacro(UseInput, int);

  // Description:
  // Slice to edit when Dimension is 2D
  vtkGetMacro(Slice, int);
  vtkSetMacro(Slice, int);
  
  // Description:
  // Orientation of slices to edit
  vtkSetStringMacro(InputSliceOrder);
  vtkGetStringMacro(InputSliceOrder);
  vtkSetStringMacro(OutputSliceOrder);
  vtkGetStringMacro(OutputSliceOrder);

  // Description:
  // 1 when can undo, else 0
  vtkGetMacro(Undoable, int);
  vtkSetMacro(Undoable, int);

  // Description:
  // Undo last filters
  void Undo();

  // Description:
  // Apply filters
  void Apply();
  void Apply(vtkImageToImageFilter *firstFilter,
    vtkImageToImageFilter *lastFilter);

  // Description:
  //
  vtkSetObjectMacro(FirstFilter, vtkImageToImageFilter);
  vtkGetObjectMacro(FirstFilter, vtkImageToImageFilter);
  vtkSetObjectMacro(LastFilter, vtkImageToImageFilter);
  vtkGetObjectMacro(LastFilter, vtkImageToImageFilter);

  // Description:
  // The whole extent of the input has to be set explicitely.
  void SetClipExtent(int extent[6]);
  void SetClipExtent(int minX, int maxX, int minY, int maxY,
                            int minZ, int maxZ);
  void GetClipExtent(int extent[6]);
  int *GetClipExtent() {return this->ClipExtent;}

  // Description:
  vtkGetMacro(Clip, int);
  vtkSetMacro(Clip, int);
  vtkBooleanMacro(Clip, int);

  // Description:
  // Output
  vtkGetObjectMacro(Output, vtkImageData);
  vtkSetObjectMacro(Output, vtkImageData);
  vtkSetObjectMacro(UndoOutput, vtkImageData);
  vtkGetObjectMacro(UndoOutput, vtkImageData);

  // Description:
  // Time of Apply in seconds
  vtkGetMacro(RunTime, float);
  vtkGetMacro(TotalTime, float);
	
protected:
  vtkImageEditor();
  ~vtkImageEditor();
  vtkImageEditor(const vtkImageEditor&) {};
  void operator=(const vtkImageEditor&) {};

  vtkGetObjectMacro(Region, vtkImageData);
  vtkSetObjectMacro(Region, vtkImageData);
  vtkGetObjectMacro(Indices, vtkIntArray);
  vtkSetObjectMacro(Indices, vtkIntArray);

  void SwapOutputs();

  vtkImageToImageFilter *FirstFilter;
  vtkImageToImageFilter *LastFilter;

  int Slice;
  int Dimension;
  int UndoDimension;
  char *InputSliceOrder;
  char *OutputSliceOrder;
  int Undoable;
  int UseInput;
  int Clip;
  int ClipExtent[6];
  float RunTime;
  float TotalTime;

  vtkImageData *Output;
  vtkImageData *UndoOutput;

  vtkImageData *Region;
  vtkIntArray *Indices;
};

#endif


