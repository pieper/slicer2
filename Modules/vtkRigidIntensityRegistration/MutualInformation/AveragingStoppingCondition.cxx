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

#include "AveragingStoppingCondition.h"
#include "vtkSystemIncludes.h"

#define BIG_NEGATIVE_NUMBER VTK_DOUBLE_MIN

/* ====================================================================== */

AveragingStoppingCondition::AveragingStoppingCondition()
{
  NumberOfIterSinceMax = 0;
  Max_Value   = BIG_NEGATIVE_NUMBER;
  Average_Value = 0.0;
  NumSampleAverage = 10;            // average over 10 samples;
  MaxAllowedItersOfShrinking = 10;  // How many iters until stop?
}

/* ====================================================================== */

double AveragingStoppingCondition::GetAverageValue() const
{
  return (Average_Value/(double)NumSampleAverage);
}

/* ====================================================================== */

void AveragingStoppingCondition::UpdateAverageValue(const double &newvalue,
                                           const double &oldvalue)
{
  Average_Value+= newvalue-oldvalue;
}

/* ====================================================================== */

int AveragingStoppingCondition::CheckStopping(const double &value)
{
  if (value > Max_Value)    // A new maximum?
    {
      Max_Value  = value;
      NumberOfIterSinceMax = 0;
    }
  else
    {
      if (queue.size() >= NumSampleAverage)
        {
          double old_average = this->GetAverageValue();
          if (old_average < value)   // Maybe the average is going up?
            {
              NumberOfIterSinceMax = 0;  
            }
          else   // Neither the average nor the max is going up
            {
              if (NumberOfIterSinceMax >= MaxAllowedItersOfShrinking)
                return 0;
            }
        }
    }

  // Update the average
  if (queue.size() >= NumSampleAverage)
    this->UpdateAverageValue(value,queue.front());
  else
    this->UpdateAverageValue(value,0.0);
  // Update the list
  queue.pop();
  queue.push(value);
  return 1;
}
