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
// .NAME vtkImageEditor - Applies editing effects to volumes.
// .SECTION Description
// vtkImageEditor allows a volume (vtkImageData) to be edited by applying
// filters derived from vtkImageToImageFilter to the data set on a 3D,
// single slice, or multi-slice basis.  There is a single level of undo to
// restore the volume to its previous value.  The effects can be implemented
// by passing the data through a pipelined chain of filters by specifying the
// first and last filters in the chain using the SetFirstFilter() and 
// SetLastFilter().  The LastFilter can equal the FirstFilter to use a chain
// of length 1.  The usefulness of this class lies in its abstraction of the
// ability to Undo() an effect, and to apply the effect to the entire volume,
// or just one slice, or to every slice one at a time. The slices may be
// oriented in any of the 3 possible directions for a volume.

#ifndef __vtkImageEditor_h
#define __vtkImageEditor_h

//#include <iostream.h>
//#include <fstream.h>
#include "vtkProcessObject.h"
#include "vtkImageData.h"
#include "vtkImageToImageFilter.h"
#include "vtkImageReplaceRegion.h"
#include "vtkImageReformatIJK.h"
#include "vtkImageCopy.h"
#include "vtkIntArray.h"
#include "vtkSlicer.h"

//  Dimension
#define EDITOR_DIM_SINGLE 1
#define EDITOR_DIM_MULTI  2
#define EDITOR_DIM_3D     3

class vtkCallbackCommand;

class VTK_SLICER_BASE_EXPORT vtkImageEditor : public vtkProcessObject
{
public:
  static vtkImageEditor *New();
  vtkTypeMacro(vtkImageEditor,vtkProcessObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set/Get the input data set for the image pipeline.
  // Use the UseInput() function to specify whether to use this input.
  void SetInput(vtkImageData *input);
  vtkImageData *GetInput();

  // Description:
  // When on, then the input data set using the SetInput() function
  // is processed.  When off, the current output data set is used
  // as input to the next processing effect.
  vtkGetMacro(UseInput, int);
  vtkSetMacro(UseInput, int);
  vtkBooleanMacro(UseInput, int);

  // Description:
  // Set/Get the scope of the effect.  The effect can be applied to
  // the entire volume, or just one slice, or to every slice one at a time.
  vtkSetMacro(Dimension, int);
  vtkGetMacro(Dimension, int);
  void SetDimensionToSingle() {this->SetDimension(EDITOR_DIM_SINGLE);};
  void SetDimensionToMulti()  {this->SetDimension(EDITOR_DIM_MULTI);};
  void SetDimensionTo3D()     {this->SetDimension(EDITOR_DIM_3D);};
  char* GetDimensionString();

  // Description:
  // Specify which slice to edit when the Dimension is set to Single.
  vtkGetMacro(Slice, int);
  vtkSetMacro(Slice, int);
  
  // Description:
  // Set/Get the orientation and scan order of the slices in the input.
  // Possible strings are SI or IS (for axial slices), LR or RL
  // (for sagittal), and AP or PA (for coronal).
  vtkSetStringMacro(InputSliceOrder);
  vtkGetStringMacro(InputSliceOrder);

  // Description:
  // Set/Get the orientation of the output slice when the Dimension is
  // set to Single or Multi.  Possibilities are IS, LR, or PA.
  // For example, IS would process the axial slices, while LR would
  // process sagittal slices.  This produces different results for
  // effects that depend on each voxel's neighborhood, such as Erode,
  // but not Threshold.
  vtkSetStringMacro(OutputSliceOrder);
  vtkGetStringMacro(OutputSliceOrder);

  // Description:
  // Get whether the Undo() function can be called.
  // A value of 1 indicates the most recently applied effect is undoable,
  // while 0 indicates not.
  vtkGetMacro(Undoable, int);
  vtkSetMacro(Undoable, int);

  // Description:
  // Undo the last call of the Apply() function to return the output
  // volume to the values it had.  The Undoable flag indicates when
  // Undo may be called.
  void Undo();

  // Description:
  // Apply the effect by filtering the data with a pipeline of
  // filters that begin with FirstFilter and end with LastFilter.
  // The Last and First can be the same for an effect that can
  // be achieved with one filter.
  void Apply();
  void Apply(vtkImageToImageFilter *firstFilter,
    vtkImageToImageFilter *lastFilter);
  vtkSetObjectMacro(FirstFilter, vtkImageToImageFilter);
  vtkGetObjectMacro(FirstFilter, vtkImageToImageFilter);
  vtkSetObjectMacro(LastFilter, vtkImageToImageFilter);
  vtkGetObjectMacro(LastFilter, vtkImageToImageFilter);

  // Description:
  // Apply the effect to a clipped portion of the input.
  vtkGetMacro(Clip, int);
  vtkSetMacro(Clip, int);
  vtkBooleanMacro(Clip, int);
  void SetClipExtent(int extent[6]);
  void SetClipExtent(int minX, int maxX, int minY, int maxY,
                            int minZ, int maxZ);
  void GetClipExtent(int extent[6]);
  int *GetClipExtent() {return this->ClipExtent;}

  // Description:
  // Set/Get the output of the effect.
  // Users should not use UndoOutput unless they know what they're doing.
  // The UndoOutput data set exists for internal use of undoing 3D and
  // multi-slice effects.
  vtkGetObjectMacro(Output, vtkImageData);
  vtkSetObjectMacro(Output, vtkImageData);
  vtkSetObjectMacro(UndoOutput, vtkImageData);
  vtkGetObjectMacro(UndoOutput, vtkImageData);

  // Description:
  // Time of Apply in seconds.  The RunTime is the actual time of
  // the algorithmic processing, while the TotalTime also includes
  // overhead.
  vtkGetMacro(RunTime, float);
  vtkGetMacro(TotalTime, float);
    
protected:
  vtkImageEditor();
  ~vtkImageEditor();
  vtkImageEditor(const vtkImageEditor&) {};
  void operator=(const vtkImageEditor&) {};

  // When "Dimension" is "Single", then the input slice is copied
  // to "Region" before the effect is applied.  Then it can be restored
  // from Region to perform an Undo.
  vtkGetObjectMacro(Region, vtkImageData);
  vtkSetObjectMacro(Region, vtkImageData);

  // "Indices" stores the voxel index into the input volume for each
  // pixel in "Region"
  vtkGetObjectMacro(Indices, vtkIntArray);
  vtkSetObjectMacro(Indices, vtkIntArray);

  // Swap the Output and UndoOutput pointers.
  // For example, this is how Undo is achieved after a 3D effect.
  void SwapOutputs();

  // Pointers to the first and last filters in the pipeline for
  // performing the effect.
  vtkImageToImageFilter *FirstFilter;
  vtkImageToImageFilter *LastFilter;

  int Slice;                // slice number for Dimension=Single
  int Dimension;            // 3D, Single, or Multi
  int UndoDimension;        // the Dimension of the effect to undo
  char *InputSliceOrder;    // slice order of the volume to be affected
  char *OutputSliceOrder;   // slice order of the output volume
  int Undoable;             // flag to indicate if can undo
  int UseInput;             // flag to indicate whether to use the input
  int Clip;                 // flag to indicate whether to clip
  int ClipExtent[6];        // the extent to clip to when clipping

  float RunTime;            // measure of effect processing time in seconds
  float TotalTime;          // measure of effect and overhead in seconds

  vtkImageData *Output;     // the output of the most recent effect
  vtkImageData *UndoOutput; // the output if we undo a 3D or Multi slice effect

  vtkImageData *Region;     // the output if we undo a Single slice effect
  vtkIntArray *Indices;     // the indices for how Region was extracted 

  // Callback registered with the ProgressObserver.
  static void ProgressCallbackFunction(vtkObject*, unsigned long, void*,
                                       void*);
  // The observer to report progress from the internal writer.
  vtkCallbackCommand* ProgressObserver;  
};

#endif


