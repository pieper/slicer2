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
  this->Range[0] = (float)this->MapRange[0];
  this->Range[1] = (float)this->MapRange[1];
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
void vtkIndirectLookupTable::MapDirect(float scalar, int index)
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

  this->BuildTime.Modified();
}
  
// Given a scalar value v, return an RGBA color value from LookupTable
unsigned char *vtkIndirectLookupTable::MapValue(float v)
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
  T *input, unsigned char *output, int length, int incr)
{
  int i;
  float *range = self->GetRange();
  long offset = self->GetMapOffset();
  unsigned char *lut = self->GetLookupTable()->GetPointer(0);
  unsigned short *map = self->GetMap()->GetPointer(0);
  
  // DAVE do this safer
  T v;
  T min = (T)range[0];
  T max = (T)range[1];

  for (i=0; i<length; i++)
  {
    v = *input;
    
    // Boundary check
    if (v < min) 
    {
      v = min;
    }
    else if (v > max)
    {
      v = max;
    }
    long s = (long)v;

    // Map s to RGBA
    memcpy(output, &lut[map[offset + s]], 4);

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



