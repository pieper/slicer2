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
#include <math.h>
#include "vtkIndirectLookupTable.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkIndirectLookupTable* vtkIndirectLookupTable::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkIndirectLookupTable");
  if(ret)
  {
    return (vtkIndirectLookupTable*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkIndirectLookupTable;
}

// Construct
//----------------------------------------------------------------------------
vtkIndirectLookupTable::vtkIndirectLookupTable()
{
  this->WinLvlMap = vtkUnsignedShortArray::New();
  this->DirectMap = vtkUnsignedShortArray::New();
  this->Map = vtkUnsignedShortArray::New();

  this->MapRange[0] = VTK_SHORT_MIN;
  this->MapRange[1] = VTK_SHORT_MAX;
  this->SetMapRange(this->MapRange[0], this->MapRange[1]);

  this->Window = 256;
  this->Level = 128;
  this->LowerThreshold = VTK_SHORT_MIN;
  this->UpperThreshold = VTK_SHORT_MAX;
  this->ApplyThreshold = 1;

  this->inLoPrev = 0;
  this->inHiPrev = 0;
  this->outLoPrev = 0;
  this->outHiPrev = 0;

  this->Direct = 0;
  this->DirectDefaultIndex = 1;

  this->LookupTable = NULL;
}

//----------------------------------------------------------------------------
vtkIndirectLookupTable::~vtkIndirectLookupTable()
{
  // Delete allocated arrays
  this->WinLvlMap->Delete();
  this->DirectMap->Delete();
  this->Map->Delete();

  // Signal that we're no longer using it
  if (this->LookupTable != NULL) 
  {
    this->LookupTable->UnRegister(this);
  }
}


void vtkIndirectLookupTable::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkScalarsToColors::PrintSelf(os,indent);

  os << indent << "Range:         " << this->Range[0] << " thru " << this->Range[1] << "\n";
  os << indent << "Window:        " << this->Window    << "\n";
  os << indent << "Level:         " << this->Level     << "\n";
  os << indent << "LowerThreshold:" << this->LowerThreshold << "\n";
  os << indent << "UpperThreshold:" << this->UpperThreshold << "\n";
  os << indent << "ApplyThreshold:" << this->ApplyThreshold << "\n";
  os << indent << "Direct:        " << this->Direct    << "\n";
  os << indent << "inLoPrev       " << this->inLoPrev    << "\n";
  os << indent << "inHiPrev       " << this->inHiPrev    << "\n";
  os << indent << "outHiPrev      " << this->outHiPrev    << "\n";
  os << indent << "outLoPrev      " << this->outLoPrev    << "\n";
  os << indent << "Build Time:    " << this->BuildTime.GetMTime() << "\n";

  if (this->LookupTable)
  {
    this->LookupTable->PrintSelf(os,indent.GetNextIndent());
  }
}

// For now, don't allow users to alter the MapRange
void vtkIndirectLookupTable::SetMapRange(long min, long max)
{
  this->MapRange[0] = VTK_SHORT_MIN;
  this->MapRange[1] = VTK_SHORT_MAX;
  this->Range[0] = (vtkFloatingPointType)this->MapRange[0];
  this->Range[1] = (vtkFloatingPointType)this->MapRange[1];
  this->MapOffset = -VTK_SHORT_MIN;

  // Allocate arrays
  this->Map->Allocate(65536);
  this->DirectMap->Allocate(65536);
  this->WinLvlMap->Allocate(65536);
}

// Init to the default (Note: multiply index by 4 for rgba).
//------------------------------------------------------------------------------
void vtkIndirectLookupTable::InitDirect()
{
  long i;
  unsigned short idx = this->DirectDefaultIndex * 4;
  unsigned short *map = this->DirectMap->GetPointer(0);
  
  for (i=this->MapRange[0]; i <= this->MapRange[1]; i++)
  {
    map[this->MapOffset + i] = idx;
  }
}

//------------------------------------------------------------------------------
void vtkIndirectLookupTable::MapDirect(vtkFloatingPointType scalar, int index)
{
  unsigned short *map = this->DirectMap->GetPointer(0);

  // Boundary check
  if (scalar < this->Range[0]) 
  {
    scalar = this->Range[0];
  }
  else if (scalar > this->Range[1])
  {
    scalar = this->Range[1];
  }
  long s = (long)scalar;

  // Map it
  map[this->MapOffset + s] = (index * 4);
}

// Consider the Lookup Table when getting the modified time
// Kilian - This M function does not do anything good, because LookupTable->Build 
// will not doe anthing. This is due to the way how this->Build is written 
// (it uses the function LookupTable->SetTebleValue which disables LookupTable->Build)
// Therefore to update a LookupTable you have to delete it and recreate it and assign it to this!  
//----------------------------------------------------------------------------
unsigned long vtkIndirectLookupTable::GetMTime()
{
  unsigned long t1, t2;

  t1 = this->vtkScalarsToColors::GetMTime();
  if (this->LookupTable)
  {
    t2 = this->LookupTable->GetMTime();
    if (t2 > t1)
    {
      t1 = t2;
    }
  }
  return t1;
}

//----------------------------------------------------------------------------
void vtkIndirectLookupTable::WindowLevel()
{
  double delta;
  short outLo, outHi, window, level;
  short i, inLo, inHi, inFirst, inLast;
  short min=this->MapRange[0];
  short max=this->MapRange[1];
  long offset=this->MapOffset;
  unsigned short *map = this->WinLvlMap->GetPointer(0);
  
  window = (short)this->Window;
  level  = (short)this->Level;

  // The range of input scalar values that will be mapped is [inLo, inHi]
  inLo = level - window / 2;
  inHi = inLo + window - 1;
  if (inHi <= inLo)
  {
    inHi = inLo + 1;
  }

  // The scalars are mapped to indices into the LookupTable.
  // These indices are in the range [outLo, outHi].
  // Assumes the first entry in the LookupTable is for thresholding.
  //
  outLo = 1;
  outHi = this->LookupTable->GetNumberOfColors() - 1;
    
  // Only update the portion of the map that changes since the last
  // time we ran this routine.  The range between [inLo, inHi] always
  // gets updated.  If inLo is greater than the previous inLo, then update
  // the portion between them.  If inHi is less than the previous inHi,
  // then update the portion between them.  If outHi changed since last time,
  // then update from inHi through the end of the map.
  // Update the portion from 'inFirst' through 'inLast', inclusive.
  //
  if (this->outLoPrev == outLo)
  {
      inFirst = this->inLoPrev;
  }
  else
  {
    inFirst = min;
  }

  if (this->outHiPrev == outHi)
  {
    inLast = this->inHiPrev;
  }
  else
  {
    inLast = max;
  }

  // Only update he portion of the map between inLo and inHi, inclusive.
  // Store for next time.
  //
  this->inLoPrev  = inLo;
  this->inHiPrev  = inHi;
  this->outLoPrev = outLo;
  this->outHiPrev = outHi;
        
  // Delta is the step in output corresponding to each step in input
  //
  delta = (double)(outHi - outLo + 1) / (double)(inHi - inLo + 1);
        
  // Integers between 'inLo' and 'inHi' are mapped linearly into the
  // range between 'outLo' and 'outHi'
  //
  for (i = inLo; i < inHi; i++)
  {
    map[offset + i] = 
      (unsigned short)(outLo + delta * (double) (i - inLo)) << 2;
  }

  // Save time by shifting now instead of later
  outLo = outLo << 2;
  outHi = outHi << 2;
        
  // All integer values below 'inLo' are mapped to 'outLo'
  // 
  for (i = inFirst; i <= inLo; i++)
  {
    map[offset + i] = outLo;
  }

  // All integer values above 'inHi' are mapped to 'outHi'
  // We have to stop before 'max', or i rolls over.
  //
  for (i = inHi; i < inLast; i++)
  {
    map[offset + i] = outHi;
  }
 
  // Need this line because if inLast=32767, then i rolls over to -32768
  // if the for loop says (i <= inLast)
  //
  if (inLast > inHi)
  {
    map[offset + inLast] = outHi;
  }

  // We have to do i=inHi in chase iLast < inHi
  //
  map[offset + inHi] = outHi;
}

// Generate lookup table from window and level.
// Table is built as a linear ramp, centered at Level and of width Window.
//----------------------------------------------------------------------------
void vtkIndirectLookupTable::Build()
{
  int numColors;
  short min   = this->MapRange[0];
  short max   = this->MapRange[1];
  long offset = this->MapOffset;
  short lower = (short)this->GetLowerThreshold();
  short upper = (short)this->GetUpperThreshold();
  unsigned short *map       = this->Map->GetPointer(0);
  unsigned short *directMap = this->DirectMap->GetPointer(0);
  unsigned short *winLvlMap = this->WinLvlMap->GetPointer(0);
  
  // If no changes since last build, then do nothing.
  if (this->BuildTime > this->GetMTime())
  {
    return;
  }

  // If there is no lookup table, create one
  // 256 bins grayscale.
  if (this->LookupTable == NULL) 
  {
    this->LookupTable = vtkLookupTable::New();
    this->LookupTable->Register(this);
    this->LookupTable->Delete();
    this->LookupTable->SetNumberOfColors(256);
    this->LookupTable->SetSaturationRange(0, 0);
    this->LookupTable->SetValueRange(0, 1);
    this->LookupTable->Build();
  }

  // Set the first color to be transparent for use when a pixel is
  // outside the threshold

   // Kilian - This Function disables LookupTable->Build() 
   // Therefore to update a LookupTable you have to delete it, recreate it and assign it to this !  
  this->LookupTable->SetTableValue(0, 0, 0, 0, 0);
  
  // Check that the LookupTable has the right number of colors
  numColors = this->LookupTable->GetNumberOfColors();
  if (numColors > (65536 >> 2)) {
    vtkErrorMacro(<< "Build: LookupTable has too many colors.");
    return;
  }
  if (numColors < 2) {
    vtkErrorMacro(<< "Build: LookupTable needs at least 2 colors.");
    return;
  }
  
  // Perform thresholding by overwriting the map with the index to the
  // transparent color for every pixel value outside the threshold.
  // The transparent color is always at index 0.
  // Use the 'direct' for direct mode, and the 'map' otherwise.
  //
  if (this->Direct) 
  {
      memcpy(map, directMap, (max-min+1)*sizeof(short));
  } 
  else 
  {
    this->WindowLevel();
      memcpy(map, winLvlMap, (max-min+1)*sizeof(short));
  }

  // Apply threshold if desired
  if (this->GetApplyThreshold())
  {
    // "AND" threshold 
    // show values >= 'lower' AND <= 'upper'
    // zero values <  'lower' OR  >  'upper'
    if (lower <= upper) 
    {
      // Zero out what's < 'lower'
      memset(map, 0, (lower-min)*sizeof(short));

      // Zero out what's > 'above'
      memset(&(map[offset+upper+1]), 0, (max-upper)*sizeof(short));
    } 
    // "OR" threshold
    // show values <  'upper' OR  > 'lower'
    // zero values >= 'upper' AND <= 'lower'
    else 
    {
      // Zero out what's >= 'upper' and <= 'lower'
      memset(&(map[offset+upper]), 0, (lower-upper+1)*sizeof(short));
    }
  }
  this->BuildTime.Modified();
}

// get the color for a scalar value
void vtkIndirectLookupTable::GetColor(vtkFloatingPointType v, vtkFloatingPointType rgb[3])
{
  unsigned char *rgb8 = this->MapValue(v);

  rgb[0] = rgb8[0]/255.0;
  rgb[1] = rgb8[1]/255.0;
  rgb[2] = rgb8[2]/255.0;
}

// Given a scalar value v, return an RGBA color value from LookupTable
unsigned char *vtkIndirectLookupTable::MapValue(vtkFloatingPointType v)
{
  unsigned short *map = this->Map->GetPointer(0);
  unsigned char *lut = this->LookupTable->GetPointer(0);

  // Boundary check
  if (v < this->Range[0]) 
  {
    v = this->Range[0];
  }
  else if (v > this->Range[1])
  {
    v = this->Range[1];
  }

  // Map it
  return &lut[map[this->MapOffset + (long)v]];
}

// accelerate the mapping by copying the data in 32-bit chunks instead
// of 8-bit chunks
template<class T>
static void vtkIndirectLookupTableMapData(vtkIndirectLookupTable *self, 
  T *input, unsigned char *output,int length, int incr)
{
  int i;
  vtkFloatingPointType *range = self->GetRange();
  long offset = self->GetMapOffset();
  unsigned char *lut = self->GetLookupTable()->GetPointer(0);
  unsigned short *map = self->GetMap()->GetPointer(0);
  
  // DAVE do this safer
  //
  // Karl Krissian: pb with unsigned ...
  // range is -32768, 32767, can not be converted to unsigned short for example ...
  //
  T v;
  long min = (long)range[0];
  long max = (long)range[1];

  for (i=0; i<length; i++)
  {
    v = *input;
    
    // Boundary check
    if ((long)v < min) 
    {
      v = min;
    }
    else if ((long)v > max)
    {
      v = max;
    }
    long s = (long)v;

    // Map s to RGBA
    int index = offset + s;

    // For VTK_FLOAT or VTK_DOUBLE, we need handle the following case:
    // For instance, we set 2.3 for the lower threshold. All values less than 2.3, 
    // including 2.1 and 2.2, must be thresholded out. 2.1 and 2.2 are special cases
    // here since 2.1, 2.2 and 2.3 all become 2 if we cast them into long, int or short.
    // The following statment will guarantee the removal of 2.1 and 2.2.
    if (v < self->GetLowerThreshold())
    {
        index = offset;
    }

    memcpy(output, &lut[map[index]], 4);

    output += 4;
    input += incr;
  }
}

static void vtkIndirectLookupTableMapShortData(vtkIndirectLookupTable *self, 
  short *input, unsigned char *output, int length, int incr)
{
  int i;
  long offset = self->GetMapOffset();
  unsigned char *lut = self->GetLookupTable()->GetPointer(0);
  unsigned short *map = self->GetMap()->GetPointer(0);

  for (i=0; i<length; i++)
  {
    memcpy(output, &lut[map[offset + *input]], 4);
    output += 4;
    input += incr;
  }
}

void vtkIndirectLookupTable::MapScalarsThroughTable2(void *input, 
                        unsigned char *output,
                        int inputDataType, 
                        int numberOfValues,
                        int inputIncrement,
                        int outputFormat)
{

  switch (inputDataType)
    {
    case VTK_CHAR:
      vtkIndirectLookupTableMapData(this,(char *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_UNSIGNED_CHAR:
      vtkIndirectLookupTableMapData(this,(unsigned char *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_SHORT:
      vtkIndirectLookupTableMapShortData(this,(short *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_UNSIGNED_SHORT:
      vtkIndirectLookupTableMapData(this,(unsigned short *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_INT:
      vtkIndirectLookupTableMapData(this,(int *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_UNSIGNED_INT:
      vtkIndirectLookupTableMapData(this,(unsigned int *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_LONG:
      vtkIndirectLookupTableMapData(this,(long *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_UNSIGNED_LONG:
      vtkIndirectLookupTableMapData(this,(unsigned long *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_FLOAT:
      vtkIndirectLookupTableMapData(this,(float *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    case VTK_DOUBLE:
      vtkIndirectLookupTableMapData(this,(double *)input,
        output,numberOfValues,inputIncrement);
      break;
      
    default:
      vtkErrorMacro(<< "MapScalarsThroughTable2: Unknown input ScalarType");
      return;
    }
}  



