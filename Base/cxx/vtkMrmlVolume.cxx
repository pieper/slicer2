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
#include "vtkMrmlVolume.h"
#include "vtkObjectFactory.h"
#include "vtkImageReader.h"
#include "vtkImageWriter.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageCopy.h"
#include <time.h>

#include "vtkImageDICOMReader.h"

//------------------------------------------------------------------------------
vtkMrmlVolume* vtkMrmlVolume::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMrmlVolume");
  if(ret)
  {
    return (vtkMrmlVolume*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMrmlVolume;
}

//----------------------------------------------------------------------------
vtkMrmlVolume::vtkMrmlVolume()
{
  // Allocate VTK objects
  this->Accumulate = vtkImageAccumulateDiscrete::New();
  this->Bimodal = vtkImageBimodalAnalysis::New();
  this->Resize = vtkImageResize::New();
  this->HistPlot = vtkImagePlot::New();
  this->IndirectLUT = vtkIndirectLookupTable::New();

  // Objects thate should be set by the user, but can be self-created
  this->MrmlNode = NULL;
  this->ImageData = NULL;
  this->LabelIndirectLUT = NULL;

  this->UseLabelIndirectLUT = 0;
 
  // W/L Slider range 
  this->RangeAuto = 1;
  this->RangeLow = 0;
  this->RangeHigh = 0;

  this->HistogramColor[0] = 1;
  this->HistogramColor[1] = 0;
  this->HistogramColor[2] = 0;
  
  this->ProcessObject = NULL;

  // NOTE: ImageData has a reference count of 2 because it is the ImageData
  // of this and the Input of this->Accumulate.
  // If we call SetImageData [something else] then the reference count drops
  // to 1.  Then when we call this->Update, then this->Accumulate gets a
  // new input and the reference count of the original ImageData drops to 0
  // and the object is deleted from memory.

}

//----------------------------------------------------------------------------
vtkMrmlVolume::~vtkMrmlVolume()
{
  // Delete if self-created or if no one else is using it
  if (this->ImageData != NULL) 
  {
    this->ImageData->UnRegister(this);
  }
  if (this->MrmlNode != NULL)
  {
    this->MrmlNode->UnRegister(this);
  }
  if (this->LabelIndirectLUT != NULL)
  {
    this->LabelIndirectLUT->UnRegister(this);
  }

  // Delete objects we allocated
  this->Accumulate->Delete();
  this->Bimodal->Delete();
  this->Resize->Delete();
  this->HistPlot->Delete();
  this->IndirectLUT->Delete();
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkProcessObject::PrintSelf(os, indent);

  os << indent << *this->HistPlot   << endl;
  os << indent << "UseLabelIndirectLUT: " << this->UseLabelIndirectLUT << endl;
  os << indent << "Range Low: " << this->RangeLow << endl;
  os << indent << "Range High: " << this->RangeHigh << endl;
  os << indent << "Range Auto: " << this->RangeAuto << endl;

  // vtkSetObjectMacro
  os << indent << "MrmlNode: " << this->MrmlNode << "\n";
  if (this->MrmlNode)
  {
    this->MrmlNode->PrintSelf(os,indent.GetNextIndent());
  }

  os << indent << "ImageData: " << this->ImageData << "\n";
  if (this->ImageData)
  {
    this->ImageData->PrintSelf(os,indent.GetNextIndent());
  }

  os << indent << "LabelIndirectLUT: " << this->LabelIndirectLUT << "\n";
  if (this->LabelIndirectLUT)
  {
    this->LabelIndirectLUT->PrintSelf(os,indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::CopyNode(vtkMrmlVolume *volume)
{
  this->GetMrmlNode()->Copy(volume->GetMrmlNode());
}

//----------------------------------------------------------------------------
// Determine the modified time of this object
unsigned long int vtkMrmlVolume::GetMTime()
{
  unsigned long result, t;

  result = vtkObject::GetMTime();
 
  // ImageData
  if (this->ImageData)
  {
    t = this->ImageData->GetMTime(); 
    result = (t > result) ? t : result;
  }
 
  // MrmlNode
  if (this->MrmlNode)
  {
    t = this->MrmlNode->GetMTime(); 
    result = (t > result) ? t : result;
  }
 
  // LabelIndirectLUT 
  if (this->UseLabelIndirectLUT && this->LabelIndirectLUT)
  {
    t = this->LabelIndirectLUT->GetMTime(); 
    result = (t > result) ? t : result;
  }
  return result;
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::CheckImageData()
{
  // If the user has not set the ImageData, then create it.
  // The key is to perform: New(), Register(), Delete().
  // Then we can call UnRegister() in the destructor, and it will delete
  // the object if no one else is using it.  We don't have to distinguish
  // between whether we created the object, or someone else did!

  if (this->ImageData == NULL)
  {
    // Give it a size that matches with the MrmlNode (but 2D)
    int dim[2];
    short s=0;
    vtkMrmlVolumeNode *node = this->MrmlNode;
    node->GetDimensions(dim);

    vtkImageCanvasSource2D *canvas = vtkImageCanvasSource2D::New();
    canvas->SetScalarType(node->GetScalarType());
    canvas->SetNumberOfScalarComponents(node->GetNumScalars());
    canvas->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, 0);
    canvas->SetSpacing(node->GetSpacing());
    canvas->SetDrawColor(s);
    canvas->FillBox(0, dim[0]-1, 0, dim[0]-1);

    vtkImageCopy *copy = vtkImageCopy::New();
    copy->SetInput(canvas->GetOutput());
    copy->Update();
    this->SetImageData(copy->GetOutput());
    copy->SetOutput(NULL);
    copy->Delete();
	  canvas->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::CheckMrmlNode()
{
  // If the user has not set the ImageData, then create it.
  // The key is to perform: New(), Register(), Delete().
  // Then we can call UnRegister() in the destructor, and it will delete
  // the object if no one else is using it.  We don't have to distinguish
  // between whether we created the object, or someone else did!

  if (this->MrmlNode == NULL)
  {
    this->MrmlNode = vtkMrmlVolumeNode::New();
    this->MrmlNode->Register(this);
    this->MrmlNode->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::CheckLabelIndirectLUT()
{
  if (this->LabelIndirectLUT == NULL)
  {
    this->LabelIndirectLUT = vtkIndirectLookupTable::New();
    this->LabelIndirectLUT->Register(this);
    this->LabelIndirectLUT->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::Update()
{
  int ext[6];
  
  // We really should have an Update time that we compare with the
  // MTime, but since the other objects inside this class do this, 
  // its alright.

  // Create objects that the user hasn't already set
  this->CheckMrmlNode();
  this->CheckImageData();
  this->CheckLabelIndirectLUT();

  // Connect pipeline
  this->Accumulate->SetInput(this->ImageData);
  this->Accumulate->Update();
  this->Bimodal->SetInput(this->Accumulate->GetOutput());
  this->Bimodal->Update();
  this->Bimodal->GetClipExtent(ext);
  this->Resize->SetInputClipExtent(ext);
  this->Resize->SetInput(this->Accumulate->GetOutput());
  this->Resize->Update();
  this->HistPlot->SetInput(this->Resize->GetOutput());


  // Bound W/L range and apply auto if requested
  this->UpdateWindowLevelThreshold();
}

//----------------------------------------------------------------------------
vtkImageData* vtkMrmlVolume::GetOutput()
{
  this->Update();
  return this->ImageData;
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::UpdateWindowLevelThreshold()
{
  int window, level, upper, lower;

  // Compute the range using the accumulator and bimodal analysis.
  // The common case is auto W/L, so we want this to be the fastest.
  //
  this->Bimodal->Update();

  // Auto Range
  if (this->RangeAuto) 
  {
    this->RangeLow  = this->Bimodal->GetMin();
    this->RangeHigh = this->Bimodal->GetMax();
  }
  
  // Auto W/L
  if (this->MrmlNode->GetAutoWindowLevel())
  {
    this->MrmlNode->SetLevel(this->Bimodal->GetLevel());
    this->MrmlNode->SetWindow(this->Bimodal->GetWindow());
  }

  // Auto Threshold
  if (this->MrmlNode->GetAutoThreshold())
  {
    this->MrmlNode->SetLowerThreshold(this->Bimodal->GetThreshold());
    this->MrmlNode->SetUpperThreshold(this->Bimodal->GetMax());
  }

  // Force Window/Level/Thresh to be within the Range
  level  = this->MrmlNode->GetLevel();
  window = this->MrmlNode->GetWindow();
  upper  = this->MrmlNode->GetUpperThreshold();
  lower  = this->MrmlNode->GetLowerThreshold();

  // Clip Level
  if (level < this->RangeLow) 
  {
    level = this->RangeLow;
    this->MrmlNode->SetLevel(level);
  } 
  else if (level > this->RangeHigh) 
  {
    level = this->RangeHigh;
    this->MrmlNode->SetLevel(level);
  }

  // Clip Window
  if (window < 0) 
  {
    window = 0;
    this->MrmlNode->SetWindow(window);
  }
  if (window > (this->RangeHigh - this->RangeLow + 1)) 
  {
    window = this->RangeHigh - this->RangeLow + 1;
    this->MrmlNode->SetWindow(window);
  }

  // Clip UpperThreshold
  if (upper < this->RangeLow) 
  {
    upper = this->RangeLow;
    this->MrmlNode->SetUpperThreshold(upper);
  } 
  else if (upper > this->RangeHigh) 
  {
    upper = this->RangeHigh;
    this->MrmlNode->SetUpperThreshold(upper);
  }

  // Clip LowerThreshold 
  if (lower < this->RangeLow) 
  {
    lower = this->RangeLow;
    this->MrmlNode->SetLowerThreshold(lower);
  } 
  else if (lower > this->RangeHigh) 
  {
    lower = this->RangeHigh;
    this->MrmlNode->SetLowerThreshold(lower);
  }

  // Do not apply W/L to the LabelIndirectLUT
  if (this->UseLabelIndirectLUT == 0)
  {
    this->IndirectLUT->SetLevel(level);
    this->IndirectLUT->SetWindow(window);
    this->IndirectLUT->SetLowerThreshold(lower);
    this->IndirectLUT->SetUpperThreshold(upper);
    this->IndirectLUT->SetApplyThreshold(this->MrmlNode->GetApplyThreshold());
    this->IndirectLUT->Build();
  }
}

//----------------------------------------------------------------------------
vtkImageData* vtkMrmlVolume::GetHistogramPlot()
{
  int ext[6];
  int signalRange[2];

  // Update pipeline for the plot
  this->Update();
  this->Bimodal->GetClipExtent(ext);
  this->Bimodal->GetSignalRange(signalRange);
  this->Resize->SetInputClipExtent(ext);
  this->HistPlot->SetDataRange(0, signalRange[1]);
  this->HistPlot->SetDataDomain(
    this->Bimodal->GetMin(), this->Bimodal->GetMax());
  this->HistPlot->SetLookupTable(this->GetIndirectLUT());
  this->HistPlot->SetColor(this->HistogramColor);
  this->HistPlot->Update();
  
  return this->HistPlot->GetOutput();
}

//----------------------------------------------------------------------------
void vtkMrmlVolumeProgress(void *arg)
{
  vtkMrmlVolume *self = (vtkMrmlVolume *)(arg);
  vtkProcessObject *obj = self->GetProcessObject();
  if (obj)
  {
    self->UpdateProgress(obj->GetProgress());
  }
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::Read()
{
  int ext[6], *range, *dim;
  vtkMrmlVolumeNode *node;

  // Create objects that don't already exist
  this->CheckMrmlNode();
  node = this->MrmlNode;

  // Start progress reporting
  if (this->StartMethod)
  {
    (*this->StartMethod)(this->StartMethodArg);
  }
  
  range = node->GetImageRange();
  dim = node->GetDimensions();
  ext[0] = 0;
  ext[1] = dim[0] - 1;
  ext[2] = 0;
  ext[3] = dim[1] - 1;
  ext[4] = range[0];
  ext[5] = range[1];

  //
  // Modified by Attila Tanacs 8/28/2000
  //

  vtkImageSource *reader;

  // try as DICOM

  if(node->GetNumberOfDICOMFiles() > 0)
    { // DICOM
      vtkImageDICOMReader *dcmreader = vtkImageDICOMReader::New();
      dcmreader->SetNumberOfScalarComponents(node->GetNumScalars());
      dcmreader->SetDataScalarType(node->GetScalarType());
      dcmreader->SetDataByteOrder(node->GetLittleEndian());
      dcmreader->SetDataSpacing(node->GetSpacing());
      dcmreader->SetFilePattern(node->GetFilePattern());
      dcmreader->SetFilePrefix(node->GetFullPrefix());
      dcmreader->SetDataExtent(ext);

      dcmreader->SetDICOMFileNames(node->GetNumberOfDICOMFiles(), node->GetDICOMFileNamesPointer());

      //if(dcmreader->GetDICOMHeaderSize(ext[4]) != -1)
      //{ // DICOM
      reader = (vtkImageSource *) dcmreader;
      //}
    }
  else
    { // Not DICOM (use the original code)
      //dcmreader->Delete();

      vtkImageReader *ireader = vtkImageReader::New();
      ireader->SetNumberOfScalarComponents(node->GetNumScalars());
      ireader->SetDataScalarType(node->GetScalarType());
      ireader->SetDataByteOrder(node->GetLittleEndian());
      ireader->SetDataSpacing(node->GetSpacing());
      ireader->SetFilePattern(node->GetFilePattern());
      ireader->SetFilePrefix(node->GetFullPrefix());
      ireader->SetDataExtent(ext);
      
      reader = (vtkImageSource *) ireader;
   }

  //
  //
  //


  // Progress callback
  this->ProcessObject = reader;
  reader->SetProgressMethod(vtkMrmlVolumeProgress, (void *)this);
 
  // Read it
  reader->Update();

  // Detach image data from reader
  this->SetImageData(reader->GetOutput());
  reader->SetOutput(NULL);
  this->ProcessObject = NULL;
  reader->Delete();

  // End progress reporting
  if (this->EndMethod)
  {
    (*this->EndMethod)(this->EndMethodArg);
  } 

  // Update W/L
  this->Update();
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::Write()
{
  vtkMrmlVolumeNode *node;

  this->CheckMrmlNode();
  this->CheckImageData();
  node = this->MrmlNode;
  
  // Start progress reporting
  if (this->StartMethod)
  {
    (*this->StartMethod)(this->StartMethodArg);
  }
  
  vtkImageWriter *writer = vtkImageWriter::New();
  writer->SetFilePattern(node->GetFilePattern());
  writer->SetFilePrefix(node->GetFullPrefix());
  writer->SetInput(this->ImageData);
  
  // Progress callback
  writer->SetProgressMethod(vtkMrmlVolumeProgress, (void *)this);
  // The progress callback function needs a handle to the writer 
  this->ProcessObject = writer;
 
  // Write it
  writer->Write();

  writer->SetInput(NULL);
  writer->Delete();

  // End progress reporting
  if (this->EndMethod)
  {
    (*this->EndMethod)(this->EndMethodArg);
  } 
}

//----------------------------------------------------------------------------
void vtkMrmlVolume::SetHistogramWidth(int w)
{
  int ext[6];

  memset(ext, 0, 6*sizeof(int));
  ext[1] = w-1;
  this->Resize->SetOutputWholeExtent(ext);
}

//----------------------------------------------------------------------------
vtkIndirectLookupTable* vtkMrmlVolume::GetIndirectLUT()
{
  if (this->UseLabelIndirectLUT) 
  {
    return this->LabelIndirectLUT;
  }
  return this->IndirectLUT;
}

