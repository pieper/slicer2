/*=auto=========================================================================

(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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


#include "vtkTimeCoursePlotActor2.h"

#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPoints.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkLegendBoxActor.h"


vtkStandardNewMacro(vtkTimeCoursePlotActor2);


vtkTimeCoursePlotActor2::vtkTimeCoursePlotActor2()
{
    this->i = -1;
    this->j = -1;
    this->k = -1;
}


vtkTimeCoursePlotActor2::~vtkTimeCoursePlotActor2()
{
}


vtkDataSet* vtkTimeCoursePlotActor2::CreateDataSet(vtkFloatArray *yPoints, float *xPoints)
{

    vtkPoints *points = vtkPoints::New();
    float point[3];
    for (int i = 0; i < yPoints->GetNumberOfTuples(); i++)
    {
        point[0] = xPoints[i];
        point[1] = 0; 
        point[2] = 0; 
        points->InsertPoint(i, point);
    }

    vtkUnstructuredGrid *dataSet = vtkUnstructuredGrid::New();
    dataSet->GetPointData()->SetScalars(yPoints);
    dataSet->SetPoints(points);
    points->Delete();

    return dataSet;
}


void vtkTimeCoursePlotActor2::SetPlot(vtkFloatArray *timeCourse, vtkFloatArray *model, int waveform)
{
    // Creates x points
    float *xPoints = new float [timeCourse->GetNumberOfTuples()];
    for (int i = 0; i < timeCourse->GetNumberOfTuples(); i++)
    {
        xPoints[i] = i;
    }
    // Creates data set for time course  
    vtkDataSet* tcDataSet = CreateDataSet(timeCourse, xPoints);

    // Max/min values in the time course
    float max = 0.0;
    float min = 1000000.0;
    float *tcPtr = timeCourse->GetPointer(0); 
    for (int i = 0; i < timeCourse->GetNumberOfTuples(); i++)
    {
        if (tcPtr[i] < min)
        {
            min = tcPtr[i];
        }
        if (tcPtr[i] > max)
        {
            max = tcPtr[i];
        }
    }

    // Scales model for plotting 
    int size = model->GetNumberOfTuples();
    float *stimPtr = model->GetPointer(0);  
    float middle = (max + min) / 2;
    vtkFloatArray *tc = vtkFloatArray::New();
    tc->SetNumberOfTuples(size);
    tc->SetNumberOfComponents(1);
    if (waveform == 0) // square waveform 
    {
        float tmp;
        for (int i = 0; i < size; i++)
        {
            if (stimPtr[i] != 0.0)
            {
                tmp = (max + min) / 2;
            }
            else
            {
                tmp = min;
            }
            tc->SetComponent(i, 0, tmp);
        }
    }
    else if (waveform == 1) // sinusoid
    {
        for (int i = 0; i < size; i++)
        {
            float tmp = middle + (stimPtr[i] / 2) * (max - middle);
            tc->SetComponent(i, 0, tmp);
        }
    }

    // Creates data set for model 
    vtkDataSet* stimDataSet = CreateDataSet(tc, xPoints);
 
    delete [] xPoints;

    // Plotting properties
    char title[50];
    sprintf(title, 
            "Voxel Index (i: %d, j: %d, k: %d)",
            this->i, this->j, this->k);
    this->SetTitle(title);
    this->SetYTitle("Intensity");
    this->SetXTitle("Volume Number");
    this->GetProperty()->SetColor(0, 0, 0);

    this->RemoveAllInputs();
    this->AddInput(tcDataSet);
    this->AddInput(stimDataSet);
    this->SetPlotColor(0, 1.0, 0.0, 0.0);
    this->SetPlotColor(1, 0.0, 0.0, 1.0);
    this->GetPositionCoordinate()->SetValue(0.05, 0.1, 0); // start position
    this->GetPosition2Coordinate()->SetValue(0.9, 0.8, 0); //relative to position
    this->SetXValuesToValue();
    this->GetLegendBoxActor()->SetEntryString(0, "Signal");
    this->GetLegendBoxActor()->SetEntryString(1, "Model");
    this->LegendOn();
    this->SetYRange(min, max);
    this->SetNumberOfXLabels(13);
}


void vtkTimeCoursePlotActor2::SetVoxelIndex(int i, int j, int k)
{
    this->i = i;
    this->j = j;
    this->k = k;
}


