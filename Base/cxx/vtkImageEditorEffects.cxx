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
#include <stdio.h>
#include "vtkImageEditorEffects.h"
#include "vtkImageErode.h"
#include "vtkImageConnectivity.h"
#include "vtkImageLabelChange.h"
#include "vtkImageFillROI.h"
#include "vtkImageThresholdBeyond.h"
#include "vtkImageCopy.h"
#include "vtkObjectFactory.h"
#include <time.h>

//------------------------------------------------------------------------------
vtkImageEditorEffects* vtkImageEditorEffects::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageEditorEffects");
  if(ret)
  {
    return (vtkImageEditorEffects*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageEditorEffects;
}

//----------------------------------------------------------------------------
vtkImageEditorEffects::vtkImageEditorEffects()
{
  this->IslandSize = 0;
  this->LargestIslandSize = 0;
}

//----------------------------------------------------------------------------
vtkImageEditorEffects::~vtkImageEditorEffects()
{
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::Threshold(float min, float max, 
  float in, float out, int replaceIn, int replaceOut)
{
  vtkImageThresholdBeyond *thresh = vtkImageThresholdBeyond::New();
  thresh->SetReplaceIn(replaceIn);
  thresh->SetReplaceOut(replaceOut);
  thresh->SetInValue(in);
  thresh->SetOutValue(out);
  thresh->ThresholdBetween(min, max);

  this->Apply(thresh, thresh);

  thresh->SetInput(NULL);
  thresh->SetOutput(NULL);
  thresh->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::Draw(int value, vtkPoints *points, int radius, 
	char *shape)
{
  vtkImageFillROI *fill = vtkImageFillROI::New();

  fill->SetValue(value);
  fill->SetRadius(radius);
  fill->SetShapeString(shape);
  fill->SetPoints(points);

  this->Apply(fill, fill);

  fill->SetInput(NULL);
  fill->SetOutput(NULL);
  fill->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::Erode(float fg, float bg, int neighbors,
                              int iterations)
{
  vtkImageErode *erode = vtkImageErode::New();

  erode->SetForeground(fg);
  erode->SetBackground(bg);

	if (neighbors == 8) 
  {
    erode->SetNeighborTo8();
  }
  else
  {
    erode->SetNeighborTo4();
  }

  for (int i=0; i<iterations; i++)
  {
    this->Apply(erode, erode);
  }

  erode->SetInput(NULL);
  erode->SetOutput(NULL);
  erode->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::Dilate(float fg, float bg, int neighbors,
                               int iterations)
{
  vtkImageErode *erode = vtkImageErode::New();
  
  // Swap bg and fg to dilate
  erode->SetForeground(bg);
  erode->SetBackground(fg);

	if (neighbors == 8) 
  {
		erode->SetNeighborTo8();
  }
	else
  {
		erode->SetNeighborTo4();
  }

  for (int i=0; i<iterations; i++)
  {
    this->Apply(erode, erode);
  }

  erode->SetInput(NULL);
  erode->SetOutput(NULL);
  erode->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::ErodeDilate(float fg, float bg, int neighbors,
                                    int iterations)
{
  vtkImageErode *erode  = vtkImageErode::New();
  vtkImageErode *dilate = vtkImageErode::New();
  
  erode->SetForeground(fg);
  erode->SetBackground(bg);
  dilate->SetForeground(bg);
  dilate->SetBackground(fg);
  dilate->SetInput(erode->GetOutput());

	if (neighbors == 8)
  {
		dilate->SetNeighborTo8();
  }
	else
  {
		  dilate->SetNeighborTo4();
  }

	if (neighbors == 8) 
  {
		erode->SetNeighborTo8();
  }
	else
  {
		erode->SetNeighborTo4();
  }

  for (int i=0; i<iterations; i++)
  {
    this->Apply(erode, dilate);
  }

  erode->SetInput(NULL);
  erode->SetOutput(NULL);
  erode->Delete();

  dilate->SetInput(NULL);
  dilate->SetOutput(NULL);
  dilate->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::DilateErode(float fg, float bg, int neighbors,
                                    int iterations)
{
  vtkImageErode *erode  = vtkImageErode::New();
  vtkImageErode *dilate = vtkImageErode::New();
  
  erode->SetForeground(fg);
  erode->SetBackground(bg);
  dilate->SetForeground(bg);
  dilate->SetBackground(fg);
  erode->SetInput(dilate->GetOutput());

	if (neighbors == 8) 
  {
    erode->SetNeighborTo8();
  }
	else
  {
    erode->SetNeighborTo4();
  }

  if (neighbors == 8) 
  {
    dilate->SetNeighborTo8();
  }
  else
  {
    dilate->SetNeighborTo4();
  }

  for (int i=0; i<iterations; i++)
  {
    this->Apply(dilate, erode);
  }
  erode->SetInput(NULL);
  erode->SetOutput(NULL);
  erode->Delete();

  dilate->SetInput(NULL);
  dilate->SetOutput(NULL);
  dilate->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::IdentifyIslands(int bg, int fgMin, int fgMax)
{
  vtkImageConnectivity *con  = vtkImageConnectivity::New();

  con->SetBackground(bg);
  con->SetMinForeground(fgMin);
  con->SetMaxForeground(fgMax);

  con->SetFunctionToIdentifyIslands();

  this->Apply(con, con);

  con->SetInput(NULL);
  con->SetOutput(NULL);
  con->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::RemoveIslands(int bg, int fgMin, int fgMax,
                                      int minSize)
{
  vtkImageConnectivity *con  = vtkImageConnectivity::New();
  int native = 0;

  con->SetBackground(bg);
  con->SetMinForeground(fgMin);
  con->SetMaxForeground(fgMax);

  con->SetFunctionToRemoveIslands();
  con->SetMinSize(minSize);

   // Can we use native slicing for speed?
  if (!strcmp(this->GetOutputSliceOrder(),"SI") || 
      !strcmp(this->GetOutputSliceOrder(),"IS"))
  {
    if (!strcmp(this->GetInputSliceOrder(),"SI") || 
        !strcmp(this->GetInputSliceOrder(),"IS"))
    {
      native = 1;
    }
  }
  if (!strcmp(this->GetOutputSliceOrder(),"PA") || 
      !strcmp(this->GetOutputSliceOrder(),"AP"))
  {
    if (!strcmp(this->GetInputSliceOrder(),"PA") || 
        !strcmp(this->GetInputSliceOrder(),"AP"))
    {
      native = 1;
    }
  }
  if (!strcmp(this->GetOutputSliceOrder(),"RL") || 
      !strcmp(this->GetOutputSliceOrder(),"LR"))
  {
    if (!strcmp(this->GetInputSliceOrder(),"RL") || 
        !strcmp(this->GetInputSliceOrder(),"LR"))
    {
      native = 1;
    }
  }
  if (this->GetDimension() != EDITOR_DIM_MULTI)
  {
    native = 0;
  }
  if (native)
  {
    con->SliceBySliceOn();
    this->SetDimensionTo3D();
  }

  this->Apply(con, con);

  if (native)
  {
    this->SetDimensionToMulti();
  }

  con->SetInput(NULL);
  con->SetOutput(NULL);
  con->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::ChangeIsland(int outputLabel, 
	int xSeed, int ySeed, int zSeed)
{
  vtkImageConnectivity *con  = vtkImageConnectivity::New();

	con->SetFunctionToChangeIsland();
	con->SetOutputLabel(outputLabel);
	con->SetSeed(xSeed, ySeed, zSeed);
  con->SetBackground(0);

  this->Apply(con, con);

  con->SetInput(NULL);
  con->SetOutput(NULL);
  con->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::MeasureIsland(int xSeed, int ySeed, int zSeed)
{
  vtkImageConnectivity *con  = vtkImageConnectivity::New();

	con->SetFunctionToMeasureIsland();
	con->SetSeed(xSeed, ySeed, zSeed);
  con->SetBackground(0);

  this->Apply(con, con);

  this->LargestIslandSize = con->GetLargestIslandSize();
  this->IslandSize = con->GetIslandSize();

  con->SetInput(NULL);
  con->SetOutput(NULL);
  con->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::SaveIsland(int xSeed, int ySeed, int zSeed)
{
  vtkImageConnectivity *con  = vtkImageConnectivity::New();

	con->SetFunctionToSaveIsland();
	con->SetSeed(xSeed, ySeed, zSeed);
  con->SetBackground(0);

  this->Apply(con, con);

  con->SetInput(NULL);
  con->SetOutput(NULL);
  con->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::ChangeLabel(int inputLabel, int outputLabel)
{
  vtkImageLabelChange *change  = vtkImageLabelChange::New();

  change->SetInputLabel(inputLabel);
	change->SetOutputLabel(outputLabel);

  this->Apply(change, change);

  change->SetInput(NULL);
  change->SetOutput(NULL);
  change->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::Clear()
{
  vtkImageCopy *clear  = vtkImageCopy::New();

  clear->ClearOn();

  this->Apply(clear, clear);

  clear->SetInput(NULL);
  clear->SetOutput(NULL);
  clear->Delete();
}

//----------------------------------------------------------------------------
void vtkImageEditorEffects::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageEditor::PrintSelf(os, indent);
}

