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
/*=============================================================================
Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
 
Direct all questions on this copyright to gering@ai.mit.edu.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files.   

The authors hereby grant permission to use, copy, and distribute this
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
=============================================================================*/
#include "vtkImageCCA.h"
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
vtkImageCCA* vtkImageCCA::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImageCCA");
  if(ret)
    {
    return (vtkImageCCA*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImageCCA;
}

//----------------------------------------------------------------------------
// Description:
// Constructor sets default values
vtkImageCCA::vtkImageCCA()
{
  this->Background = 0;
  this->MinForeground = VTK_SHORT_MIN;
  this->MaxForeground = VTK_SHORT_MAX;
  this->MinSize = 10000;
  this->function = FUNCTION_MEASURE;
  this->OutputLabel = 1;
  this->LargestIslandSize = this->IslandSize = 0;
  this->Seed[0] = this->Seed[1] = this->Seed[2] = 0;
}

char* vtkImageCCA::GetFunctionString()
{
  switch (this->function) 
  {
    case FUNCTION_REMOVE:
      return "RemoveIslands";
    case FUNCTION_CHANGE:
      return "ChangeIsland";
    case FUNCTION_MEASURE:
      return "MeasureIsland";
    case FUNCTION_SAVE:
      return "SaveIsland";
   default:
      return "ERROR: Unknown";
  }
}

/*************************************************************************
The following procedures were written by Andre Robatino on November 1999

connect
recursive_copy
*************************************************************************/

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

/*************************************************************************
The following procedures were written by Andre Robatino on 8/5/97
and modified only slightly by Dave Gering on 5/29/99:

connect
recursive_copy_from
recursive_copy_to
*************************************************************************/
/*

int connect(int, int *, short *, int *, int *);
static void recursive_copy_from(int, int);
static void recursive_copy_to(int, int);

static int *g_axis_len, *g_outimage_ptr, *g_pad_outimage_ptr;
static short *g_inimage_ptr;

int connect(
     int rank,
     int *axis_len,
     short *inimage,
     int *outimage,
     int *num_components)
{
  int i;
  int data_len, *pad_outimage, **path_image, **path_stride;
  int *pad_outimage_ptr, *pad_outimage_end, *imagep, *stride, stridev, label, **path_image_ptr, *path_image_ptrv, **path_stride_ptr, *path_stride_ptrv;

  // Check rank is 2D or 3D
  if (rank <= 0) {
    printf("Rank must be positive\n");
    return -1;
  }

  // Allocate stride
  stride = new int[2*rank+1];

  data_len = stride[0] = 1;

  for (i=0; i<rank; i++) {
    // Check axis len > 0
    if (axis_len[i] <= 0) {
      printf("axis len not positive\n");
      return -1;
    }
    data_len *= axis_len[i];
    stride[i+1] = stride[i]*(axis_len[i] + 2);
  }
  
  // Allocate
  pad_outimage = new int[stride[rank]];
  path_image   = new int*[data_len];
  path_stride  = new int*[data_len];
  
  g_axis_len = axis_len;
  g_inimage_ptr = inimage;
  g_pad_outimage_ptr = pad_outimage;
  
  recursive_copy_from(rank-1, 1);

  label = 0;
  path_image_ptr = path_image;
  path_stride_ptr = path_stride;
  pad_outimage_ptr = pad_outimage;
  pad_outimage_end = pad_outimage + stride[rank];
 
  for (i=0; i<rank; i++) {
    stride[i+rank] = -stride[i];
  }
  stride[2*rank] = 0;

  do {
    if (*pad_outimage_ptr) 
      continue;
    
    *(path_image_ptrv = pad_outimage_ptr) = ++label;
    path_stride_ptrv = stride;
   
    while (1) 
    {
      while (stridev = *path_stride_ptrv++) 
      {
        if (*(imagep = path_image_ptrv + stridev)) 
          continue;

        *path_image_ptr++ = path_image_ptrv;
        *(path_image_ptrv = imagep) = label;
        *path_stride_ptr++ = path_stride_ptrv;
        path_stride_ptrv = stride;
      }

      if (path_image_ptr == path_image) 
        break;
      
      path_image_ptrv = *--path_image_ptr;
      path_stride_ptrv = *--path_stride_ptr;
    }
  } while (++pad_outimage_ptr < pad_outimage_end);

  if (num_components != NULL) 
    *num_components = label;

  // Cleanup

  delete [] stride;
  delete [] path_image;
  delete [] path_stride;

  g_outimage_ptr = outimage;
  g_pad_outimage_ptr = pad_outimage;

  recursive_copy_to(rank-1, 1);

  delete [] pad_outimage;
  return 0;
}

static void recursive_copy_from(
     int axis,
     int flag)
{
  int i, axis_len;

  axis_len = g_axis_len[axis];

  if (axis) {
    recursive_copy_from(axis-1, 0);

    for (i=0; i<axis_len; i++) {
      recursive_copy_from(axis-1, flag);
    }
    recursive_copy_from(axis-1, 0);
  } else {
    int *pad_outimage_ptr, *pad_outimage_end;
    short *inimage_ptr;

    inimage_ptr = g_inimage_ptr;
    pad_outimage_ptr = g_pad_outimage_ptr;
    *pad_outimage_ptr++ = -1;
    pad_outimage_end = pad_outimage_ptr + axis_len;
   
    if (flag) {
      while (pad_outimage_ptr < pad_outimage_end) {
        *pad_outimage_ptr++ = (*inimage_ptr++)? 0 : -1;
      }
    } else {
      while (pad_outimage_ptr < pad_outimage_end) {
        *pad_outimage_ptr++ = -1;
      }
    }
    *pad_outimage_ptr++ = -1;
    g_pad_outimage_ptr = pad_outimage_ptr;
    g_inimage_ptr = inimage_ptr;
  }
  return;
}

static void recursive_copy_to(
     int axis,
     int flag)
{
  int i, axis_len;
 
  axis_len = g_axis_len[axis];

  if (axis) {
    recursive_copy_to(axis-1, 0);

    for (i=0; i<axis_len; i++) {
      recursive_copy_to(axis-1, flag);
    }
    recursive_copy_to(axis-1, 0);
  } else {
    int *outimage_ptr, *pad_outimage_ptr, *pad_outimage_end;

    outimage_ptr = g_outimage_ptr;
    pad_outimage_ptr = g_pad_outimage_ptr;
    pad_outimage_ptr++;
 
    if (flag) {
      pad_outimage_end = pad_outimage_ptr + axis_len;

      while (pad_outimage_ptr < pad_outimage_end) {
        *outimage_ptr++ = *pad_outimage_ptr++;
      }
    } else {
      pad_outimage_ptr += axis_len;
    }
    pad_outimage_ptr++;
    g_pad_outimage_ptr = pad_outimage_ptr;
    g_outimage_ptr = outimage_ptr;
  }
  return;
}
*/
//----------------------------------------------------------------------------
// End Andre's cool routines.
//----------------------------------------------------------------------------


static void vtkImageCCAExecute(vtkImageCCA *self,
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
  short seedLabel, maxLabel;
  int largest, len=1, j;
  int *census = NULL;
  int seed[3];
  int minSize = self->GetMinSize();
  short pix;
  int SaveIsland    = self->GetFunction() == FUNCTION_SAVE;
  int MeasureIsland = self->GetFunction() == FUNCTION_MEASURE;
  int ChangeIsland  = self->GetFunction() == FUNCTION_CHANGE;
  int RemoveIslands = self->GetFunction() == FUNCTION_REMOVE;

  size_t conSeedLabel, i, idx;
  // connect
  int rank;
  size_t *axis_len=NULL;
  short bg = self->GetBackground();
  short bgMask = 0;
  short fgMask = 1;
  char inbackground = (char)bgMask;
  size_t numIslands=0;
  char *conInput=NULL;
  size_t *conOutput=NULL;

  // Image bounds
  outMin0 = outExt[0];   outMax0 = outExt[1];
  outMin1 = outExt[2];   outMax1 = outExt[3];
  outMin2 = outExt[4];   outMax2 = outExt[5];

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
  // Remove:
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

  if (RemoveIslands)
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
  // Save, Change, Measure, Remove
  // -----------------------------
  // Run Connectivity
  // 
  ///////////////////////////////////////////////////////////////

  if (SaveIsland || ChangeIsland || MeasureIsland || RemoveIslands)
  {
    connect(rank, axis_len, conInput, inbackground, conOutput, &numIslands);
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
    census = new int[numIslands+1];
    memset(census, 0, (numIslands+1)*sizeof(int));

	  i = 0;
    for (outIdx2 = outMin2; outIdx2 <= outMax2; outIdx2++) {
    for (outIdx1 = outMin1; outIdx1 <= outMax1; outIdx1++) {
    for (outIdx0 = outMin0; outIdx0 <= outMax0; outIdx0++)
    {
      idx = conOutput[i];
      if (idx >= 0 && idx <= numIslands)
      {
        census[idx] = census[idx] +1;
      }
      i++;
    }//for0
    }//for1
    }//for2
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
    for (i=0; i<=numIslands; i++)
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
  // Remove
  // -----------------------------
  // Output gets input except where islands too small
  //
  //   outData[i] = inData[i],  census[conOutput[i]] >= minIslandSize
  //              = bg,    else
  //
  // Output gets input where the input was thresholded away
  //
  //   outData[i] = inData[i],  inData[i] outside [min,max]
  //              = do nothing, else
  //
  ///////////////////////////////////////////////////////////////

  if (RemoveIslands)
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
  delete [] conInput;
  delete [] conOutput;
}


//----------------------------------------------------------------------------
// Description:
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageCCA::Execute(vtkImageData *inData, vtkImageData *outData)
{
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

  vtkImageCCAExecute(this, inData, (short *)inPtr, 
          outData, (short *)(outPtr), outExt);
}

void vtkImageCCA::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageFilter::PrintSelf(os,indent);
  
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
