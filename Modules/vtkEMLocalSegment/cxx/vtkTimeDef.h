/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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
#ifndef __vtkTimeDef_h
#define __vtkTimeDef_h


#include <vtkEMLocalSegmentConfigure.h>
#include "vtkObject.h"

#ifndef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <errno.h>
#include <sys/time.h>

// ----------------------------------------------------------------------------------------------
// This file includes alll Data and time Structures  
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
// Time Stop Measure up to msec  
// ----------------------------------------------------------------------------------------------
// Do not forget to define in cxx
// extern "C" {
//  struct timeval preciseTimeEnd;
//  struct timeval preciseTimeStart;
// }
extern time_t timeToComputeTime; /* In seconds */
extern time_t timeCount; /* In seconds */
extern time_t timer_Start;
extern time_t timer_Stop;

extern struct timeval preciseTimeStart;
extern struct timeval preciseTimeEnd;
extern long milliSecondDifferences;

#define START_PRECISE_TIMING \
do { \
  gettimeofday(&preciseTimeStart, NULL); \
} while (0)

#define END_PRECISE_TIMING \
do { \
  gettimeofday(&preciseTimeEnd, NULL); \
} while (0)

 
#define SHOW_ELAPSED_PRECISE_TIME \
do { \
  double uend = 1e-06*(double)preciseTimeEnd.tv_usec; \
  double ustart = 1e-06*(double)preciseTimeStart.tv_usec; \
  double end = preciseTimeEnd.tv_sec + uend;\
  double start = preciseTimeStart.tv_sec + ustart;\
  printf("Elapsed time: %g\n", (end-start) ); \
} while(0)

#define START_TIMING \
do { \
  findTimeToComputeTime(); \
  timer_Start = time((time_t *)NULL); \
} while (0)

#define END_TIMING \
do { \
  timer_Stop = time((time_t *)NULL); \
  timeCount = timer_Stop - timer_Start - timeToComputeTime; \
} while(0)


#define SHOW_ELAPSED_TIME(fp) \
do { \
  fprintf(fp,"Elapsed time: %d\n",(int)timeCount); \
} while(0)


#endif // _WIN32




// ----------------------------------------------------------------------------------------------
// Dummy class 
// ----------------------------------------------------------------------------------------------/ 
class VTK_EMLOCALSEGMENT_EXPORT vtkTimeDef { 
public:
  static vtkTimeDef *New() {return (new vtkTimeDef);}
protected:

};

#endif
