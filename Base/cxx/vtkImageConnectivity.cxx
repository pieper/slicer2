/*=auto=========================================================================

(c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for internal 
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
#include "vtkImageConnectivity.h"
#include "vtkObjectFactory.h"
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>


//------------------------------------------------------------------------------
vtkImageConnectivity* vtkImageConnectivity::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageConnectivity");
  if(ret)
    {
    return (vtkImageConnectivity*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageConnectivity;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageConnectivity::vtkImageConnectivity()
{
  this->Background = 0;
  this->MinForeground = VTK_SHORT_MIN;
  this->MaxForeground = VTK_SHORT_MAX;
  this->MinSize = 10000;
  this->function = CONNECTIVITY_MEASURE;
  this->OutputLabel = 1;
  this->SliceBySlice = 0;
  this->LargestIslandSize = this->IslandSize = 0;
  this->Seed[0] = this->Seed[1] = this->Seed[2] = 0;
}

char* vtkImageConnectivity::GetFunctionString()
{
  switch (this->function) 
  {
    case CONNECTIVITY_IDENTIFY:
      return "IdentifyIslands";
    case CONNECTIVITY_REMOVE:
      return "RemoveIslands";
    case CONNECTIVITY_CHANGE:
      return "ChangeIsland";
    case CONNECTIVITY_MEASURE:
      return "MeasureIsland";
    case CONNECTIVITY_SAVE:
      return "SaveIsland";
   default:
      return "ERROR: Unknown";
  }
}

//************************************************************************
// The following procedures were written by Andre Robatino 
// in November, 1999
//
// connect
// recursive_copy
//************************************************************************

int connect(int, size_t *, char *, char, size_t *, size_t *);
static void recursive_copy(int, size_t);

static size_t *g_axis_len, *g_outimagep, num_stride_index_bits;
static char *g_inimagep, g_inbackground;

int connect(
     int rank,
     size_t *axis_len,
     char *inimage,
     char inbackground,
     size_t *outimage,
     size_t *num_components) /* set to NULL if not interested */
{
  int i;
  register unsigned int axisv;
  size_t data_len;
  register size_t label, *outimagep, *outimage_end, *imagep, *new_imagep, boundary_mask_start, component_mask, axis_mask;
  register ptrdiff_t *stride, stridev;

  assert(rank >= 0);
  if (rank == 0) {
    *outimage = (*inimage != inbackground);
    if (num_components) *num_components = *outimage;
    return 0;
  }
  label = 2*rank;
  num_stride_index_bits = 1;
  while (label >>= 1) num_stride_index_bits++;
  assert(num_stride_index_bits + 2*rank + 1 <= CHAR_BIT*sizeof(size_t));
  boundary_mask_start = 1<<num_stride_index_bits;
  axis_mask = boundary_mask_start - 1;
  stride = (ptrdiff_t *)malloc((2*rank + 1)*sizeof(ptrdiff_t));
  assert(stride);
  data_len = 1;
  for (i=0; i<rank; i++) {
    assert(axis_len[i] > 1);
    stride[2*i + 1] = -(stride[2*i] = data_len);
    data_len *= axis_len[i];
  }
  stride[2*rank] = 0;
  g_axis_len = axis_len;
  g_inimagep = inimage;
  g_inbackground = inbackground;
  g_outimagep = outimage;
  component_mask = 0;
  component_mask = ~(~component_mask>>1);
  recursive_copy(rank-1, component_mask);
  outimagep = outimage;
  outimage_end = outimage + data_len;
  label = 0;
  do {
    if (!(*outimagep & component_mask)) continue;
    imagep = outimagep;
    *imagep ^= component_mask;
    axisv = 0;
    label++;
    while (1) {
      while (stridev = stride[axisv]) {
        if ((*imagep & boundary_mask_start<<axisv) && (*(new_imagep = imagep + stridev) & component_mask)) {
          imagep = new_imagep;
          *imagep ^= component_mask;
          *imagep |= axisv;
          axisv = 0;
          continue;
        }
        axisv++;
      }
      if (imagep == outimagep) break;
      axisv = *imagep & axis_mask;
      *imagep = label;
      imagep -= stride[axisv++];
    }
    *outimagep = label;
  } while (++outimagep < outimage_end);
  if (num_components) *num_components = label;
  free(stride);
  return 0;
}

static void recursive_copy(
     int axis,
     register size_t mask)
{
  register size_t len = g_axis_len[axis] - 2;

  if (axis == 0) {
    register size_t *outimagep;
    register char *inimagep, inbackground;

    inimagep = g_inimagep;
    inbackground = g_inbackground;
    outimagep = g_outimagep;
    mask |= 1<<num_stride_index_bits;
    *outimagep++ = (*inimagep++ == inbackground)? 0 : mask;
    mask |= 2<<num_stride_index_bits;
    while (len--) *outimagep++ = (*inimagep++ == inbackground)? 0 : mask;
    mask ^= 1<<num_stride_index_bits;
    *outimagep++ = (*inimagep++ == inbackground)? 0 : mask;
    g_outimagep = outimagep;
    g_inimagep = inimagep;
  } else {
    mask |= 1<<(num_stride_index_bits + 2*axis);
    recursive_copy(axis-1, mask);
    mask |= 2<<(num_stride_index_bits + 2*axis);
    while (len--) recursive_copy(axis-1, mask);
    mask ^= 1<<(num_stride_index_bits + 2*axis);
    recursive_copy(axis-1, mask);
  }
  return;
}

//***********************************************************************
// End Andre's cool routines.
//************************************************************************


static void vtkImageConnectivityExecute(vtkImageConnectivity *self,
                     vtkImageData *inData, short *inPtr,
                     vtkImageData *outData, short *outPtr, 
                     int outExt[6])
{
  // For looping though output (and input) pixels.
  int outMin0, outMax0, outMin1, outMax1, outMin2, outMax2;
  int outIdx0, outIdx1, outIdx2;
  int inInc0, inInc1, inInc2;
  int outInc0, outInc1, outInc2;
  short *inPtr0, *outPtr0, *outPtr1;
  short minForegnd = (short)self->GetMinForeground();
  short maxForegnd = (short)self->GetMaxForeground();
  short newLabel = (short)self->GetOutputLabel();
  short seedLabel;
  int largest, len=1, nxy, z, nz, j;
  int *census = NULL;
  int seed[3];
  int minSize = self->GetMinSize();
  short pix;
  int IdentifyIslands = self->GetFunction() == CONNECTIVITY_IDENTIFY;
  int RemoveIslands   = self->GetFunction() == CONNECTIVITY_REMOVE;
  int ChangeIsland    = self->GetFunction() == CONNECTIVITY_CHANGE;
  int SaveIsland      = self->GetFunction() == CONNECTIVITY_SAVE;
  int MeasureIsland   = self->GetFunction() == CONNECTIVITY_MEASURE;
  int SliceBySlice    = self->GetSliceBySlice();

  // connect
  size_t conSeedLabel, i, idx, dz;
  int rank;
  size_t *axis_len=NULL;
  short bg = self->GetBackground();
  short bgMask = 0;
  short fgMask = 1;
  char inbackground = (char)bgMask;
  char *conInput=NULL;
  size_t *conOutput=NULL;
  size_t *numIslands=NULL;

  // Image bounds
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];

  // Computer Parameters for connect().
  rank = (outExt[5]==outExt[4]) ? 2 : 3;
  axis_len = new size_t[rank+1];
  axis_len[0] = outExt[1]-outExt[0]+1;
  axis_len[1] = outExt[3]-outExt[2]+1;
  axis_len[2] = outExt[5]-outExt[4]+1;
  for (j=0; j<rank; j++)
  {
    len *= axis_len[j];
  }
  conInput = new char[len];
  conOutput = new size_t[len];
  numIslands = new size_t[axis_len[2]];

  // Get increments to march through data continuously
  outData->GetContinuousIncrements(outExt, outInc0, outInc1, outInc2);
  inData->GetContinuousIncrements(outExt, inInc0, inInc1, inInc2);

  ///////////////////////////////////////////////////////////////
  // Save, Change, Measure:
  // ----------------------
  // Get the seed
  // 
  //   seedLabel = inData[xSeed,ySeed,zSeed]
  //
  // If the seed is out of bounds, return the input
  //
  //   outData[i] = inData[i]
  //
  ///////////////////////////////////////////////////////////////

  if (ChangeIsland || MeasureIsland || SaveIsland)
  {
    self->GetSeed(seed);
  
    if (seed[0] < outMin0 || seed[0] > outMax0 ||
        seed[1] < outMin1 || seed[1] > outMax1 ||
        seed[2] < outMin2 || seed[2] > outMax2)
    {
      //
      // Out of bounds -- abort!
      //
      inPtr0 = inPtr;
      outPtr0 = outPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
      for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
      {
        *outPtr0 = *inPtr0;
        outPtr0++;
        inPtr0++;
      }//for0
      outPtr0 += outInc1;
      inPtr0 += inInc1;
      }//for1
      outPtr0 += outInc2;
      inPtr0 += inInc2;
      }//for2

      fprintf(stderr, "Seed %d,%d,%d out of bounds in CCA.\n",
          seed[0], seed[1], seed[2]);
      return;
    }
  
    //
      // In bounds!
      //
      outPtr1 = (short*)inData->GetScalarPointer(seed[0], seed[1], seed[2]);
    seedLabel = *outPtr1;
  }

  ///////////////////////////////////////////////////////////////
  // Remove, Identify:
  // ----------------------
  // Create a mask where everything outside [min,max] or in the
  // sea (bg) is in the background.
  //
  //   conInput[i] = fgMask,   inData[i] != bg
  //               = bgMask,   else
  //
  //   conInput[i] = unchanged
  //               = bgMask,   where inData[i] not on [min,max]
  //
  ///////////////////////////////////////////////////////////////

  if (RemoveIslands || IdentifyIslands)
  {
      inPtr0 = inPtr;
      i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
      {
      if (*inPtr0 != bg)
      {
        conInput[i] = fgMask;
      }
          else
      {
            conInput[i] = bgMask;
      }
      inPtr0++;
          i++;
      }//for0
    inPtr0 += inInc1;
      }//for1
    inPtr0 += inInc2;
      }//for2

    // Optionally threshold [min,max]
    if(minForegnd > VTK_SHORT_MIN || maxForegnd < VTK_SHORT_MAX)
      {
      inPtr0 = inPtr;
        i=0;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
      for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
        pix = *inPtr0;
        if (pix < minForegnd || pix > maxForegnd)
        {
          conInput[i] = bgMask;
        }
        i++;
        inPtr0++;
        }//for0
      inPtr0 += inInc1;
        }//for1
      inPtr0 += inInc2;
        }//for2
      }
  }


  ///////////////////////////////////////////////////////////////
  // Save, Change, Measure:
  // ----------------------
  //
  // Create a mask where everything not equal to seedLabel is 
  // in the background.
  //
  //     conInput[i] = fgMask,  inData[i] == seedLabel
  //                 = bgMask,  else
  //
  ///////////////////////////////////////////////////////////////

  if (SaveIsland || ChangeIsland || MeasureIsland)
  {
      inPtr0 = inPtr;
      i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
      {
      if (*inPtr0 == seedLabel)
      {
        conInput[i] = fgMask;
      }
          else
      {
            conInput[i] = bgMask;
      }
      inPtr0++;
          i++;
      }//for0
    inPtr0 += inInc1;
      }//for1
    inPtr0 += inInc2;
      }//for2
  }


  ///////////////////////////////////////////////////////////////
  // Save, Change, Measure, Remove, Identify
  // ---------------------------------------
  // Run Connectivity
  // 
  ///////////////////////////////////////////////////////////////

  if (SaveIsland || ChangeIsland || MeasureIsland || RemoveIslands || IdentifyIslands)
  {
    nz = 1;
    if (SliceBySlice && RemoveIslands)
    {
      // If SliceBySlice, then call connect() for each slice
      nxy = axis_len[0] * axis_len[1];
      nz = axis_len[2];
      rank = 2;
      int axis_len2 = axis_len[2];
      axis_len[2] = 1;

      for (z=0; z < nz; z++)
      {
        connect(rank, axis_len, &conInput[nxy*z], inbackground, 
          &conOutput[nxy*z], &numIslands[z]);
      }
      axis_len[2] = axis_len2;
    }
    else
    {
      connect(rank, axis_len, conInput, inbackground, conOutput, &numIslands[0]);
    }
  }


  ///////////////////////////////////////////////////////////////
  // Save, Change, Measure
  // -----------------------------
  // Get conSeedLabel
  //
  //   conSeedLabel = conOutput[xSeed,ySeed,zSeed]
  //
  ///////////////////////////////////////////////////////////////

  if (SaveIsland || ChangeIsland || MeasureIsland)
  {
      i = seed[2]*axis_len[1]*axis_len[0] + seed[1]*axis_len[0] + seed[0];
    conSeedLabel = conOutput[i];
  }


  ///////////////////////////////////////////////////////////////
  // Measure, Remove
  // -----------------------------
  // Count size of each island in conOutput
  //
  //   census[c] = COUNT(conOutput[c]),  forall c on [0,numIslands]
  //
  ///////////////////////////////////////////////////////////////

  if (RemoveIslands || MeasureIsland)
  {
    // For each label value, count the number of pixels with that label
    // If SliceBySlice, then work on each slice one at a time
    len = 0;
    for (z=0; z<nz; z++)
    {
      len += numIslands[z] + 1;
    }
    census = new int[len];
    memset(census, 0, len*sizeof(int));

    if (nz == 1)
    {
        i = 0;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
      for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
      {
        idx = conOutput[i];
        if (idx >= 0 && idx <= numIslands[0])
        {
          census[idx] = census[idx] + 1;
        }
        i++;
      }//for0
      }//for1
      }//for2
    } 
    else 
    {
      dz = 0;
        i = 0;
      for (z=0; z < nz; z++)
      {
        for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
        for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          idx = conOutput[i];
          if (idx >= 0 && idx <= numIslands[z])
          {
            census[dz+idx] = census[dz+idx] + 1;
          }
          i++;
        }//for0
        }//for1

        dz += numIslands[z]+1;
      }//forz
    }
  }


  ///////////////////////////////////////////////////////////////
  // Remove
  // -----------------------------
  // Output gets input except where islands too small
  //
  //   outData[i] = inData[i],  census[conOutput[i]] >= minIslandSize
  //              = bg,    else
  //
  ///////////////////////////////////////////////////////////////

  if (RemoveIslands)
  {
    if (nz == 1)
    {
      inPtr0 = inPtr;
      outPtr0 = outPtr;
        i = 0;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
      for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
      {
        if (census[conOutput[i]] >= minSize)
        {
          *outPtr0 = *inPtr0;
        }
        else
        {
          *outPtr0 = bg;
        }
          i++;
        outPtr0++;
        inPtr0++;
      }//for0
      outPtr0 += outInc1;
      inPtr0 += inInc1;
      }//for1
      outPtr0 += outInc2;
      inPtr0 += inInc2;
      }//for2
    }
    else 
    {
      dz = 0;
        i = 0;
      inPtr0 = inPtr;
      outPtr0 = outPtr;
      for (z=0; z < nz; z++)
      {
        for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
        for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          if (census[dz+conOutput[i]] >= minSize)
          {
           *outPtr0 = *inPtr0;
          }
          else
          {
            *outPtr0 = bg;
          }
            i++;
          outPtr0++;
          inPtr0++;
        }//for0
        outPtr0 += outInc1;
        inPtr0 += inInc1;
        }//for1
        outPtr0 += outInc2;
        inPtr0 += inInc2;

        dz += numIslands[z] + 1;
      }//z
    }//else
  }


  ///////////////////////////////////////////////////////////////
  // Measure
  // -----------------------------
  // Store statistics, and return output = input.
  //
  //   islandSize = census[conSeedLabel]
  //   largest    = MAX(census[c])
  //   outData[i] = inData[i]
  //
  ///////////////////////////////////////////////////////////////

  if (MeasureIsland)
  {
    // Find largest island
    largest = 0;
    for (i=0; i<=numIslands[0]; i++)
    {
      if (i != bg)
      {
        if (census[i] > largest)
        {
          largest = census[i];
        }
      }
    }
      self->SetLargestIslandSize(largest);

    // Measure island at seed
    self->SetIslandSize(census[conSeedLabel]);

    // Return output values to be the inputs
    inPtr0 = inPtr;
    outPtr0 = outPtr;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
    {
      *outPtr0 = *inPtr0;
      outPtr0++;
      inPtr0++;
    }//for0
    outPtr0 += outInc1;
    inPtr0 += inInc1;
    }//for1
    outPtr0 += outInc2;
    inPtr0 += inInc2;
    }//for2
  }

  
  ///////////////////////////////////////////////////////////////
  // Identify
  // -----------------------------
  // Output gets the output of connect()
  //
  //   outData[i] = conOutput[i]
  //
  ///////////////////////////////////////////////////////////////

  if (IdentifyIslands)
  {
    outPtr0 = outPtr;
      i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
    {
      *outPtr0 = (short)conOutput[i];
        i++;
      outPtr0++;
    }//for0
    outPtr0 += outInc1;
    }//for1
    outPtr0 += outInc2;
    }//for2
  }

  ///////////////////////////////////////////////////////////////
  // Remove, Identify
  // -----------------------------
  // Output gets input where the input was thresholded away
  //
  //   outData[i] = inData[i],  inData[i] outside [min,max]
  //              = do nothing, else
  //
  ///////////////////////////////////////////////////////////////

  if (RemoveIslands || IdentifyIslands)
  {
      if(minForegnd > VTK_SHORT_MIN || maxForegnd < VTK_SHORT_MAX)
      {
      inPtr0 = inPtr;
      outPtr0 = outPtr;
      for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
      for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
      for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
        {
          pix = *inPtr0;
      if (pix < minForegnd || pix > maxForegnd)
      {
          *outPtr0 = pix;
      }
      inPtr0++;
      outPtr0++;
        }//for0
      inPtr0 += inInc1;
      outPtr0 += outInc1;
        }//for1
      inPtr0 += inInc2;
      outPtr0 += outInc2;
        }//for2
    }
  }

  if (RemoveIslands || MeasureIsland)
  {
      delete [] census;
  }


  ///////////////////////////////////////////////////////////////
  // Save
  // -----------------------------
  // Output gets input where seedLabel, else bg
  //
  //   outData[i] = inData[i],  conOutput[i] == conSeedLabel
  //              = bg,         else
  //
  ///////////////////////////////////////////////////////////////

  if (SaveIsland)
  {
    inPtr0 = inPtr;
    outPtr0 = outPtr;
      i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
    {
      if (conOutput[i] == conSeedLabel) 
      {
        *outPtr0 = *inPtr0;
        }        
      else 
      {
        *outPtr0 = bg;
         }
        i++;
      outPtr0++;
      inPtr0++;
    }//for0
    outPtr0 += outInc1;
    inPtr0 += inInc1;
    }//for1
    outPtr0 += outInc2;
    inPtr0 += inInc2;
    }//for2
  }


  ///////////////////////////////////////////////////////////////
  // Change
  // -----------------------------
  // Output gets newLabel where seedLabel, else input
  //
  //   outData[i] = newLabel,   conOutput[i] == conSeedLabel
  //              = inData[i],  else
  //
  ///////////////////////////////////////////////////////////////

  if (ChangeIsland)
  {
    inPtr0 = inPtr;
    outPtr0 = outPtr;
      i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
    {
      if (conOutput[i] == conSeedLabel)
      {
        *outPtr0 = newLabel;
      }
      else
      {
        *outPtr0 = *inPtr0;
      }
        i++;
      outPtr0++;
      inPtr0++;
    }//for0
    outPtr0 += outInc1;
    inPtr0 += inInc1;
    }//for1
    outPtr0 += outInc2;
    inPtr0 += inInc2;
    }//for2
  }

  ///////////////////////////////////////////////////////////////
  // Cleanup
  ///////////////////////////////////////////////////////////////

  delete [] axis_len;
  delete [] numIslands;
  delete [] conInput;
  delete [] conOutput;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageConnectivity::ExecuteData(vtkDataObject *)
{
  vtkImageData *inData = this->GetInput();
  vtkImageData *outData = this->GetOutput();
  outData->SetExtent(outData->GetWholeExtent());
  outData->AllocateScalars();

  int outExt[6], id=0, s;
  outData->GetWholeExtent(outExt);
  void *inPtr = inData->GetScalarPointerForExtent(outExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  int x1;

  x1 = GetInput()->GetNumberOfScalarComponents();
  if (x1 != 1) 
  {
    vtkErrorMacro(<<"Input has "<<x1<<" instead of 1 scalar component.");
    return;
  }

  /* Need short data */
  s = inData->GetScalarType();
  if (s != VTK_SHORT) 
  {
    vtkErrorMacro("Input scalars are type "<<s 
      << " instead of "<<VTK_SHORT);
    return;
  }

  vtkImageConnectivityExecute(this, inData, (short *)inPtr, 
          outData, (short *)(outPtr), outExt);
}

void vtkImageConnectivity::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);
  
  os << indent << "Background:        " << this->Background << "\n";
  os << indent << "MinForeground:     " << this->MinForeground << "\n";
  os << indent << "MaxForeground:     " << this->MaxForeground << "\n"; 
  os << indent << "LargestIslandSize: " << this->LargestIslandSize << "\n"; 
  os << indent << "IslandSize:        " << this->IslandSize << "\n";
  os << indent << "MinSize:           " << this->MinSize << "\n";
  os << indent << "OutputLabel:       " << this->OutputLabel << "\n";
  os << indent << "Seed[0]:           " << this->Seed[0] << "\n";
  os << indent << "Seed[1]:           " << this->Seed[1] << "\n";
  os << indent << "Seed[2]:           " << this->Seed[2] << "\n";
  os << indent << "function:          " << this->function << "\n";
}
