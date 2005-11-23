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
/*==============================================================================
(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
==============================================================================*/


#include "vtkActivationFalseDiscoveryRate.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"

#include <vtkstd/algorithm>
#include <gsl/gsl_cdf.h>

vtkStandardNewMacro(vtkActivationFalseDiscoveryRate);


vtkActivationFalseDiscoveryRate::vtkActivationFalseDiscoveryRate()
{
    this->FDRThreshold = 0; 
    this->DOF = 0;
    this->Option = 1;
}


vtkActivationFalseDiscoveryRate::~vtkActivationFalseDiscoveryRate()
{
}


void vtkActivationFalseDiscoveryRate::SimpleExecute(vtkImageData *input, vtkImageData* output)
{
    if (this->NumberOfInputs == 0)
    {
        vtkErrorMacro( << "This filter needs one input of image data.");
        return;
    }

    // we don't use output image
    output = NULL;

    // get the data array from input image (t map)
    vtkFloatArray *tArray = (vtkFloatArray *)input->GetPointData()->GetScalars();
    vtkFloatArray *tmp = vtkFloatArray::New();
    tmp->DeepCopy(tArray);

    // convert t -> p
    int size = tmp->GetNumberOfTuples();
    float *data = (float *)tmp->GetPointer(0);
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        float t = data[i];
        if (t != 0)
        {
            double p = gsl_cdf_tdist_Q(t, this->DOF);
            // double sided tail probability for t-distribution
            p *= 2;

            data[count++] = (float) p;
        }
    }

    // use vtkstd::sort sort p values from min to max
    float *ps = new float [count];
    memcpy(ps, data, count);
    vtkstd::sort(ps, ps + count);

    // compute c(N) according to option 
    float cn = 1.0;
    float l = 0.0;
    if (this->Option == 2)
    {
        for (int i = 0; i < count; i++)
        {
            l += 1.0 / (i+1); 
        }
        cn = l;
    }

    // get the max p(i), where
    // p(i) <= i * q / (N * c(N))
    float pc = 0.0;
    for (int i = 0; i < count; i++)
    {
        float v = (i+1) * this->Q / count / cn;
        if (ps[i] == v)
        {
            pc = ps[i];
            break;
        }
        else if (ps[i] > v)
        {
            pc = ps[i-1];
            break;
        }
    }

    // t threshold
    this->FDRThreshold = (float)gsl_cdf_tdist_Qinv((pc / 2), this->DOF);

    tmp->Delete();
    delete [] ps;
}

