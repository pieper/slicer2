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

  this->RunTime = 0.0;
  this->TotalTime = 0.0;
  this->Clip = 0;  
  this->UseInput = 1;
  this->Slice = 0;
  this->UndoDimension = this->Dimension = EDITOR_DIM_3D;
  this->Undoable = 0;

  for (i=0; i<3; i++)
  {
    this->ClipExtent[i*2] = -VTK_LARGE_INTEGER;
    this->ClipExtent[i*2+1] = VTK_LARGE_INTEGER;
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
  
  // Progress callback
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
  
  tStartTotal = clock();
  
  switch(this->Dimension)
  {
  // Single
  case EDITOR_DIM_SINGLE:

    // We will process a slice from the input,
    // and copy the processed slice into the Output.

    // Delete the UndoOutput.
    this->SetUndoOutput(NULL);

    // If the output does not exist, or has the wrong extent,
    // then create it
    if (this->Output == NULL)
    {
      newOutput = 1;
    }
    else
    {
      // Make sure output has same extent as the ORIGINAL input
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
      copy->SetOutput(NULL);
      copy->Delete();
    }
    
    // Get the input
    if (this->UseInput)
    {
      input = this->GetInput();
    }
    else 
    {
      input = this->Output;
    }

    // For undo, need to store the region from the output, not input
    reformat = vtkImageReformatIJK::New();
    if (this->UseInput)
    {
      reformat->SetInput(this->Output);
      reformat->SetInputOrderString(this->InputSliceOrder);
      reformat->SetOutputOrderString(this->OutputSliceOrder);
      reformat->ComputeTransform();
      reformat->ComputeOutputExtent();
      reformat->SetSlice(this->Slice);
      reformat->Update();

      this->SetRegion(reformat->GetOutput());
      this->SetIndices(reformat->GetIndices());
      reformat->SetOutput(NULL);
      reformat->Delete();

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

    this->FirstFilter->SetInput(reformat->GetOutput());
    tStart = clock();
    this->LastFilter->Update();
    this->RunTime = (float)(clock() - tStart) / CLOCKS_PER_SEC;

    // In-place filters are not in place if the in/out extents differ,
    // or the input's ReleaseData flag is off
    replace  = vtkImageReplaceRegion::New();
    replace->SetInput(this->Output);
    replace->SetOutput(this->Output);
    replace->SetIndices(reformat->GetIndices());
    replace->SetRegion(this->LastFilter->GetOutput());
    replace->Modified();
    replace->Update();

    // Detach Output from source
    replace->SetOutput(NULL);
    replace->SetRegion(NULL);
    replace->Delete();
    reformat->SetOutput(NULL);
    reformat->Delete();

    this->Undoable = 1;
    this->UndoDimension = this->Dimension;
    break;
 
  // Multi
  case EDITOR_DIM_MULTI:

    // One slice at a time, we will process a slice from the input,
    // and copy the processes slice into the UndoOutput.

    // If the output does not exist, or has the wrong extent,
    // then create it
    if (this->Output == NULL)
    {
      newOutput = 1;
    }
    else
    {
      // Make sure output has same extent as the ORIGINAL input
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
      copy->SetOutput(NULL);
      copy->Delete();
    }
    
    // If the UndoOutput does not exist, create it with all 0's.
    // If it exists, but has the wrong extent, then UnRegister it,
    // and create a new one with all 0's.
    newOutput = 0;
    if (this->UndoOutput == NULL)
    {
      newOutput = 1;
    }
    else
    {
      // Make sure undooutput has same extent as input
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
      // NOTE:
      // copy->Delete() performs copy->SetInput(NULL) 
      // but not                 copy->SetOutput(NULL)
      copy->SetOutput(NULL);
      copy->Delete();
    }

    // Get the input
    if (this->UseInput)
    {
      input = this->GetInput();
    }
    else 
    {
      input = this->Output;
    }

    // Now safe to begin
    reformat = vtkImageReformatIJK::New();
    reformat->SetInput(input);
    reformat->SetInputOrderString(this->InputSliceOrder);
    reformat->SetOutputOrderString(this->OutputSliceOrder);
    reformat->ComputeTransform();
    reformat->ComputeOutputExtent();
    sMax = reformat->GetNumSlices(); 

    replace  = vtkImageReplaceRegion::New();
    replace->SetInput(this->UndoOutput);
    replace->SetOutput(this->UndoOutput);
    replace->SetIndices(reformat->GetIndices());
    
    // Filter each slice one at a time
    this->RunTime = 0.0;
    for (s=0; s < sMax; s++)
    {
      reformat->SetSlice(s);
      reformat->Update();

      this->FirstFilter->SetInput(reformat->GetOutput());
      tStart = clock();
      this->LastFilter->Update();
      this->RunTime += (float)(clock() - tStart);

      replace->SetRegion(this->LastFilter->GetOutput());
      replace->Modified();
      replace->Update();
    
      if (sMax > 1)
      { 
        this->UpdateProgress((float)s/(float)(sMax-1));
      }
    }
    this->RunTime /= (float)CLOCKS_PER_SEC;

    // Detach Output from source
    replace->SetOutput(NULL);
    replace->SetRegion(NULL);
    replace->Delete();
    reformat->SetOutput(NULL);
    reformat->Delete();
      
    this->SwapOutputs();

    if (this->UndoOutput != NULL)
    {
      this->Undoable = 1;
      this->UndoDimension = this->Dimension;
    }
   break;

  // 3D
  case EDITOR_DIM_3D:

    if (this->UseInput == 0)
    {
    if (this->Output == NULL)
    {
      newOutput = 1;
    }
    else
    {
      // Make sure output has same extent as the ORIGINAL input
      this->GetInput()->GetWholeExtent(inExt);
      this->Output->GetWholeExtent(outExt);
      if ((inExt[0] != outExt[0])||(inExt[1] != outExt[1])||
          (inExt[2] != outExt[2])||(inExt[3] != outExt[3])||
          (inExt[4] != outExt[4])||(inExt[5] != outExt[5]))
      {
        newOutput = 1;
      }
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
      copy->SetOutput(NULL);
      copy->Delete();
    }

    // Get the input
    if (this->UseInput)
    {
      input = this->GetInput();
    }
    else 
    {
      input = this->Output;
    }

    // 1.) Output->filter->UndoOutput
    // 2.) Swap Output with UndoOutput
    
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

    this->SwapOutputs();
  
    if (this->UndoOutput != NULL)
    {
      this->Undoable = 1;
      this->UndoDimension = this->Dimension;
    }
    break;
  }

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
  if (this->Undoable == 0) 
  {
    return;
  }
  
  switch (this->UndoDimension)
  {
  case EDITOR_DIM_MULTI:
  case EDITOR_DIM_3D:

    this->SwapOutputs();
    break;

  case EDITOR_DIM_SINGLE:

    // Connect output to source
    vtkImageReplaceRegion *replace  = vtkImageReplaceRegion::New();
    replace->SetInput(this->Output);
    replace->SetOutput(this->Output);

    // Execute
    replace->SetRegion(this->Region);
    replace->SetIndices(this->Indices);
    replace->Update();

    // Detach Output from source
    replace->SetRegion(NULL);
    replace->SetOutput(NULL);
    replace->Delete();
    break;
  
  }
  
  this->Undoable = 0;
}

