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
#include <stdio.h>
#include <string.h>
#include "vtkMrmlSlicer.h"
#include "vtkObjectFactory.h"
#include "vtkImageCanvasSource2D.h"

//------------------------------------------------------------------------------
vtkMrmlSlicer* vtkMrmlSlicer::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlSlicer");
  if(ret)
  {
    return (vtkMrmlSlicer*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlSlicer;
}


static void Normalize(double *a)
{
  double d;
  d = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

  if (d == 0.0) return;

  a[0] = a[0] / d;
  a[1] = a[1] / d;
  a[2] = a[2] / d;
}

// a = b x c
static void Cross(double *a, double *b, double *c)
{
  a[0] = b[1]*c[2] - c[1]*b[2];
  a[1] = c[0]*b[2] - b[0]*c[2];
  a[2] = b[0]*c[1] - c[0]*b[1];
}

//----------------------------------------------------------------------------
vtkMrmlSlicer::vtkMrmlSlicer()
{
  this->ZoomCenter0[0] = this->ZoomCenter0[1] = 0.0;
  this->ZoomCenter1[0] = this->ZoomCenter1[1] = 0.0;
  this->ZoomCenter2[0] = this->ZoomCenter2[1] = 0.0;
  this->FieldOfView = 240.0;
  this->LabelIndirectLUT = NULL;
  this->PolyDraw = vtkImageDrawROI::New();
  this->ReformatIJK = vtkImageReformatIJK::New();
  this->DrawIjkPoints = vtkPoints::New();

  // NoneVolume's MrmlNode
  this->NoneNode = vtkMrmlVolumeNode::New();
  this->NoneNode->Register(this);
  this->NoneNode->Delete();
  this->NoneNode->SetID(0);
  this->NoneNode->SetDescription("NoneVolume created by vtkMrmlSlicer");
  this->NoneNode->SetName("None");

	// Create a NoneVolume
  this->NoneVolume = vtkMrmlDataVolume::New();
  this->NoneVolume->Register(this);
  this->NoneVolume->Delete();
  this->NoneVolume->SetMrmlNode(this->NoneNode);

  this->ComputeOffsetRange();

  for (int s=0; s<NUM_SLICES; s++)
  {
    this->ReformatMatrix[s] = vtkMatrix4x4::New();

    // Lower Pipeline

    // Volumes
    this->BackVolume[s]  = NULL;
    this->SetBackVolume(s, this->NoneVolume);
	  this->ForeVolume[s]  = NULL;
    this->SetForeVolume(s, this->NoneVolume);
	  this->LabelVolume[s] = NULL;
    this->SetLabelVolume(s, this->NoneVolume);

    // Reformatters
    this->BackReformat[s]  = vtkImageReformat::New();
    this->BackReformat[s]->SetReformatMatrix(this->ReformatMatrix[s]);

    this->ForeReformat[s]  = vtkImageReformat::New();
    this->ForeReformat[s]->SetReformatMatrix(this->ReformatMatrix[s]);

    this->LabelReformat[s] = vtkImageReformat::New();
    this->LabelReformat[s]->SetReformatMatrix(this->ReformatMatrix[s]);
	  this->LabelReformat[s]->InterpolateOff();

    // Mappers
    this->BackMapper[s]  = vtkImageMapToColors::New();
    this->BackMapper[s]->SetOutputFormatToRGBA();
    this->ForeMapper[s]  = vtkImageMapToColors::New();
    this->ForeMapper[s]->SetOutputFormatToRGBA();
    this->LabelMapper[s] = vtkImageMapToColors::New();
    this->LabelMapper[s]->SetOutputFormatToRGBA();

    // Label outline
    this->LabelOutline[s] = vtkImageLabelOutline::New();

    // Overlays
    this->ForeOpacity = 0.5;
    // Initialize Overlay to have 3 inputs (even though these are not
    // the actual inputs).
    // Once the inputs are created, I can then set the fore opacity.
    this->Overlay[s] = vtkImageOverlay::New();
    this->Overlay[s]->SetInput(0, this->NoneVolume->GetOutput());
    this->Overlay[s]->SetInput(1, this->NoneVolume->GetOutput());
    this->Overlay[s]->SetInput(2, this->NoneVolume->GetOutput());
    this->Overlay[s]->SetOpacity(1, this->ForeOpacity);

    // Upper Pipeline
    
    // Double
	  this->Double[s] = vtkImageDouble2D::New();
    this->DoubleSliceSize[s] = 0;

    // Zoom
	  this->Zoom[s] = vtkImageZoom2D::New();

    // Cursor
    this->Cursor[s] = vtkImageCrossHair2D::New();
	  // DAVE need a SetAnnoColor
	  this->Cursor[s]->SetCursorColor(1.0, 1.0, 0.5);
	  this->Cursor[s]->SetCursor(127, 127);
	  this->Cursor[s]->SetHashGap(10);
	  this->Cursor[s]->SetHashLength(6);

	  // Offset and Orient

	  this->ComputeOffsetRangeIJK(s);
    for (int j=0; j<MRML_SLICER_NUM_ORIENT; j++)
    {
      this->InitOffset(s, this->GetOrientString(j), 0.0);
    }
	  this->Driver[s] = 0;
    this->SetOrient(s, MRML_SLICER_ORIENT_ORIGSLICE);

    // Filter
    this->FirstFilter[s] = NULL;
    this->LastFilter[s] = NULL;
  }

  this->BackFilter = 0;
  this->ForeFilter = 0;
  this->FilterActive = 0;
  this->FilterOverlay = 0;

  // Matrix
  this->DirN[0] = 0;
  this->DirN[1] = 0;
  this->DirN[2] = -1;
  this->DirT[0] = 1;
  this->DirT[1] = 0;
  this->DirT[2] = 0;
  this->DirP[0] = 0;
  this->DirP[1] = 0;
  this->DirP[2] = 0;
  this->CamN[0] = 0;
  this->CamN[1] = 0;
  this->CamN[2] = -1;
  this->CamT[0] = 1;
  this->CamT[1] = 0;
  this->CamT[2] = 0;
  this->CamP[0] = 0;
  this->CamP[1] = 0;
  this->CamP[2] = 0;

  // Draw
  this->DrawX = this->DrawY = 0;

  // Point
  this->WldPoint[0] = 0;
  this->WldPoint[1] = 0;
  this->WldPoint[2] = 0;
  this->IjkPoint[0] = 0;
  this->IjkPoint[1] = 0;
  this->IjkPoint[2] = 0;
  this->Seed[0] = 0;
  this->Seed[1] = 0;
  this->Seed[2] = 0;
  this->Seed2D[0] = 0;
  this->Seed2D[1] = 0;
  this->Seed2D[2] = 0;
  this->ReformatPoint[0] = 0;
  this->ReformatPoint[1] = 0;

  this->BuildLowerTime.Modified();
  this->BuildUpperTime.Modified();

  // >> AT 3/26/01
  // Use the original approach by default.
  this->DrawDoubleApproach = 0;
  // << AT 3/26/01

  // reformatting additions
  this->VolumesToReformat = vtkCollection::New();
  this->VolumeReformatters = vtkVoidArray::New();
  this->MaxNumberOfVolumesToReformat = 20;
  this->VolumeReformatters->SetNumberOfValues(this->MaxNumberOfVolumesToReformat);
  for (int i = 0; i < this->MaxNumberOfVolumesToReformat; i++)
    {
      this->VolumeReformatters->SetValue(i,NULL);
    }

  // Active slice has polygon drawing (return with GetActiveOutput)
  this->SetActiveSlice(0);

  this->Update();
}

//----------------------------------------------------------------------------
vtkMrmlSlicer::~vtkMrmlSlicer()
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->BackReformat[s]->Delete();
	  this->ForeReformat[s]->Delete();
	  this->LabelReformat[s]->Delete();
    this->Overlay[s]->Delete();
    this->BackMapper[s]->Delete();
    this->ForeMapper[s]->Delete();
    this->LabelMapper[s]->Delete();
	  this->ReformatMatrix[s]->Delete();
	  this->LabelOutline[s]->Delete();
	  this->Cursor[s]->Delete();
	  this->Zoom[s]->Delete();
	  this->Double[s]->Delete();

    // Unregister objects others allocated

    if (this->BackVolume[s] != NULL) 
    {
      this->BackVolume[s]->UnRegister(this);
    }
    if (this->ForeVolume[s] != NULL) 
    {
      this->ForeVolume[s]->UnRegister(this);
    }
    if (this->LabelVolume[s] != NULL) 
    {
      this->LabelVolume[s]->UnRegister(this);
    }

    if (this->FirstFilter[s] != NULL) 
    {
      this->FirstFilter[s]->UnRegister(this);
    }
    if (this->LastFilter[s] != NULL) 
    {
      this->LastFilter[s]->UnRegister(this);
    }
  }
  this->PolyDraw->Delete();
  this->DrawIjkPoints->Delete();
  this->ReformatIJK->Delete();

  // Unregister objects others allocated
  if (this->LabelIndirectLUT)
  {
    this->LabelIndirectLUT->UnRegister(this);
  }

  // Signal that we're no longer using it
  if (this->NoneVolume != NULL) 
  {
    this->NoneVolume->UnRegister(this);
  }
  if (this->NoneNode != NULL) 
  {
    this->NoneNode->UnRegister(this);
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSlicer::PrintSelf(ostream& os, vtkIndent indent)
{
  int s;
  
  vtkObject::PrintSelf(os, indent);

  os << indent << "FOV:             " << this->FieldOfView << "\n";
  os << indent << "BuildLower Time: " <<this->BuildLowerTime.GetMTime()<<"\n";
  os << indent << "BuildUpper Time: " <<this->BuildUpperTime.GetMTime()<<"\n";
  os << indent << "Update Time:     " <<this->UpdateTime.GetMTime() << "\n";
  os << indent << "Active Slice:    " <<this->ActiveSlice << "\n";
  os << indent << "ForeOpacity:     " <<this->ForeOpacity << "\n";

  // vtkSetObjectMacro
  os << indent << "NoneVolume: " << this->NoneVolume << "\n";
  if (this->NoneVolume)
  {
    this->NoneVolume->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "NoneNode: " << this->NoneNode << "\n";
  if (this->NoneNode)
  {
    this->NoneNode->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "LabelIndirectLUT: " << this->LabelIndirectLUT << "\n";
  if (this->LabelIndirectLUT)
  {
    this->LabelIndirectLUT->PrintSelf(os,indent.GetNextIndent());
  }

  for (s=0; s<NUM_SLICES; s++)
  {
    os << indent << "BackVolume: " << s << " " << this->BackVolume[s] << "\n";
    if (this->BackVolume[s])
    {
      this->BackVolume[s]->PrintSelf(os,indent.GetNextIndent());
    }
    os << indent << "ForeVolume: " << s << " " << this->ForeVolume[s] << "\n";
    if (this->ForeVolume[s])
    {
      this->ForeVolume[s]->PrintSelf(os,indent.GetNextIndent());
    }
    os << indent << "LabelVolume: " << s << " " << this->LabelVolume[s] << "\n";
    if (this->LabelVolume[s])
    {
      this->LabelVolume[s]->PrintSelf(os,indent.GetNextIndent());
    }
    os << indent << "FirstFilter: " << s << " " << this->FirstFilter[s] << "\n";
    if (this->FirstFilter[s])
    {
      this->FirstFilter[s]->PrintSelf(os,indent.GetNextIndent());
    }
    os << indent << "LastFilter:  " << s << " " << this->LastFilter[s] << "\n";
    if (this->LastFilter[s])
    {
      this->LastFilter[s]->PrintSelf(os,indent.GetNextIndent());
    }
    os << indent << "DoubleSliceSize: " <<this->DoubleSliceSize[s] << "\n";
  }
}

//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetNoneVolume(vtkMrmlDataVolume *vol)
{
  int s;
  
  // Only act if this is a different vtkMrmlDataVolume
  if (this->NoneVolume != vol) 
  {
    for (s=0; s<NUM_SLICES; s++)
    {
      if (this->ForeVolume[s] == this->NoneVolume ||
        this->ForeVolume[s] == NULL)
      {
        this->SetForeVolume(s, vol);
      }
      if (this->BackVolume[s] == this->NoneVolume ||
        this->BackVolume[s] == NULL)
      {
        this->SetBackVolume(s, vol);
      }
      if (this->LabelVolume[s] == this->NoneVolume ||
        this->LabelVolume[s] == NULL)
      {
        this->SetLabelVolume(s, vol);
      }
    }

    if (this->NoneVolume != NULL) 
    {
      this->NoneVolume->UnRegister(this);
    }
    this->NoneVolume = vol;

    if (this->NoneVolume != NULL) 
    {
      this->NoneVolume->Register(this);
    }

    // Node
    if (this->NoneNode != NULL) 
    {
      this->NoneNode->UnRegister(this);
    }

    if (vol != NULL) 
    {
      this->NoneNode = (vtkMrmlVolumeNode *) vol->GetMrmlNode();
    }
    else
    {
      this->NoneNode = NULL;
    }

    if (this->NoneNode != NULL) 
    {
      this->NoneNode->Register(this);
    }

    // Refresh pointers

    this->Modified();
    this->BuildUpperTime.Modified();
  }
}

//----------------------------------------------------------------------------
// IsOrientIJK
//----------------------------------------------------------------------------
int vtkMrmlSlicer::IsOrientIJK(int s)
{
  if (
    this->Orient[s] == MRML_SLICER_ORIENT_ORIGSLICE ||
	  this->Orient[s] == MRML_SLICER_ORIENT_AXISLICE ||
	  this->Orient[s] == MRML_SLICER_ORIENT_CORSLICE ||
	  this->Orient[s] == MRML_SLICER_ORIENT_SAGSLICE) 
  {
    return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------
// GetIJKVolume
//----------------------------------------------------------------------------
vtkMrmlDataVolume* vtkMrmlSlicer::GetIJKVolume(int s)
{
  if (this->BackVolume[s] != this->NoneVolume)
  {
    return this->BackVolume[s];
  }
  if (this->ForeVolume[s] != this->NoneVolume)
  {
    return this->ForeVolume[s];
  }
  if (this->LabelVolume[s] != this->NoneVolume)
  {
    return this->LabelVolume[s];
  }
  return this->NoneVolume;
}

vtkImageReformat* vtkMrmlSlicer::GetIJKReformat(int s)
{
  if (this->BackVolume[s] != this->NoneVolume)
  {
    return this->BackReformat[s];
  }
  if (this->ForeVolume[s] != this->NoneVolume)
  {
    return this->ForeReformat[s];
  }
  if (this->LabelVolume[s] != this->NoneVolume)
  {
    return this->LabelReformat[s];
  }
  return this->BackReformat[2];
}

////////////////////////////////////////////////////////////////////////////////
//                              PIPELINE
////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// Update
//----------------------------------------------------------------------------
void vtkMrmlSlicer::Update()
{
  int s;
  
  // Do we need to rebuild the pipeline?
  if (this->BuildUpperTime > this->UpdateTime)
  {
    for (s=0; s<NUM_SLICES; s++)
    {
      this->BuildUpper(s);
    }
  }
  if (this->BuildLowerTime > this->UpdateTime)
  {
    for (s=0; s<NUM_SLICES; s++)
    {
      this->BuildLower(s);
    }
  }

  this->UpdateTime.Modified();
}

//----------------------------------------------------------------------------
// Active Slice
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetActiveSlice(int s)
{
  // no change
  if (this->ActiveSlice == s)
  {
	  return;
  }
  this->ActiveSlice = s;
  this->BuildUpperTime.Modified();
  this->BuildLowerTime.Modified();
  
  // arbitrary reformatting
  this->VolumeReformattersModified();
}

//----------------------------------------------------------------------------
// Background Volume
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetBackVolume(vtkMrmlDataVolume *vol)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->SetBackVolume(s, vol);
  }
}
void vtkMrmlSlicer::SetBackVolume(int s, vtkMrmlDataVolume *vol)
{
  if (this->BackVolume[s] != vol) 
  {
    if (this->BackVolume[s] != NULL) 
    { 
      this->BackVolume[s]->UnRegister(this); 
    }
    this->BackVolume[s] = vol;
    if (this->BackVolume[s] != NULL) 
    { 
      this->BackVolume[s]->Register(this); 
    }

    this->Modified(); 
    this->BuildUpperTime.Modified();
  } 
}

//----------------------------------------------------------------------------
// Foreground Volume
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetForeVolume(vtkMrmlDataVolume *vol)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->SetForeVolume(s, vol);
  }
}
void vtkMrmlSlicer::SetForeVolume(int s, vtkMrmlDataVolume *vol)
{
  if (this->ForeVolume[s] != vol) 
  {
    if (this->ForeVolume[s] != NULL) 
    { 
      this->ForeVolume[s]->UnRegister(this); 
    }
    this->ForeVolume[s] = vol;
    if (this->ForeVolume[s] != NULL) 
    { 
      this->ForeVolume[s]->Register(this); 
    }
    this->Modified(); 
    this->BuildUpperTime.Modified();
  } 
}

//----------------------------------------------------------------------------
// Label Volume
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetLabelVolume(vtkMrmlDataVolume *vol)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->SetLabelVolume(s, vol);
  }
}
void vtkMrmlSlicer::SetLabelVolume(int s, vtkMrmlDataVolume *vol)
{
  if (this->LabelVolume[s] != vol) 
  {
    if (this->LabelVolume[s] != NULL) 
    { 
      this->LabelVolume[s]->UnRegister(this); 
    }
    this->LabelVolume[s] = vol;
    if (this->LabelVolume[s] != NULL) 
    { 
      this->LabelVolume[s]->Register(this); 
    }
    this->Modified(); 
    this->BuildUpperTime.Modified();
  } 
}

//----------------------------------------------------------------------------
// Filter 
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetFirstFilter(int s, vtkImageToImageFilter *filter)
{
  if (this->FirstFilter[s] != filter) 
  {
    if (this->FirstFilter[s] != NULL) 
    { 
      this->FirstFilter[s]->UnRegister(this); 
    }
    this->FirstFilter[s] = filter;
    if (this->FirstFilter[s] != NULL) 
    { 
      this->FirstFilter[s]->Register(this); 
    }
    this->Modified(); 
    this->BuildUpperTime.Modified();
  } 
}
void vtkMrmlSlicer::SetLastFilter(int s, vtkImageSource *filter)
{
  if (this->LastFilter[s] != filter) 
  {
    if (this->LastFilter[s] != NULL) 
    { 
      this->LastFilter[s]->UnRegister(this); 
    }
    this->LastFilter[s] = filter;
    if (this->LastFilter[s] != NULL) 
    { 
      this->LastFilter[s]->Register(this); 
    }
    this->Modified(); 
    this->BuildUpperTime.Modified();
  } 
}

//----------------------------------------------------------------------------
// BuildUpper
//----------------------------------------------------------------------------
void vtkMrmlSlicer::BuildUpper(int s)
{
  vtkMrmlDataVolume *v;
  int filter = 0;

  // Error checking
  if (this->NoneVolume == NULL)
  {
    vtkErrorMacro(<<"BuildUpper: NULL NoneVolume");
    return;
  }

  // if we are displaying filter output over a slice, AND
  // either this slice is the only one we are filtering
  // OR we are filtering all slices, then make sure the filter is set.
  if ((this->BackFilter || this->ForeFilter) && 
     ((this->FilterActive && s == this->ActiveSlice) || !this->FilterActive))
  {
    filter = 1;
    if ( this->FirstFilter[s] == NULL )
    {
      vtkErrorMacro(<<"Apply: FirstFilter not set: " << s);
      return;
    }
    if ( this->LastFilter[s] == NULL )
    {
      vtkErrorMacro(<<"Apply: LastFilter not set: " << s);
      return;
    }
  }

  // Back Layer
  /////////////////////////////////////////////////////
  
  v = this->BackVolume[s];
  vtkMrmlVolumeNode *node = (vtkMrmlVolumeNode*) v->GetMrmlNode();

  // Reformatter
  this->BackReformat[s]->SetInput(v->GetOutput());
  this->BackReformat[s]->SetInterpolate(node->GetInterpolate());
  this->BackReformat[s]->SetWldToIjkMatrix(node->GetWldToIjk());

  // If data has more than one scalar component, then don't use the mapper,
  if (v->GetOutput()->GetNumberOfScalarComponents() > 1)
  {
    // Overlay
    this->Overlay[s]->SetInput(0, this->BackReformat[s]->GetOutput());
  }
  else 
  {
    // Mapper
    this->BackMapper[s]->SetInput(this->BackReformat[s]->GetOutput());
    this->BackMapper[s]->SetLookupTable(v->GetIndirectLUT());
    // Overlay
    this->Overlay[s]->SetInput(0, this->BackMapper[s]->GetOutput());
  }  


  // Fore Layer
  /////////////////////////////////////////////////////
  
  v = this->ForeVolume[s];

  // If the None volume, then turn the Fore input off
  if (v == this->NoneVolume) 
  {
	  this->Overlay[s]->SetInput(1, NULL);
  } 
  else 
  {
    // Reformatter
    this->ForeReformat[s]->SetInput(v->GetOutput());
    this->ForeReformat[s]->SetInterpolate(node->GetInterpolate());
    this->ForeReformat[s]->SetWldToIjkMatrix(node->GetWldToIjk());

    // If data has more than one scalar component, then don't use the mapper,
    if (v->GetOutput()->GetNumberOfScalarComponents() > 1)
    {
      // Overlay
      this->Overlay[s]->SetInput(0, this->ForeReformat[s]->GetOutput());
    }
    else 
    {
      // Active filter?
      if (filter)
      {
	// Filter
        if (this->ForeFilter)
        {
          this->FirstFilter[s]->SetInput(this->ForeReformat[s]->GetOutput());
        }
        else
        {
          this->FirstFilter[s]->SetInput(this->BackReformat[s]->GetOutput());
        }
	// Mapper
	if (this->FilterOverlay)
	  {
	    // If filter is being overlayed in label layer,
	    // don't display the filter's output in the fore layer.
	    this->ForeMapper[s]->SetInput(this->ForeReformat[s]->GetOutput());
	  }
	else
	  {
	    // default display: just replace fore layer with filter output
	    this->ForeMapper[s]->SetInput(this->LastFilter[s]->GetOutput());
	  }
      } // end if filter
      else 
      {
        // Mapper
        this->ForeMapper[s]->SetInput(this->ForeReformat[s]->GetOutput());
      }
      // Mapper
      this->ForeMapper[s]->SetLookupTable(v->GetIndirectLUT());
      // Overlay
      this->Overlay[s]->SetInput(1, this->ForeMapper[s]->GetOutput());
    } 
  }


  // Label Layer
  /////////////////////////////////////////////////////
    
  v = this->LabelVolume[s];

  // If the None volume, then turn the Label input off
  if (v == this->NoneVolume) 
  {
    this->Overlay[s]->SetInput(2, NULL);
  }
  else
  {
    // If ForeVolume == LabelVolume, then save reformatting work
    if (v == this->ForeVolume[s])
    {
      if (filter)
      {
        // Outline
	this->LabelOutline[s]->SetInput(this->LastFilter[s]->GetOutput());
      }
      else 
      {
        // Outline
        this->LabelOutline[s]->SetInput(this->ForeReformat[s]->GetOutput());
      }
      // Overlay
      this->LabelMapper[s]->SetInput(this->LabelOutline[s]->GetOutput());
      this->Overlay[s]->SetInput(2, this->LabelMapper[s]->GetOutput());
    }
    else
    {
      // Reformatter
      this->LabelReformat[s]->SetInput(v->GetOutput());
      this->LabelReformat[s]->InterpolateOff(); // never interpolate label
      this->LabelReformat[s]->SetWldToIjkMatrix(node->GetWldToIjk());

      // Outline
      this->LabelOutline[s]->SetInput(this->LabelReformat[s]->GetOutput());

      // Overlay
      this->LabelMapper[s]->SetInput(this->LabelOutline[s]->GetOutput());
      this->Overlay[s]->SetInput(2, this->LabelMapper[s]->GetOutput());
    }
  }

  // The IJK reformatting depends on the volumes
  /////////////////////////////////////////////////////

  // Reset the offset range.
  // If the range changed, then reset the offset to be
  // in the center of this new range.
  this->ComputeOffsetRangeIJK(s);
    
  // IJK Orientations depends on volumes
  if (this->IsOrientIJK(s))
  {
    this->ComputeReformatMatrix(s);
  }

}

//----------------------------------------------------------------------------
// BuildLower
//----------------------------------------------------------------------------
void vtkMrmlSlicer::BuildLower(int s)
{
  int mode;

  // InActive Slices:
  //
  // 1.) Overlay --> Cursor
  // 2.) Overlay --> Zoom   --> Cursor
  // 3.) Overlay --> Double --> Cursor
  // 4.) Overlay --> Zoom   --> Double --> Cursor
  //
  // Active Slice:
  //
  // 1.) Overlay --> PolyDraw --> Cursor
  // 2.) Overlay --> PolyDraw --> Zoom   --> Cursor
  // 3.) Overlay --> PolyDraw --> Double --> Cursor
  // 4.) Overlay --> PolyDraw --> Zoom   --> Double --> Cursor
  	
  float ctr[2];
  this->Zoom[s]->GetCenter(ctr);
  if (this->Zoom[s]->GetMagnification() != 1.0 || 
    this->Zoom[s]->GetAutoCenter() == 0 ||
    (ctr[0] == 0.0 && ctr[1] == 0.0))
  {
    mode = (this->DoubleSliceSize[s] == 1) ? 4 : 2;
  } 
  else 
  {
    mode = (this->DoubleSliceSize[s] == 1) ? 3 : 1;
  }

  if (this->ActiveSlice == s)
  {
    switch (mode)
	  {
    case 1:
      this->PolyDraw->SetInput(this->Overlay[s]->GetOutput());
      this->Cursor[s]->SetInput(this->PolyDraw->GetOutput());
	    break;
    case 2:
      this->PolyDraw->SetInput(this->Overlay[s]->GetOutput());
      this->Zoom[s]->SetInput(this->PolyDraw->GetOutput());
      this->Cursor[s]->SetInput(this->Zoom[s]->GetOutput());
      break;
    case 3:
      this->PolyDraw->SetInput(this->Overlay[s]->GetOutput());
  	  this->Double[s]->SetInput(this->PolyDraw->GetOutput());
      this->Cursor[s]->SetInput(this->Double[s]->GetOutput());
  	  break;
    case 4:
      this->PolyDraw->SetInput(this->Overlay[s]->GetOutput());
  	  this->Zoom[s]->SetInput(this->PolyDraw->GetOutput());
      this->Double[s]->SetInput(this->Zoom[s]->GetOutput());
      this->Cursor[s]->SetInput(this->Double[s]->GetOutput());
      break;
	  }
  }
  else
  {
    switch (mode)
	  {
    case 1:
      this->Cursor[s]->SetInput(this->Overlay[s]->GetOutput());
	    break;
    case 2:
      this->Zoom[s]->SetInput(this->Overlay[s]->GetOutput());
      this->Cursor[s]->SetInput(this->Zoom[s]->GetOutput());
      break;
    case 3:
  	  this->Double[s]->SetInput(this->Overlay[s]->GetOutput());
      this->Cursor[s]->SetInput(this->Double[s]->GetOutput());
  	  break;
    case 4:
  	  this->Zoom[s]->SetInput(this->Overlay[s]->GetOutput());
      this->Double[s]->SetInput(this->Zoom[s]->GetOutput());
      this->Cursor[s]->SetInput(this->Double[s]->GetOutput());
      break;
	  }
  }
}


////////////////////////////////////////////////////////////////////////////////
//                          SLICE OFFSET & ORIENT
////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Offset
//----------------------------------------------------------------------------
void vtkMrmlSlicer::ComputeOffsetRange()
{
  int s, orient;
  float fov = this->FieldOfView / 2.0;

  for (s=0; s<NUM_SLICES; s++)
  {
    for (orient = MRML_SLICER_ORIENT_AXIAL; orient <= MRML_SLICER_ORIENT_PERP;
      orient++)
    {
      this->OffsetRange[s][orient][0] = -fov;
      this->OffsetRange[s][orient][1] =  fov;
      this->Offset[s][orient] = 0;
    }
  }
}

void vtkMrmlSlicer::SetOffsetRange(int s, int orient, int min, int max, int *modified)
{
  if (this->OffsetRange[s][orient][0] != min)
  {
    this->OffsetRange[s][orient][0] = min;
    *modified = 1;
  }
  if (this->OffsetRange[s][orient][1] != max)
  {
    this->OffsetRange[s][orient][1] = max;
    *modified = 1;
  }
}

void vtkMrmlSlicer::ComputeOffsetRangeIJK(int s)
{
  int xMax, yMax, zMax, xMin, yMin, zMin, xAvg, yAvg, zAvg, *ext;
  float fov = this->FieldOfView / 2.0;
  int orient = this->GetOrient(s);
  int modified = 0;
  vtkMrmlDataVolume *vol = this->GetIJKVolume(s);
  if (vol == NULL) return;
  vtkMrmlVolumeNode *node = (vtkMrmlVolumeNode*) vol->GetMrmlNode();
  char* order = node->GetScanOrder();
  if (order == NULL) return;

  ext = vol->GetOutput()->GetWholeExtent();
  xMin = ext[0];
  yMin = ext[2];
  zMin = ext[4];
  xMax = ext[1];
  yMax = ext[3];
  zMax = ext[5];
  xAvg = (ext[1] + ext[0])/2;
  yAvg = (ext[3] + ext[2])/2;
  zAvg = (ext[5] + ext[4])/2;

  this->OffsetRange[s][MRML_SLICER_ORIENT_ORIGSLICE][0] = zMin;
  this->OffsetRange[s][MRML_SLICER_ORIENT_ORIGSLICE][1] = zMax;

	// Sagittal
  if (!strcmp(order,"LR") || !strcmp(order,"RL")) 
  {
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_AXISLICE, xMin, xMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_SAGSLICE, zMin, zMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_CORSLICE, xMin, xMax, &modified);

    if (modified)
    {
      this->Offset[s][MRML_SLICER_ORIENT_ORIGSLICE] = zAvg;
      this->Offset[s][MRML_SLICER_ORIENT_AXISLICE]  = xAvg;
      this->Offset[s][MRML_SLICER_ORIENT_SAGSLICE]  = zAvg;
      this->Offset[s][MRML_SLICER_ORIENT_CORSLICE]  = xAvg;
    }
	}
  // Coronal
	else if (!strcmp(order,"AP") || !strcmp(order,"PA")) 
  {
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_AXISLICE, xMin, xMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_SAGSLICE, xMin, xMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_CORSLICE, zMin, zMax, &modified);

    if (modified)
    {
      this->Offset[s][MRML_SLICER_ORIENT_ORIGSLICE] = zAvg;
      this->Offset[s][MRML_SLICER_ORIENT_AXISLICE]  = xAvg;
      this->Offset[s][MRML_SLICER_ORIENT_SAGSLICE]  = xAvg;
      this->Offset[s][MRML_SLICER_ORIENT_CORSLICE]  = zAvg;
    }
	}
  // Axial (and oblique)
	else 
  {
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_AXISLICE, zMin, zMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_SAGSLICE, xMin, xMax, &modified);
	  this->SetOffsetRange(s, MRML_SLICER_ORIENT_CORSLICE, xMin, xMax, &modified);

    if (modified)
    {
      this->Offset[s][MRML_SLICER_ORIENT_ORIGSLICE] = zAvg;
      this->Offset[s][MRML_SLICER_ORIENT_AXISLICE]  = zAvg;
      this->Offset[s][MRML_SLICER_ORIENT_SAGSLICE]  = xAvg;
      this->Offset[s][MRML_SLICER_ORIENT_CORSLICE]  = xAvg;
    }
	}
}

void vtkMrmlSlicer::InitOffset(int s, char *str, float offset)
{
  int orient = (int) ConvertStringToOrient(str);
  this->Offset[s][orient] = offset;
}

//----------------------------------------------------------------------------
// Orient
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetOrient(int orient)
{
  if (orient == MRML_SLICER_ORIENT_AXISAGCOR) 
  {
	  this->SetOrient(0, MRML_SLICER_ORIENT_AXIAL);
	  this->SetOrient(1, MRML_SLICER_ORIENT_SAGITTAL);
	  this->SetOrient(2, MRML_SLICER_ORIENT_CORONAL);
  } 
  else if (orient == MRML_SLICER_ORIENT_SLICES) 
  {
	  this->SetOrient(0, MRML_SLICER_ORIENT_AXISLICE);
	  this->SetOrient(1, MRML_SLICER_ORIENT_SAGSLICE);
	  this->SetOrient(2, MRML_SLICER_ORIENT_CORSLICE);
  } 
  else if (orient == MRML_SLICER_ORIENT_ORTHO) 
  {
	  this->SetOrient(0, MRML_SLICER_ORIENT_PERP);
	  this->SetOrient(1, MRML_SLICER_ORIENT_INPLANE);
	  this->SetOrient(2, MRML_SLICER_ORIENT_INPLANE90);
  }
}

void vtkMrmlSlicer::SetOrient(int s, int orient)
{
  this->Orient[s] = orient;

  this->ComputeReformatMatrix(s);
}

void vtkMrmlSlicer::SetOrientString(char *str)
{
  if (strcmp(str, "AxiSagCor") == 0)
 	  this->SetOrient(MRML_SLICER_ORIENT_AXISAGCOR);
  else if (strcmp(str, "Orthogonal") == 0)
 	  this->SetOrient(MRML_SLICER_ORIENT_ORTHO);
  else if (strcmp(str, "Slices") == 0)
 	  this->SetOrient(MRML_SLICER_ORIENT_SLICES);
  else
 	  this->SetOrient(MRML_SLICER_ORIENT_AXISAGCOR);
}

char* vtkMrmlSlicer::GetOrientString(int s)
{
  return ConvertOrientToString(this->Orient[s]);
}

void vtkMrmlSlicer::SetOrientString(int s, char *str)
{
  int orient = ConvertStringToOrient(str);
  this->SetOrient(s, orient);
}

int vtkMrmlSlicer::ConvertStringToOrient(char *str)
{
  if      (strcmp(str, "Axial") == 0)
 	  return MRML_SLICER_ORIENT_AXIAL;
  else if (strcmp(str, "Sagittal") == 0)
 	  return MRML_SLICER_ORIENT_SAGITTAL;
  else if (strcmp(str, "Coronal") == 0)
 	  return MRML_SLICER_ORIENT_CORONAL;
  else if (strcmp(str, "InPlane") == 0)
 	  return MRML_SLICER_ORIENT_INPLANE;
  else if (strcmp(str, "InPlane90") == 0)
 	  return MRML_SLICER_ORIENT_INPLANE90;
  else if (strcmp(str, "InPlaneNeg90") == 0)
 	  return MRML_SLICER_ORIENT_INPLANENEG90;
  else if (strcmp(str, "Perp") == 0)
 	  return MRML_SLICER_ORIENT_PERP;
  else if (strcmp(str, "OrigSlice") == 0)
 	  return MRML_SLICER_ORIENT_ORIGSLICE;
  else if (strcmp(str, "AxiSlice") == 0)
 	  return MRML_SLICER_ORIENT_AXISLICE;
  else if (strcmp(str, "CorSlice") == 0)
 	  return MRML_SLICER_ORIENT_CORSLICE;
  else if (strcmp(str, "SagSlice") == 0)
 	  return MRML_SLICER_ORIENT_SAGSLICE;
  else
 	  return MRML_SLICER_ORIENT_AXIAL;
}

char* vtkMrmlSlicer::ConvertOrientToString(int orient)
{
  switch (orient) 
  {
    case MRML_SLICER_ORIENT_AXIAL:
      return "Axial";
    case MRML_SLICER_ORIENT_SAGITTAL:
      return "Sagittal";
    case MRML_SLICER_ORIENT_CORONAL:
      return "Coronal";
    case MRML_SLICER_ORIENT_INPLANE:
      return "InPlane";
    case MRML_SLICER_ORIENT_INPLANE90:
      return "InPlane90";
    case MRML_SLICER_ORIENT_INPLANENEG90:
      return "InPlaneNeg90";
    case MRML_SLICER_ORIENT_PERP:
      return "Perp";
    case MRML_SLICER_ORIENT_ORIGSLICE:
      return "OrigSlice";
    case MRML_SLICER_ORIENT_AXISLICE:
      return "AxiSlice";
    case MRML_SLICER_ORIENT_CORSLICE:
      return "CorSlice";
    case MRML_SLICER_ORIENT_SAGSLICE:
      return "SagSlice";
	default:
	  return "Axial";
  }	  
}


////////////////////////////////////////////////////////////////////////////////
//                    REFORMAT
////////////////////////////////////////////////////////////////////////////////


void vtkMrmlSlicer::ComputeReformatMatrixIJK(int s, 
	float offset, vtkMatrix4x4 *ref)
{
  char orderString[3];
  vtkMrmlDataVolume *vol = this->GetIJKVolume(s);
  vtkMrmlVolumeNode *node = (vtkMrmlVolumeNode*) vol->GetMrmlNode();


  if (this->IsOrientIJK(s) == 0)
  {
	  vtkErrorMacro(<<"ComputeReformatMatrixIJK: orient is "<<this->Orient[s]);
    return;
  }

  switch (this->Orient[s])
  {
    case MRML_SLICER_ORIENT_ORIGSLICE:
      sprintf(orderString, "%s", node->GetScanOrder());
      break;
    case MRML_SLICER_ORIENT_AXISLICE:
      sprintf(orderString, "IS");
      break;
    case MRML_SLICER_ORIENT_SAGSLICE:
      sprintf(orderString, "LR");
      break;
    case MRML_SLICER_ORIENT_CORSLICE:
      sprintf(orderString, "PA");
      break;
  }//switch

  vtkImageReformatIJK *ijk = this->ReformatIJK;
  ijk->SetWldToIjkMatrix(node->GetWldToIjk());
  ijk->SetInput(vol->GetOutput());
  ijk->SetInputOrderString(node->GetScanOrder());
  ijk->SetOutputOrderString(orderString);
  ijk->SetSlice((int)offset);
  ijk->ComputeTransform();
  ijk->ComputeOutputExtent();
  ijk->ComputeReformatMatrix(ref);
}

float vtkMrmlSlicer::GetOffsetForComputation(int s)
{
  float uOff, cOff;
  
  uOff = this->GetOffset(s);

  // Reformatted slices are defined by:
  //   origin = 2D point in lower left of reformatted image
  //   P  = 3D focal point
  //   Ux = 3D vector from origin to right side
  //   Uy = 3D vector from origin to top side
  //   Uz = Ux cross Uy
  //   Up = center of reformatted image = P + offset*Uz
  // For axial slices:
  //   Ux=(-1,0,0) to go from R to L
  //   Uy=( 0,1,0) to go from P to A
  //   therefore:
  //   Uz=(0,0,-1) to go from S to I
  //
  // To allow the user to specify the location of the slice, 
  // we provide a number called "offset" that goes from I to S.
  // Note that this has opposite polarity of the number that one
  // would multiply by Uz to arrive at the center of the slice, Up.
  //
  // It is the user's view of offset that is stored in this object's
  // this->Offset[slice][orient] array.  So the GetOffsetForComputation()
  // function inverts this user's offset in the case of Axial orientation.
  //
  // The full spectrum is as follows: 
  //
  //   Notation: uOff = user's offset for GUI
  //             cOff = offset for computation: Up = P + cOff*Uz
  //
  //   cOff = -uOff whenever uOff and Uz are of opposite polarity
  //   in the table below
  //
  // Orient   Vec  Direction    
  // ---------------------------
  // Axi      Ux   RL (-1, 0, 0)
  //          Uy   PA ( 0, 1, 0)
  //          Uz   SI ( 0, 0,-1)
  //          uOff IS 
  //
  // Sag      Ux   AP ( 0,-1, 0)
  //          Uy   IS ( 0, 0, 1)
  //          Uz   RL (-1, 0, 0)
  //          uOff LR 
  //
  // Cor      Ux   RL (-1, 0, 0)
  //          Uy   IS ( 0, 0, 1)
  //          Uz   PA ( 0, 1, 0)
  //          uOff PA 
  //

  // If the driver is the locator, then the offset is 0
  if (this->Driver[s])
  {
    return 0.0f;
  }

  switch (this->Orient[s])
  {
    case MRML_SLICER_ORIENT_AXIAL:    cOff = -uOff; break;
    case MRML_SLICER_ORIENT_SAGITTAL: cOff = -uOff; break;
    case MRML_SLICER_ORIENT_CORONAL:  cOff =  uOff; break;
    default:                          cOff =  uOff; break;
  }
  return cOff;
}

void vtkMrmlSlicer::SetOffset(int s, float userOffset)
{
  double Uz[3], *P;
  vtkMatrix4x4 *ref = this->ReformatMatrix[s];
  float offset;

  this->Offset[s][this->Orient[s]] = userOffset;
  offset = this->GetOffsetForComputation(s);

  if (this->IsOrientIJK(s))
  {
    this->ComputeReformatMatrixIJK(s, offset, ref);
  }
  else
  {
    Uz[0] = ref->GetElement(0, 2);
    Uz[1] = ref->GetElement(1, 2);
    Uz[2] = ref->GetElement(2, 2);

    P = this->GetP(s);

    for (int i=0; i<3; i++)
    {
      ref->SetElement(i, 3, P[i] + offset * Uz[i]);
    }
    ref->SetElement(3, 3, 1.0);
  }

  // Use ReformatMatix in reformatters
  this->BackReformat[s]->Modified();
  this->ForeReformat[s]->Modified();
  this->LabelReformat[s]->Modified();

  // Use reformat matrix for other arbitrary volumes we may be reformatting
  this->VolumeReformattersModified();
}

//----------------------------------------------------------------------------
// Reformat Matrix
//----------------------------------------------------------------------------
void vtkMrmlSlicer::ComputeReformatMatrix(int s)
{
  double Ux[3], Uy[3], Uz[3], *P, *T, *N;
  int i;
  vtkMatrix4x4 *ref = this->ReformatMatrix[s];
  float offset = this->GetOffsetForComputation(s);
  
  P = this->GetP(s);
  N = this->GetN(s);
  T = this->GetT(s);

  // 1.) Create the R rotation matrix.  If the plane's reference frame has
  //     axis Ux, Uy, Uz, then Ux is the left col of R, Uy is the second, etc.
  // 2.) Concatenate a translation, T, from the origin to the plane's center.
  // Then: M = T*R.
  //
  // (See page 419 and 429 of "Computer Graphics", Hearn & Baker, 1997,
  //  ISBN 0-13-530924-7 for more details.)
  //
  // The Ux axis is the vector across the top of the image from left to right.
  // The Uy axis is the vector down the side of the image from top to bottom.
  // The Uz axis is the vector from the focal point to the camera.
  // The rightmost column of the matrix is the offset, P, to the image center.

  // Use the Reformatters convenience functions to form the reformat matrix.
  // The matrix is a function of the scan orientation and slice order of
  // the background volume.
  //
  if (this->IsOrientIJK(s))
  {
    this->ComputeReformatMatrixIJK(s, offset, ref);
  }
  //
  // Compute reformat matrix from N, T, P
  // 
  else
  {
    switch (this->Orient[s])
	  {
    case MRML_SLICER_ORIENT_AXIAL:
      Ux[0] = -1.0;
      Ux[1] =  0.0;
      Ux[2] =  0.0;
      Uy[0] =  0.0;
      Uy[1] =  1.0;
      Uy[2] =  0.0;
	  break;

    case MRML_SLICER_ORIENT_SAGITTAL:
      Ux[0] =  0.0;
      Ux[1] = -1.0;
      Ux[2] =  0.0;
      Uy[0] =  0.0;
      Uy[1] =  0.0;
      Uy[2] =  1.0;
	  break;

    case MRML_SLICER_ORIENT_CORONAL:
      Ux[0] = -1.0;
      Ux[1] =  0.0;
      Ux[2] =  0.0;
      Uy[0] =  0.0;
      Uy[1] =  0.0;
      Uy[2] =  1.0;
	  break;

    case MRML_SLICER_ORIENT_INPLANE:
      // In the plane of N, and normal to T
      // Ux = -N x T = T x N
      // Uy = -N
      // Uz =  T
      Cross(Ux, T, N);
      Uy[0] = - N[0];
      Uy[1] = - N[1];
      Uy[2] = - N[2];
      break;

    case MRML_SLICER_ORIENT_INPLANE90:
      // Ux = -T
      // Uy = -N
      Ux[0] = - T[0];
      Ux[1] = - T[1];
      Ux[2] = - T[2];
      Uy[0] = - N[0];
      Uy[1] = - N[1];
      Uy[2] = - N[2];
      break;

    case MRML_SLICER_ORIENT_INPLANENEG90:
      // Ux =  T
      // Uy = -N
      Ux[0] =   T[0];
      Ux[1] =   T[1];
      Ux[2] =   T[2];
      Uy[0] = - N[0];
      Uy[1] = - N[1];
      Uy[2] = - N[2];
      break;

    case MRML_SLICER_ORIENT_PERP:
      // Ux = N x T
      // Uy = T
      // Uz = -N
      Cross(Ux, N, T);
      Uy[0] =   T[0];
      Uy[1] =   T[1];
      Uy[2] =   T[2];
      break;
	  }//switch

    // Form Uz
	  Cross(Uz, Ux, Uy);
    Normalize(Ux);
    Normalize(Uy);
    Normalize(Uz);
		
    // Set ReformatMatrix
    for(i=0; i<3; i++) 
    {
      ref->SetElement(i, 0, Ux[i]);
      ref->SetElement(i, 1, Uy[i]);
      ref->SetElement(i, 2, Uz[i]);
      ref->SetElement(i, 3, P[i] + offset * Uz[i]);
	  }
    for(i=0; i<3; i++) 
    {
      ref->SetElement(3, i, 0.0);
	  }
    ref->SetElement(3, 3, 1.0);

  }//else

  // Use ReformatMatrix in reformatters
  this->BackReformat[s]->Modified();
  this->ForeReformat[s]->Modified();
  this->LabelReformat[s]->Modified();
}


////////////////////////////////////////////////////////////////////////////////
//                    POINTS
////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// SetScreenPoint
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetScreenPoint(int s, int x, int y)
{
  // Convert from 512x512 to 256x256
  if (this->DoubleSliceSize[s] == 1) {
    x /= 2;
    y /= 2;
  }

  // Convert from zoom space to reformat space
  float ctr[2];
  this->Zoom[s]->GetCenter(ctr);
  if (this->Zoom[s]->GetMagnification() != 1.0 || 
    this->Zoom[s]->GetAutoCenter() == 0 ||
    (ctr[0] == 0.0 && ctr[1] == 0.0))
  {
    this->Zoom[s]->SetZoomPoint(x, y);
    this->Zoom[s]->GetOrigPoint(this->ReformatPoint);
  } 
  else 
  {
    this->ReformatPoint[0] = x;
    this->ReformatPoint[1] = y;
  }
}

//----------------------------------------------------------------------------
// Point
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetReformatPoint(int s, int x, int y)
{
  vtkMrmlDataVolume *vol = this->GetIJKVolume(s);
  vtkImageReformat *ref = this->GetIJKReformat(s);
  vtkMrmlVolumeNode *node = (vtkMrmlVolumeNode*) vol->GetMrmlNode();
  // Convert (s,x,y) to (i,j,k), (r,a,s), and (x,y,z).
  // (s,x,y) = slice, x,y coordinate on slice
  // (r,a,s) = this->WldPoint = mm float
  // (i,j,k) = this->IjkPoint = 0-based float
  // (x,y,z) = this->Seed     = extent-based int (x = extent[0]+i)

  // First convert to ras, then ijk, then xyz
  ref->SetPoint(x, y);
  ref->GetWldPoint(this->WldPoint);
  ref->GetIjkPoint(this->IjkPoint);

  int ext[6];
  vol->GetOutput()->GetWholeExtent(ext);

  this->Seed[0] = ext[0] + (int)(this->IjkPoint[0] + 0.49);
  this->Seed[1] = ext[2] + (int)(this->IjkPoint[1] + 0.49);
  this->Seed[2] = ext[4] + (int)(this->IjkPoint[2] + 0.49);

  if (this->IsOrientIJK(s))
  {
    char orderString[3];

    switch (this->Orient[s])
    {
      case MRML_SLICER_ORIENT_ORIGSLICE:
        sprintf(orderString, "%s", node->GetScanOrder());
        break;
      case MRML_SLICER_ORIENT_AXISLICE:
        sprintf(orderString, "IS");
        break;
      case MRML_SLICER_ORIENT_SAGSLICE:
        sprintf(orderString, "LR");
        break;
      case MRML_SLICER_ORIENT_CORSLICE:
        sprintf(orderString, "PA");
        break;
    }//switch

    vtkImageReformatIJK *ijk = this->ReformatIJK;
    ijk->SetWldToIjkMatrix(node->GetWldToIjk());
    ijk->SetInput(vol->GetOutput());
    ijk->SetInputOrderString(node->GetScanOrder());
    ijk->SetOutputOrderString(orderString);
    ijk->SetSlice(this->Offset[s][this->Orient[s]]);
    ijk->ComputeTransform();
    ijk->ComputeOutputExtent();
    ijk->SetIJKPoint(this->Seed[0], this->Seed[1], this->Seed[2]);
    ijk->GetXYPoint(this->Seed2D);
  }
  else
  {
    this->Seed2D[0] = ext[0] + x;
    this->Seed2D[1] = ext[2] + y;
  }
  this->Seed2D[2] = 0;
}

//----------------------------------------------------------------------------
// DrawComputeIjkPoints
//----------------------------------------------------------------------------
void vtkMrmlSlicer::DrawComputeIjkPoints()
{
  float *rasPt;
  int ijkPt[3], i, n, x=0, y=0;
  int s = this->GetActiveSlice();
  vtkPoints *ijk = this->DrawIjkPoints;
  vtkPoints *ras = this->PolyDraw->GetPoints();

  ijk->Reset();
  n = ras->GetNumberOfPoints();
  for (i=0; i<n; i++)
  {
    rasPt = ras->GetPoint(i);
    this->SetReformatPoint(s, (int)(rasPt[0]), (int)(rasPt[1]));
    this->GetSeed2D(ijkPt);

    if (i == 0 || ijkPt[0] != x || ijkPt[1] != y)
    {
      ijk->InsertNextPoint((float)(ijkPt[0]), (float)(ijkPt[1]),
        (float)(ijkPt[2]));
    } 
    x = ijkPt[0];
    y = ijkPt[1];
  }
}

//----------------------------------------------------------------------------
// Pixel Values
//----------------------------------------------------------------------------
int vtkMrmlSlicer::GetBackPixel(int s, int x, int y)
{
  int ext[6];
  vtkImageData *data = this->BackReformat[s]->GetOutput();

  data->GetWholeExtent(ext);
  if (x >= ext[0] && x <= ext[1] && y >= ext[2] && y <= ext[3])
  {
    return data->GetPointData()->GetScalars()->GetScalar(y*(ext[1]-ext[0]+1)+x);
  }
  return 0;
}

int vtkMrmlSlicer::GetForePixel(int s, int x, int y)
{
  int ext[6];

  if (this->ForeVolume[s] == this->NoneVolume)
  {
    return 0;
  }
  vtkImageData *data = this->ForeReformat[s]->GetOutput();

  data->GetWholeExtent(ext);
  if (x >= ext[0] && x <= ext[1] && y >= ext[2] && y <= ext[3])
  {
    return data->GetPointData()->GetScalars()->GetScalar(y*(ext[1]-ext[0]+1)+x);
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//                          VIEW
////////////////////////////////////////////////////////////////////////////////


//----------------------------------------------------------------------------
// Cursor
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetShowCursor(int vis) 
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->Cursor[s]->SetShowCursor(vis);
  }
}

// DAVE need to call with SetAnnoColor
void vtkMrmlSlicer::SetCursorColor(float red, float green, float blue) 
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->Cursor[s]->SetCursorColor(red, green, blue);
  }
}

void vtkMrmlSlicer::SetNumHashes(int hashes) 
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->Cursor[s]->SetNumHashes(hashes);
  }
}

//----------------------------------------------------------------------------
// View direction
//----------------------------------------------------------------------------
void vtkMrmlSlicer::ComputeNTPFromCamera(vtkCamera *camera)
{
  int i;
  double *VPN; // View Plane Normal vector
  double *VU;  // View Up vector
  double *FP;  // Focal Point

  if (camera == NULL)
  {
	  vtkErrorMacro(<< "ComputeNTPFromCamera: NULL camera");
  }
  VPN = camera->GetViewPlaneNormal();
  VU  = camera->GetViewUp();
  FP  = camera->GetFocalPoint();

  // Compute N, T, P as if the view direction were the locator.
  // For the locator:
  //   N points along the needle toward the tip.
  //   T points from the handle's center back toward the cable.
  // So:
  //   N = -VPN
  //   T = VPN x VU
  //   P = FP

  for (i=0; i<3; i++)
  {
	  this->CamN[i] = -VPN[i];
  }
  Cross(this->CamT, VPN, VU);

  Normalize(this->CamN);
  Normalize(this->CamT);

  for (i=0; i<3; i++)
  {
	  this->CamP[i] = FP[i];
  }

  for (i=0; i<3; i++)
  {
    this->ComputeReformatMatrix(i);
  }
}

void vtkMrmlSlicer::SetDirectNTP(float nx, float ny, float nz,
  float tx, float ty, float tz, float px, float py, float pz)
{
  this->DirN[0] = nx;
  this->DirN[1] = ny;
  this->DirN[2] = nz;
  this->DirT[0] = tx;
  this->DirT[1] = ty;
  this->DirT[2] = tz;
  this->DirP[0] = px;
  this->DirP[1] = py;
  this->DirP[2] = pz;

  for (int s=0; s<NUM_SLICES; s++)
  {
    this->ComputeReformatMatrix(s);
  }
}

double *vtkMrmlSlicer::GetP(int s)
{
  if (this->Driver[s] == 0) 
  {
    return this->CamP;
  } else {
    return this->DirP;
  }
}

double *vtkMrmlSlicer::GetT(int s)
{
  if (this->Driver[s] == 0) 
  {
    return this->CamT;
  } else {
    return this->DirT;
  }
}

double *vtkMrmlSlicer::GetN(int s)
{
  if (this->Driver[s] == 0) 
  {
    return this->CamN;
  } else {
    return this->DirN;
  }
}

// DAVE: Update the GUI after calling this
void vtkMrmlSlicer::SetFieldOfView(float fov)
{
  this->FieldOfView = fov;

  this->ComputeOffsetRange();

  for(int s=0; s<NUM_SLICES; s++)
  {
    this->BackReformat[s]->SetFieldOfView(fov);
    this->ForeReformat[s]->SetFieldOfView(fov);
    this->LabelReformat[s]->SetFieldOfView(fov);
  }

  // arbitrary volume reformatting
  this->VolumeReformattersSetFieldOfView(fov);
}

//----------------------------------------------------------------------------
// Zoom
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetZoom(float mag)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->SetZoom(s, mag);
  }
}

void vtkMrmlSlicer::SetZoom(int s, float mag)
{
  this->Zoom[s]->SetMagnification(mag);
  this->BuildLowerTime.Modified();
}

void vtkMrmlSlicer::SetZoomCenter(int s, float x, float y)
{
  this->Zoom[s]->SetCenter(x, y);
  this->BuildLowerTime.Modified();
  this->GetZoomCenter();
}

void vtkMrmlSlicer::GetZoomCenter()
{
  this->Zoom[0]->GetCenter(this->ZoomCenter0);
  this->Zoom[1]->GetCenter(this->ZoomCenter1);
  this->Zoom[2]->GetCenter(this->ZoomCenter2);
}

void vtkMrmlSlicer::SetZoomAutoCenter(int s, int yes)
{
  this->Zoom[s]->SetAutoCenter(yes);
  this->Zoom[s]->Update();
  this->GetZoomCenter();
  this->BuildLowerTime.Modified();
}

//----------------------------------------------------------------------------
// Label LUT
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetLabelIndirectLUT(vtkIndirectLookupTable *lut)
{
  // This block is basically vtkSetObjectMacro
  if (this->LabelIndirectLUT != lut)
  { 
    if (this->LabelIndirectLUT != NULL) 
    { 
      this->LabelIndirectLUT->UnRegister(this); 
    }
    this->LabelIndirectLUT = lut;

    if (this->LabelIndirectLUT != NULL) 
    { 
      this->LabelIndirectLUT->Register(this); 
    }
    this->Modified();
  }

  for (int s=0; s<NUM_SLICES; s++)
  {
    this->LabelMapper[s]->SetLookupTable(this->LabelIndirectLUT);
  }
  this->BuildUpperTime.Modified();
}

//----------------------------------------------------------------------------
// Fore Opacity
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetForeOpacity(float opacity)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->Overlay[s]->SetOpacity(1, opacity);
  }
}

//----------------------------------------------------------------------------
// Fore Opacity
//----------------------------------------------------------------------------
void vtkMrmlSlicer::SetForeFade(int fade)
{
  for (int s=0; s<NUM_SLICES; s++)
  {
	  this->Overlay[s]->SetFade(1, fade);
  }
}

//----------------------------------------------------------------------------
// Convenient reformatting functions: odonnell, 5/4/2001
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Description:
// Use the same reformat matrix as slice s.
// Causes this volume to be reformatted along with this slice.
// (reformatter will update when slice number changes.)
//
// Currently this is not used.
void vtkMrmlSlicer::ReformatVolumeLikeSlice(vtkMrmlDataVolume *v, int s)
{
  // find the reformatter for this volume
  vtkImageReformat *reformat = this->GetVolumeReformatter(v);

  if (reformat != NULL)
    reformat->SetReformatMatrix(this->ReformatMatrix[s]);
}

//----------------------------------------------------------------------------
// Description:
// Add a volume for reformatting: this may be any volume in the 
// slicer, and the reformatted output can be obtained by
// Slicer->GetReformatOutputFromVolume(vtkMrmlDataVolume *v).
// Currently only reformatting along with the slices is 
// supported, but code may be added to use arbitrary 
// reformat matrices if needed.
// Note that this does not allow duplicate entries: so now you may
// only reformat a certain volume with one reformatter.
//
//
// For now, only reformatting that mirrors the active slice is supported.
// This happens automatically and the reformatters update when the 
// active slice or slice offset change.  In the future this should
// be made more general to allow arbitrary reformatting.
void vtkMrmlSlicer::AddVolumeToReformat(vtkMrmlDataVolume *v)
{
  int index = this->VolumesToReformat->IsItemPresent(v);
  if (index) 
    {
      //vtkErrorMacro("already reformatting volume " << v );  
      return;
    }

  if (index > this->MaxNumberOfVolumesToReformat)
    {
      vtkErrorMacro("increase the number of volumes the slicer can reformat"); 
      return;
    }

  // make a reformatter object to do this
  vtkImageReformat *reformat = vtkImageReformat::New();

  // set its input to be this volume
  vtkMrmlVolumeNode *node = (vtkMrmlVolumeNode*) v->GetMrmlNode();
  reformat->SetInput(v->GetOutput());
  reformat->SetInterpolate(node->GetInterpolate());
  reformat->SetWldToIjkMatrix(node->GetWldToIjk());

  // bookkeeping: add to list of volumes
  this->VolumesToReformat->AddItem(v);
  index = this->VolumesToReformat->IsItemPresent(v);
  vtkDebugMacro("add: index of volume:" << index );

  // bookkeeping: the index of the reformatter and volume will be the same:
  // add to list of reformatters
  this->VolumeReformatters->InsertValue(index, reformat);

  // for now only allow reformatting along with the active slice
  reformat->SetReformatMatrix(this->ReformatMatrix[this->GetActiveSlice()]);
  reformat->Modified();
  
  // set the field of view to match other volumes reformatted in the slicer
  reformat->SetFieldOfView(this->FieldOfView);
}

//----------------------------------------------------------------------------
// Description:
// Stop reformatting all volumes.  Do this when your module is done
// with this input (such as when it is exited).
void vtkMrmlSlicer::RemoveAllVolumesToReformat()
{
  // clear all pointers to volumes
  this->VolumesToReformat->RemoveAllItems();

  // delete all reformatters
  for (int i = 0; i < this->MaxNumberOfVolumesToReformat; i++)
    {
      vtkImageReformat *reformat = 
	(vtkImageReformat *) this->VolumeReformatters->GetValue(i);
      if (reformat != NULL) 
	{
	  // kill it 
	  reformat->Delete();
	  this->VolumeReformatters->SetValue(i,NULL);
	}
    }
}

//----------------------------------------------------------------------------
// Description:
// internal use: get the reformatter used for this volume.
vtkImageReformat *vtkMrmlSlicer::GetVolumeReformatter(vtkMrmlDataVolume *v)
{
  int index = this->VolumesToReformat->IsItemPresent(v);
  if (index) 
    {
      // get pointer to reformatter
      vtkImageReformat *reformat;
      reformat = (vtkImageReformat *) this->VolumeReformatters->GetValue(index);
      return reformat;
    }
  else
    {
      vtkErrorMacro("Not reformatting this volume: " << v );  
      return NULL;
    }
}

//----------------------------------------------------------------------------
// Description:
// internal use: mark all reformatters as modified so their output
// will update. (this is called when the reformat matrix for a slice
// changes, so these reformatted slices can follow the original
// three slices in the slicer's slice windows).
void vtkMrmlSlicer::VolumeReformattersModified()
{
  int max = this->VolumeReformatters->GetNumberOfTuples();

  for (int i = 0; i < max; i++)
    {
      vtkImageReformat *ref = 
	(vtkImageReformat *)this->VolumeReformatters->GetValue(i);
      if (ref != NULL)
	{
	  // for now only allow reformatting along with the active slice
	  ref->SetReformatMatrix(this->ReformatMatrix[this->GetActiveSlice()]);
	  ref->Modified();
	}
    }
}

void vtkMrmlSlicer::VolumeReformattersSetFieldOfView(float fov)
{
  int max = this->VolumeReformatters->GetNumberOfTuples();

  for (int i = 0; i < max; i++)
    {
      vtkImageReformat *ref = 
	(vtkImageReformat *)this->VolumeReformatters->GetValue(i);
      if (ref != NULL)
	{
	  ref->SetFieldOfView(fov);
	}
    }
}

