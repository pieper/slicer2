/*=auto=========================================================================
Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
 
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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "vtkImageEditor.h"
#include "vtkObjectFactory.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageCopy.h"
#include "vtkImageClip.h"
#include "vtkImageConstantPad.h"

//------------------------------------------------------------------------------
vtkImageEditor* vtkImageEditor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEditor");
  if(ret)
  {
    return (vtkImageEditor*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEditor;
}

//----------------------------------------------------------------------------
vtkImageEditor::vtkImageEditor()
{
  int i;

  // SliceOrder can never be NULL
  this->InputSliceOrder = new char[3];
  strcpy(this->InputSliceOrder, "SI");
  this->OutputSliceOrder = new char[3];
  strcpy(this->OutputSliceOrder, "SI");

  this->UseInput = 1;
  this->Slice = 0;
  this->Clip = 0;  
  this->UndoDimension = this->Dimension = EDITOR_DIM_3D;
  this->Undoable = 0;
  this->RunTime = 0.0;
  this->TotalTime = 0.0;

  // By setting to the largest possible extent, an error will be generated
  // to warn the user if they attempt to clip without specifying the extent.
  // So do change this to avoid an error message!
  for (i=0; i<3; i++)
  {
    this->ClipExtent[i*2]   = -VTK_LARGE_INTEGER;
    this->ClipExtent[i*2+1] =  VTK_LARGE_INTEGER;
  }
  this->FirstFilter = NULL;
  this->LastFilter = NULL;
  this->Region = NULL;
  this->Indices = NULL;
  this->Output  = NULL;
  this->UndoOutput = NULL;
}

//----------------------------------------------------------------------------
vtkImageEditor::~vtkImageEditor()
{
  // We must delete any objects we created
  if (this->InputSliceOrder)
  {
    delete [] this->InputSliceOrder;
    this->InputSliceOrder = NULL;
  }
  if (this->OutputSliceOrder)
  {
    delete [] this->OutputSliceOrder;
    this->OutputSliceOrder = NULL;
  }

  // We must UnRegister any object that has a vtkSetObjectMacro
  if (this->FirstFilter != NULL) 
  {
    this->FirstFilter->UnRegister(this);
  }
  if (this->LastFilter != NULL) 
  {
    this->LastFilter->UnRegister(this);
  }
  if (this->Output != NULL) 
  {
    this->Output->UnRegister(this);
  }
  if (this->UndoOutput != NULL) 
  {
    this->UndoOutput->UnRegister(this);
  }
  if (this->Region != NULL) 
  {
    this->Region->UnRegister(this);
  }
  if (this->Indices != NULL) 
  {
    this->Indices->UnRegister(this);
  }
}

//----------------------------------------------------------------------------
void vtkImageEditor::PrintSelf(ostream& os, vtkIndent indent)
{
  int i;
  
  vtkProcessObject::PrintSelf(os,indent);

	os << indent << "RunTime:       " << this->RunTime << "\n";
	os << indent << "TotalTime:     " << this->TotalTime << "\n";
  os << indent << "Clip:          " << this->Clip << "\n";
  os << indent << "UseInput:      " << this->UseInput << "\n";
  os << indent << "Slice:         " << this->Slice << "\n";
  os << indent << "Undoable:      " << this->Undoable << "\n";
  os << indent << "Dimension:     " << this->GetDimensionString() << "\n";
  os << indent << "UndoDimension: " << this->UndoDimension << "\n";
  os << indent << "InputSliceOrder:   " <<
    (this->InputSliceOrder ? this->InputSliceOrder : "(none)") << "\n";
  os << indent << "OutputSliceOrder:   " <<
    (this->OutputSliceOrder ? this->OutputSliceOrder : "(none)") << "\n";

  os << "Clip Extent:\n";
  for (i = 0; i < 3; i++)
  {
    os << indent << ", " << this->ClipExtent[i*2]
       << "," << this->ClipExtent[i*2+1];
  }
  os << ")\n";

  // vtkSetObjectMacro
  os << indent << "FirstFilter: " << this->FirstFilter << "\n";
  if (this->FirstFilter)
  {
    this->FirstFilter->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "LastFilter:  " << this->LastFilter << "\n";
  if (this->LastFilter)
  {
    this->LastFilter->PrintSelf(os,indent.GetNextIndent());
  }

  // Outputs
  os << indent << "Output:     " << this->Output << "\n";
  if (this->Output)
  {
    this->Output->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "UndoOutput:     " << this->UndoOutput << "\n";
  if (this->UndoOutput)
  {
    this->UndoOutput->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "Region:     " << this->Region << "\n";
  if (this->Region)
  {
    this->Region->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "Indices:     " << this->Indices << "\n";
  if (this->Indices)
  {
    this->Indices->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
void vtkImageEditor::SetClipExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    if (this->ClipExtent[idx] != extent[idx])
    {
      this->ClipExtent[idx] = extent[idx];
      this->Modified();
    }
  }  
}

//----------------------------------------------------------------------------
void vtkImageEditor::SetClipExtent(int minX, int maxX,
                                   int minY, int maxY,
                                   int minZ, int maxZ)
{
  int extent[6];
  extent[0] = minX;  extent[1] = maxX;
  extent[2] = minY;  extent[3] = maxY;
  extent[4] = minZ;  extent[5] = maxZ;
  this->SetClipExtent(extent);
}
 
//----------------------------------------------------------------------------
void vtkImageEditor::GetClipExtent(int extent[6])
{
  int idx;
  for (idx = 0; idx < 6; ++idx)
  {
    extent[idx] = this->ClipExtent[idx];
  }
}

//----------------------------------------------------------------------------
char* vtkImageEditor::GetDimensionString()
{
  switch (this->Dimension) 
  {
    case EDITOR_DIM_MULTI:  return "Multi";
    case EDITOR_DIM_SINGLE: return "Single";
    case EDITOR_DIM_3D:     return "3D";
    default: return "ERROR";
  }
}

//----------------------------------------------------------------------------
void vtkImageEditorProgress(void *arg)
{
  // This function is a callback for the FirstFilter for the purpose
  // of reporting processing progress to the user.
  
  vtkImageEditor *self = (vtkImageEditor *)(arg);
  vtkImageToImageFilter *filter = self->GetFirstFilter();
  if (filter)
  {
    self->UpdateProgress(filter->GetProgress());
  }
}


//----------------------------------------------------------------------------
void vtkImageEditor::SetInput(vtkImageData *input)
{
  this->vtkProcessObject::SetNthInput(0, input);
}

//----------------------------------------------------------------------------
vtkImageData *vtkImageEditor::GetInput()
{
  if (this->NumberOfInputs < 1)
  {
    return NULL;
  }
  return (vtkImageData *)(this->Inputs[0]);
}

//----------------------------------------------------------------------------
void vtkImageEditor::SwapOutputs()
{
  vtkImageData *tmp = this->Output;
  this->Output = this->UndoOutput;
  this->UndoOutput = tmp;
}

//----------------------------------------------------------------------------
void vtkImageEditor::Apply(vtkImageToImageFilter *firstFilter,
                           vtkImageToImageFilter *lastFilter)
{
  this->SetFirstFilter(firstFilter);
  this->SetLastFilter(lastFilter);
  this->Apply();
}

//----------------------------------------------------------------------------
void vtkImageEditor::Apply()
{
  vtkImageData *input;
  vtkImageReformatIJK *reformat;
  vtkImageReplaceRegion *replace;
  int s, sMax, newOutput=0;
  int wholeExtent[6], inExt[6], outExt[6];
  clock_t tStartTotal, tStart;

  // Error checking
  if (this->GetInput() == NULL)
  {
    vtkErrorMacro(<<"Apply: Input not set!");
    return;
  }
  if (this->UseInput == 0 && this->Output == NULL)
  {
    vtkErrorMacro(<<"Apply: Output not set!");
    return;
  }
  if (this->FirstFilter == NULL)
  {
    vtkErrorMacro(<<"Apply: FirstFilter not set!");
    return;
  }
  if (this->LastFilter == NULL)
  {
    vtkErrorMacro(<<"Apply: LastFilter not set!");
    return;
  }
  
  // Attach the Progress callback to the first filter, unless Multi-slice.
  // Multi-slice progress is computed after each slice. 
  if (this->Dimension != EDITOR_DIM_MULTI)
  {
    this->FirstFilter->SetProgressMethod(
      vtkImageEditorProgress, (void *)this);
  }    
 
  // Start progress reporting
  if (this->StartMethod)
  {
    (*this->StartMethod)(this->StartMethodArg);
  }
  
  // Start measuring the total time that "Apply" takes.
  tStartTotal = clock();
  
  ///////////////////////////////////////////////////////////////////////////
  // Here are the processing steps for each Dimension.
  //
  // Single:
  //   1.) If UseInput=1
  //         If Output has a different extent from Input
  //           Delete Output and create a new one of all 0's
  //   2.) Extract Region from Input if UseInput=1, else Output.
  //   3.) Region->filter->replace-in-Output
  //   4.) Delete UndoOutput.
  //   5.) Allow Undo.
  //
  // Multi:
  //   1.) If UseInput=1
  //         If UndoOutput has a different extent from Input
  //           Delete UndoOutput and create a new one of all 0's
  //       Else
  //         If UndoOutput has a different extent from Output
  //           Delete UndoOutput and create a new one of all 0's
  //   2.) If UseInput=1, do 1 slice at a time:
  //         Input->filter->replace-in-UndoOutput
  //       Else
  //         Output->filter->replace-in-UndoOutput
  //   3.) Swap UndoOutput with Output
  //   4.) If Output and UndoOutput have differing extents, delete UndoOutput.
  //   5.) Allow Undo if UndoOutput != NULL
  //
  // 3D:
  //   1.) If UseInput=1
  //         Input->filter->UndoOutput
  //       Else
  //         Output->filter->UndoOutput
  //   3.) Swap UndoOutput with Output
  //   4.) If Output and UndoOutput have differing extents, delete UndoOutput.
  //   5.) Allow Undo if UndoOutput != NULL
  //
  //
  // To reuse some code, it is organized as follows:
  // -----------------------------------------------
  // If Single
  //       If UseInput=1
  //         If Output has a different extent from Input
  //           Delete Output and create a new one of all 0's
  // 
  // If Multi
  //       If UseInput=1
  //         If UndoOutput has a different extent from Input
  //           Delete UndoOutput and create a new one of all 0's
  //       Else
  //         If UndoOutput has a different extent from Output
  //           Delete UndoOutput and create a new one of all 0's
  //
  // If UseInput=1
  //   input = Input
  // Else
  //   input = Output
  //
  // Switch Dimension
  //   Single:
  //     input->extract-slice->filter->replace-in-Output
  //     Delete UndoOutput.
  //     Allow Undo.
  //
  //    Multi:
  //      Do 1 slice at a time:
  //        input->filter->replace-in-UndoOutput
  //
  //    3D:
  //      input->filter->UndoOutput
  //
  // If Multi or 3D
  //   Swap UndoOutput with Output
  //   If Output and UndoOutput have differing extents, delete UndoOutput.
  //   Allow Undo if UndoOutput != NULL
  //
  ///////////////////////////////////////////////////////////////////////////

  // Check Output for Single-slice
  if (this->Dimension == EDITOR_DIM_SINGLE)
  {
    newOutput = 0;
    
    // If the output does not exist, or has the wrong extent,
    // then we'll need to create a new one.

    if (this->UseInput)
    {
      if (this->Output == NULL)
      {
        newOutput = 1;
      }
      else if (this->UseInput == 1)
      {
        // Make sure Output has same extent as Input
        this->GetInput()->GetWholeExtent(inExt);
        this->Output->GetWholeExtent(outExt);
        if ((inExt[0] != outExt[0])||(inExt[1] != outExt[1])||
            (inExt[2] != outExt[2])||(inExt[3] != outExt[3])||
            (inExt[4] != outExt[4])||(inExt[5] != outExt[5]))
        {
          newOutput = 1;
        }
      }
      if (newOutput)
      {
        vtkImageCopy *copy = vtkImageCopy::New();
        copy->SetInput(this->GetInput());
        copy->ClearOn();
        copy->Update();
        this->SetOutput(copy->GetOutput());
        // NOTE:
        // copy->Delete() performs copy->SetInput(NULL) 
        // but not                 copy->SetOutput(NULL)
        // so I have to detach the output here:
        copy->SetOutput(NULL);
        copy->Delete();
      }
    }
  }

  // Check UndoOutput for Multi-slice
  if (this->Dimension == EDITOR_DIM_MULTI)
  {
    newOutput = 0;
    
    // If the UndoOutput does not exist, or has the wrong extent,
    // then we'll need to create a new one.
    
    // UseInput=1
    if (this->UseInput)
    {
      if (this->UndoOutput == NULL)
      {
        newOutput = 1;
      }
      else
      {
        // Make sure UndoOutput has same extent as Input
        this->GetInput()->GetWholeExtent(inExt);
        this->UndoOutput->GetWholeExtent(outExt);
        if ((inExt[0] != outExt[0])||(inExt[1] != outExt[1])||
            (inExt[2] != outExt[2])||(inExt[3] != outExt[3])||
            (inExt[4] != outExt[4])||(inExt[5] != outExt[5]))
        {
          newOutput = 1;
        }
      }
      if (newOutput)
      {
        vtkImageCopy *copy = vtkImageCopy::New();
        copy->SetInput(this->GetInput());
        copy->ClearOn();
        copy->Update();
        this->SetUndoOutput(copy->GetOutput());
        copy->SetOutput(NULL);
        copy->Delete();
      }
    }
    // UseInput=0
    else
    {
      if (this->UndoOutput == NULL)
      {
        newOutput = 1;
      }
      else
      {
        // Make sure UndoOutput has same extent as Output
        this->Output->GetWholeExtent(inExt);
        this->UndoOutput->GetWholeExtent(outExt);
        if ((inExt[0] != outExt[0])||(inExt[1] != outExt[1])||
            (inExt[2] != outExt[2])||(inExt[3] != outExt[3])||
            (inExt[4] != outExt[4])||(inExt[5] != outExt[5]))
        {
          newOutput = 1;
        }
      }
      if (newOutput)
      {
        vtkImageCopy *copy = vtkImageCopy::New();
        copy->SetInput(this->Output);
        copy->ClearOn();
        copy->Update();
        this->SetUndoOutput(copy->GetOutput());
        copy->SetOutput(NULL);
        copy->Delete();
      }
    }
  }

  // Get the input, which is either an externally set volume,
  // or the Output of the last effect.
  if (this->UseInput)
  {
    input = this->GetInput();
  }
  else 
  {
    input = this->Output;
  }

    vtkImageData *data = this->Output;

  // Depending on the scope of the effect...
  switch(this->Dimension)
  {

  // Single-slice
  case EDITOR_DIM_SINGLE:

    // We will process a slice from the input,
    // and copy the processed slice into the Output.

    // Make "Undo" possible, and extract the slice to affect
    // from the input.
    reformat = vtkImageReformatIJK::New();
    if (this->UseInput)
    {
      // For undo, we need to store the region we're about to overwrite
      // in the current Output.

      // Use "reformat" to extract the slice.
      reformat->SetInput(this->Output);
      reformat->SetInputOrderString(this->InputSliceOrder);
      reformat->SetOutputOrderString(this->OutputSliceOrder);
      reformat->ComputeTransform();
      reformat->ComputeOutputExtent();
      reformat->SetSlice(this->Slice);
      reformat->Update();

      // And store the extracted slice as "Region".
      // Also store the indices that we'll need to restore the region.
      this->SetRegion(reformat->GetOutput());
      this->SetIndices(reformat->GetIndices());
      reformat->SetOutput(NULL);
      reformat->Delete();

      // Now extract the slice from the input that we're going to apply
      // the effect to.
      reformat = vtkImageReformatIJK::New();
      reformat->SetInput(input);
      reformat->SetInputOrderString(this->InputSliceOrder);
      reformat->SetOutputOrderString(this->OutputSliceOrder);
      reformat->ComputeTransform();
      reformat->ComputeOutputExtent();
      reformat->SetSlice(this->Slice);
      reformat->Update();
    }
    else
    {
      // Since the input is also the output, we don't need to perform
      // the second reformat done in the above case.
      reformat->SetInput(this->Output);
      reformat->SetInputOrderString(this->InputSliceOrder);
      reformat->SetOutputOrderString(this->OutputSliceOrder);
      reformat->ComputeTransform();
      reformat->ComputeOutputExtent();
      reformat->SetSlice(this->Slice);
      reformat->Update();

      this->SetRegion(reformat->GetOutput());
      this->SetIndices(reformat->GetIndices());
    }

    // Connect the reformatted slice to the filters, and execute.
    this->FirstFilter->SetInput(reformat->GetOutput());
    tStart = clock();
    this->LastFilter->Update();
    this->RunTime = (float)(clock() - tStart) / CLOCKS_PER_SEC;

    // Copy the 2D from the effect-processing pipeline into the 3D Output 
    // using the Indices.  
    // NOTE: vtkImageReplaceRegion derived from vtkImageInPlace, which is 
    // not in place unless Output's ReleaseDataFlag is on.
    this->Output->ReleaseDataFlagOn();
    replace = vtkImageReplaceRegion::New();
    replace->SetInput(this->Output);
    replace->SetIndices(reformat->GetIndices());
    replace->SetRegion(this->LastFilter->GetOutput());
    replace->Update();

    // Reset Output pointer
    this->SetOutput(replace->GetOutput());

    // Detach Output from sources
    replace->SetOutput(NULL);
    replace->SetRegion(NULL);
    replace->Delete();

    reformat->SetOutput(NULL);
    reformat->Delete();

    // Delete the UndoOutput (by reducing its reference count to 0).
    // We will undo by copying one slice, rather than storing the
    // entire volume.
    this->SetUndoOutput(NULL);

    // Allow undo
    this->Undoable = 1;
    this->UndoDimension = this->Dimension;
    break;
 

  // Multi-slice
  case EDITOR_DIM_MULTI:

    // One slice at a time, we will process a slice from the input,
    // and copy the processed slice into UndoOutput.
    
    // Now safe to begin
    reformat = vtkImageReformatIJK::New();
    reformat->SetInput(input);
    reformat->SetInputOrderString(this->InputSliceOrder);
    reformat->SetOutputOrderString(this->OutputSliceOrder);
    reformat->ComputeTransform();
    reformat->ComputeOutputExtent();
    sMax = reformat->GetNumSlices(); 

    // Filter each slice one at a time
    this->RunTime = 0.0;
    for (s=0; s < sMax; s++)
    {
      // Extract the slice
      reformat->SetSlice(s);
      reformat->Update();

      // Filter the slice
      tStart = clock();
      this->FirstFilter->SetInput(reformat->GetOutput());
      this->LastFilter->Update();
      this->RunTime += (float)(clock() - tStart);

      // Copy the filtered slice into UndoOutput
      // NOTE: vtkImageReplaceRegion derived from vtkImageInPlace, which is 
      // not in place unless UndoOutput's ReleaseDataFlag is on.
      replace = vtkImageReplaceRegion::New();
      replace->SetInput(this->UndoOutput);
      replace->SetRegion(this->LastFilter->GetOutput());
      replace->SetIndices(reformat->GetIndices());
      this->UndoOutput->ReleaseDataFlagOn();
      replace->Update();
      this->SetUndoOutput(replace->GetOutput());
      replace->SetOutput(NULL);
      replace->SetRegion(NULL);
      replace->Delete();

      // Update progress
      if (sMax > 1)
      { 
        this->UpdateProgress((float)s/(float)(sMax-1));
      }
    }
    this->RunTime /= (float)CLOCKS_PER_SEC;

    reformat->SetOutput(NULL);
    reformat->Delete();
   break;

  // 3D
  case EDITOR_DIM_3D:
    
    // Optionally clip, and attach output to source
    if (this->Clip)
    {
      input->GetWholeExtent(wholeExtent);

      vtkImageClip *clip = vtkImageClip::New();
      clip->SetInput(input);
      clip->ClipDataOff();
      clip->SetOutputWholeExtent(this->ClipExtent);
   
      this->FirstFilter->SetInput(clip->GetOutput());

      vtkImageConstantPad *pad = vtkImageConstantPad::New();
      pad->SetInput(this->LastFilter->GetOutput());
      pad->SetOutputWholeExtent(wholeExtent);
      
      // Execute
      clip->Update();
      tStart = clock();
      this->LastFilter->Update();
      this->RunTime = (float)(clock() - tStart) / CLOCKS_PER_SEC;
      pad->Update();

      this->SetUndoOutput(pad->GetOutput());
 
      clip->SetOutput(NULL);
      clip->Delete(); 
      pad->SetOutput(NULL);
      pad->Delete();
    }
    else
    {
      this->FirstFilter->SetInput(input);
      
      // Execute
      input->Update();
      tStart = clock();
      this->LastFilter->Update();
      this->RunTime = (float)(clock() - tStart) / CLOCKS_PER_SEC;

      this->SetUndoOutput(this->LastFilter->GetOutput());
    } 
    break;
  }


  // Determine if can undo
  if (this->Dimension == EDITOR_DIM_MULTI || 
      this->Dimension == EDITOR_DIM_3D)
  {
    // ERROR
    // If the user does 2 consecutive Apply()'s too quickly,
    // then the UndoOutput==NULL here.
    // I can avoid the error by just ignoring the second
    // Apply, as I'm implementing here, but this should be 
    // handled better in the future.
    if (this->UndoOutput == NULL)
    {
      vtkDebugMacro(<<"UndoOutput=NULL because called Apply twice too fast!");
      this->TotalTime = 0;
      if (this->EndMethod)
      {
        (*this->EndMethod)(this->EndMethodArg);
      } 
      return;
    }

    this->SwapOutputs();

    // Make sure UndoOutput has same extent as Output, else delete it.
    if (this->UndoOutput != NULL)
    {
      this->Output->GetWholeExtent(inExt);
      this->UndoOutput->GetWholeExtent(outExt);
      if ((inExt[0] != outExt[0])||(inExt[1] != outExt[1])||
          (inExt[2] != outExt[2])||(inExt[3] != outExt[3])||
          (inExt[4] != outExt[4])||(inExt[5] != outExt[5]))
      {
        this->SetUndoOutput(NULL);
      }
    }

    // Allow Undo if UndoOutput exists
    if (this->UndoOutput != NULL)
    {
      this->Undoable = 1;
      this->UndoDimension = this->Dimension;
    }
  }

  // Measure total processing time, including overhead
  this->TotalTime = (float)(clock() - tStartTotal) / CLOCKS_PER_SEC;

  // End progress reporting
  if (this->EndMethod)
  {
    (*this->EndMethod)(this->EndMethodArg);
  } 
}

//----------------------------------------------------------------------------
void vtkImageEditor::Undo()
{
  // Only undo if you can
  if (this->Undoable == 0) 
  {
    return;
  }
  
  switch (this->UndoDimension)
  {
  case EDITOR_DIM_MULTI:
  case EDITOR_DIM_3D:
    // If the effect was applied to the entire volume, then just swap the
    // volume pointers so the output is the previous volume.

    this->SwapOutputs();
    break;

  case EDITOR_DIM_SINGLE:
    // If the effect was applied to one slice, then take the pixels
    // that were stored in Region before applying the effect,
    // and copy them into Output.  The Indices array stores where to 
    // place each pixel from Region into Output.

    // NOTE: vtkImageReplaceRegion derived from vtkImageInPlace, which is 
    // not in place unless Output's ReleaseDataFlag is on.
    this->Output->ReleaseDataFlagOn();

    vtkImageReplaceRegion *replace  = vtkImageReplaceRegion::New();
    replace->SetInput(this->Output);
    replace->SetRegion(this->Region);
    replace->SetIndices(this->Indices);
    replace->Update();

    // Reset Output pointer
    this->SetOutput(replace->GetOutput());

    // Detach Output from source
    replace->SetRegion(NULL);
    replace->SetOutput(NULL);
    replace->Delete();
    break;
  
  }
  
  this->Undoable = 0;
}

