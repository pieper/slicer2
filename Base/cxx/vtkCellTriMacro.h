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
//
//BTX - begin tcl exclude
#define CELLTRIANGLES(CELLPTIDS, TYPE, IDX, PTID0, PTID1, PTID2) \
    { switch( TYPE ) \
      { \
      case VTK_TRIANGLE: \
      case VTK_POLYGON: \
      case VTK_QUAD: \
        PTID0 = CELLPTIDS[0]; \
        PTID1 = CELLPTIDS[(IDX)+1]; \
        PTID2 = CELLPTIDS[(IDX)+2]; \
        break; \
      case VTK_TRIANGLE_STRIP: \
        PTID0 = CELLPTIDS[IDX]; \
        PTID1 = CELLPTIDS[(IDX)+1+((IDX)&1)]; \
        PTID2 = CELLPTIDS[(IDX)+2-((IDX)&1)]; \
        break; \
      default: \
        PTID0 = PTID1 = PTID2 = -1; \
      } }
//ETX - end tcl exclude
//
