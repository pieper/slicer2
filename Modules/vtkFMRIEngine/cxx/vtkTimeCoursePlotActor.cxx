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
/*==============================================================================
(c) Copyright 2004 Massachusetts Institute of Technology (MIT) All Rights Reserved.

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

#include "vtkTimeCoursePlotActor.h"

#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPoints.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkLegendBoxActor.h"
#include "vtkSphereSource.h"


vtkStandardNewMacro(vtkTimeCoursePlotActor);


vtkTimeCoursePlotActor::vtkTimeCoursePlotActor()
{
    this->i = -1;
    this->j = -1;
    this->k = -1;

    this->Model = NULL; 
    this->TimeCourse = NULL;

    this->ConditionMaxTimeCourse = NULL;
    this->ConditionMinTimeCourse = NULL;
    this->ConditionAverageTimeCourse = NULL;

    this->BaselineMaxTimeCourse = NULL;
    this->BaselineMinTimeCourse = NULL;
    this->BaselineAverageTimeCourse = NULL;
}


void vtkTimeCoursePlotActor::SetModel(vtkFloatArray *model)
{
    if (model == NULL)
    {
        cout << "vtkTimeCoursePlotActor::SetModel: Parameter model is a null pointer!!!" 
             << endl;
        return;
    }

    if (this->Model != NULL)
    {
        this->Model->Delete();
    }
    this->Model = vtkFloatArray::New();
    this->Model->DeepCopy(model);
}


void vtkTimeCoursePlotActor::SetParadigm(vtkFloatArray *paradigm)
{
    if (paradigm == NULL)
    {
        cout << "vtkTimeCoursePlotActor::SetParadigm: Parameter paradigm is a null pointer!!!" 
             << endl;
        return;
    }
    
    float *paradigmPtr = paradigm->GetPointer(0); 
    this->TotalVolumes = (int) paradigmPtr[0];
    this->NumberOfConditions = (int) paradigmPtr[1];
    this->VolumesPerBaseline = (int) paradigmPtr[2];
    this->VolumesPerCondition = (int) paradigmPtr[3];
    this->VolumesAtStart = (int) paradigmPtr[4];
    this->StartsWith = (int) paradigmPtr[5];
}


vtkTimeCoursePlotActor::~vtkTimeCoursePlotActor()
{
    if (this->Model != NULL)
    {
        this->Model->Delete();
    }

    if (this->TimeCourse != NULL)
    {
        this->TimeCourse->Delete();
    }

    if (this->ConditionMaxTimeCourse != NULL)
    {
        this->ConditionMaxTimeCourse->Delete();
    }
 
    if (this->ConditionMinTimeCourse != NULL)
    {
        this->ConditionMinTimeCourse->Delete();
    }
 
    if (this->ConditionAverageTimeCourse != NULL)
    {
        this->ConditionAverageTimeCourse->Delete();
    }

    if (this->BaselineMaxTimeCourse != NULL)
    {
        this->BaselineMaxTimeCourse->Delete();
    }

    if (this->BaselineMinTimeCourse != NULL)
    {
        this->BaselineMinTimeCourse->Delete();
    }

    if (this->BaselineAverageTimeCourse != NULL)
    {
        this->BaselineAverageTimeCourse->Delete();
    }
}


vtkDataSet* vtkTimeCoursePlotActor::CreateDataSet(vtkFloatArray *yPoints, float *xPoints)
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

void vtkTimeCoursePlotActor::MakeTimeCourses()
{
    float range[2];
    this->TimeCourse->GetRange(range);
    this->Min = range[0];
    this->Max = range[1];

    if (this->ConditionMaxTimeCourse != NULL)
    {
        this->ConditionMaxTimeCourse->Delete();
    }
    this->ConditionMaxTimeCourse = vtkFloatArray::New();
    this->ConditionMaxTimeCourse->SetNumberOfTuples(this->VolumesPerCondition);
    this->ConditionMaxTimeCourse->SetNumberOfComponents(1);
 
    if (this->ConditionMinTimeCourse != NULL)
    {
        this->ConditionMinTimeCourse->Delete();
    }
    this->ConditionMinTimeCourse = vtkFloatArray::New();
    this->ConditionMinTimeCourse->SetNumberOfTuples(this->VolumesPerCondition);
    this->ConditionMinTimeCourse->SetNumberOfComponents(1);
 
    if (this->ConditionAverageTimeCourse != NULL)
    {
        this->ConditionAverageTimeCourse->Delete();
    }
    this->ConditionAverageTimeCourse = vtkFloatArray::New();
    this->ConditionAverageTimeCourse->SetNumberOfTuples(this->VolumesPerCondition);
    this->ConditionAverageTimeCourse->SetNumberOfComponents(1);

    if (this->BaselineMaxTimeCourse != NULL)
    {
        this->BaselineMaxTimeCourse->Delete();
    }
    this->BaselineMaxTimeCourse = vtkFloatArray::New();
    this->BaselineMaxTimeCourse->SetNumberOfTuples(this->VolumesPerBaseline);
    this->BaselineMaxTimeCourse->SetNumberOfComponents(1);

    if (this->BaselineMinTimeCourse != NULL)
    {
        this->BaselineMinTimeCourse->Delete();
    }
    this->BaselineMinTimeCourse = vtkFloatArray::New();
    this->BaselineMinTimeCourse->SetNumberOfTuples(this->VolumesPerBaseline);
    this->BaselineMinTimeCourse->SetNumberOfComponents(1);

    if (this->BaselineAverageTimeCourse != NULL)
    {
        this->BaselineAverageTimeCourse->Delete();
    }
    this->BaselineAverageTimeCourse = vtkFloatArray::New();
    this->BaselineAverageTimeCourse->SetNumberOfTuples(this->VolumesPerBaseline);
    this->BaselineAverageTimeCourse->SetNumberOfComponents(1);
    
    int cStart = (this->StartsWith == 1 ? (this->VolumesAtStart + 
        this->VolumesPerBaseline) : this->VolumesAtStart);
    int bStart = (this->StartsWith == 0 ? (this->VolumesAtStart + 
        this->VolumesPerCondition) : this->VolumesAtStart);

    float *timeCoursePtr = this->TimeCourse->GetPointer(0);  
    float total, max, min;
    
    // For condition
    for (int i = 0; i < this->VolumesPerCondition; i++)
    {
        total = 0.0;
        max = 0.0;
        min = 1000000.0;
        int j = cStart + i;
        int count = 0;

        while (j < this->TotalVolumes)
        {
            total += timeCoursePtr[j];
            if (timeCoursePtr[j] > max)
            {
                max = timeCoursePtr[j];
            }
            if (timeCoursePtr[j] < min)
            {
                min = timeCoursePtr[j];
            }

            j += (this->VolumesPerBaseline + this->VolumesPerCondition);
            count++;
        }

        this->ConditionMinTimeCourse->SetComponent(i, 0, min);
        this->ConditionMaxTimeCourse->SetComponent(i, 0, max);
        float ave = total / count;
        this->ConditionAverageTimeCourse->SetComponent(i, 0, ave);
   }
   
    // For baseline 
    for (int i = 0; i < this->VolumesPerBaseline; i++)
    {
        total = 0.0;
        max = 0.0;
        min = 1000000.0;
        int j = bStart + i;
        int count = 0;

        while (j < this->TotalVolumes)
        {
            total += timeCoursePtr[j];
            if (timeCoursePtr[j] > max)
            {
                max = timeCoursePtr[j];
            }
            if (timeCoursePtr[j] < min)
            {
                min = timeCoursePtr[j];
            }

            j += (this->VolumesPerBaseline + this->VolumesPerCondition);
            count++;
        }

        this->BaselineMinTimeCourse->SetComponent(i, 0, min);
        this->BaselineMaxTimeCourse->SetComponent(i, 0, max);
        float ave = total / count;
        this->BaselineAverageTimeCourse->SetComponent(i, 0, ave); 
   }
}


void vtkTimeCoursePlotActor::SetPlotLong(vtkFloatArray *timeCourse)
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

    // Scales stimulus for plotting 
    int size = this->Model->GetNumberOfTuples();
    float *stimPtr = this->Model->GetPointer(0);  
    float *stimTmp = new float [size];
    memcpy(stimTmp, stimPtr, size);
    vtkFloatArray *tc = vtkFloatArray::New();
    tc->SetNumberOfTuples(size);
    tc->SetNumberOfComponents(1);
    for (int i = 0; i < size; i++)
    {
        if (stimPtr[i] != 0.0)
        {
            stimTmp[i] = (max + min) / 2;
        }
        else
        {
            stimTmp[i] = min;
        }
        tc->SetComponent(i, 0, stimTmp[i]);
    }
    // Creates data set for stimulus 
    vtkDataSet* stimDataSet = CreateDataSet(tc, xPoints);
    delete [] xPoints;
    delete [] stimTmp;

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
    this->GetLegendBoxActor()->SetEntryString(1, "Paradigm");
    this->LegendOn();
    this->SetYRange(min, max);
    this->SetXRange(0.0, size);
    this->SetNumberOfXLabels(13);
}


void vtkTimeCoursePlotActor::SetPlotShort(vtkFloatArray *timeCourse)
{
    if (timeCourse == NULL)
    {
        cout << "vtkTimeCoursePlotActor::SetPlotShort: Parameter timeCourse is a null pointer!!!" 
             << endl;
        return;
    }

    if (this->TimeCourse != NULL)
    {
        this->TimeCourse->Delete();
    }
    this->TimeCourse = vtkFloatArray::New();
    this->TimeCourse->DeepCopy(timeCourse);

    MakeTimeCourses();

    // Creates x points for condition
    float *xPointsForCondition = new float [this->VolumesPerCondition];
    for (int i = 0; i < this->VolumesPerCondition; i++)
    {
        xPointsForCondition[i] = i;
    }
    float *xPointsForBaseline = new float [this->VolumesPerBaseline];
    for (int i = 0; i < this->VolumesPerBaseline; i++)
    {
        xPointsForBaseline[i] = i;
    }

/*
    float *testPoints = new float [2];
    testPoints[0] = 5;
    testPoints[1] = 5.1;

    vtkFloatArray *testData = vtkFloatArray::New();
    testData->SetNumberOfTuples(this->VolumesPerBaseline);
    testData->SetNumberOfComponents(1);
    testData->SetComponent(0, 0, this->Min);
    testData->SetComponent(1, 0, this->Max);

    vtkDataSet* testDataSet = CreateDataSet(testData, testPoints);
*/

    // Creates data sets for condtion  
    vtkDataSet* cMaxDataSet = CreateDataSet(this->ConditionMaxTimeCourse, xPointsForCondition);
    vtkDataSet* cMinDataSet = CreateDataSet(this->ConditionMinTimeCourse, xPointsForCondition);
    vtkDataSet* cAverageDataSet = CreateDataSet(this->ConditionAverageTimeCourse, xPointsForCondition);

    // Creates data sets for baseline  
    vtkDataSet* bMaxDataSet = CreateDataSet(this->BaselineMaxTimeCourse, xPointsForBaseline);
    vtkDataSet* bMinDataSet = CreateDataSet(this->BaselineMinTimeCourse, xPointsForBaseline);
    vtkDataSet* bAverageDataSet = CreateDataSet(this->BaselineAverageTimeCourse, xPointsForBaseline);

    delete [] xPointsForCondition;
    delete [] xPointsForBaseline;

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

    this->AddInput(cMaxDataSet);
    this->AddInput(cAverageDataSet);
    this->AddInput(cMinDataSet);

    this->AddInput(bMaxDataSet);
    this->AddInput(bAverageDataSet);
    this->AddInput(bMinDataSet);

//    this->AddInput(testDataSet);

    this->SetPlotColor(0, 1.0, 0.0, 0.0);
    this->SetPlotColor(1, 1.0, 0.0, 0.0);
    this->SetPlotColor(2, 1.0, 0.0, 0.0);
    this->SetPlotColor(3, 0.0, 0.0, 1.0);
    this->SetPlotColor(4, 0.0, 0.0, 1.0);
    this->SetPlotColor(5, 0.0, 0.0, 1.0);
//    this->SetPlotColor(6, 0.0, 1.0, 0.0);
 
//    this->GetPositionCoordinate()->SetValue(0.05, 0.1, 0); // start position
//    this->GetPosition2Coordinate()->SetValue(0.9, 0.8, 0); //relative to position
    this->GetPositionCoordinate()->SetValue(0.05, 0.01, 0); // start position
    this->GetPosition2Coordinate()->SetValue(0.95, 0.95, 0); //relative to position
//    this->SetXValuesToNormalizedArcLength();
//    this->SetXValuesToValue();
    this->SetXValuesToArcLength();

/*
    this->GetLegendBoxActor()->SetEntryString(0, "C-Max");
    this->GetLegendBoxActor()->SetEntryString(1, "C-Average");
    this->GetLegendBoxActor()->SetEntryString(2, "C-Min");
    this->GetLegendBoxActor()->SetEntryString(3, "B-Max");
    this->GetLegendBoxActor()->SetEntryString(4, "B-Average");
    this->GetLegendBoxActor()->SetEntryString(5, "B-Min");
    this->GetLegendBoxActor()->SetEntryString(6, "Test");
*/

    this->SetPlotLabel(0, "C-Max");
    this->SetPlotLabel(1, "C-Average");
    this->SetPlotLabel(2, "C-Min");
    this->SetPlotLabel(3, "B-Max");
    this->SetPlotLabel(4, "B-Average");
    this->SetPlotLabel(5, "B-Min");

    this->LegendOn();
    this->PlotLinesOff();
    this->PlotCurveLinesOn();
    this->PlotCurvePointsOn();

    this->GetProperty()->SetLineWidth(1);
    this->GetProperty()->SetPointSize(5);

    this->SetPlotLines(0, 0);
    this->SetPlotLines(2, 0);
    this->SetPlotLines(3, 0);
    this->SetPlotLines(5, 0);

    this->SetPlotPoints(0, 1);
    this->SetPlotPoints(1, 1);
    this->SetPlotPoints(2, 1);
    this->SetPlotPoints(3, 1);
    this->SetPlotPoints(4, 1);
    this->SetPlotPoints(5, 1);

    this->SetYRange(this->Min, this->Max);
    int size = (this->VolumesPerCondition > this->VolumesPerBaseline ? this->VolumesPerCondition : 
        this->VolumesPerBaseline);
    this->SetXRange(0.0, size);
    this->SetNumberOfXLabels(5);
}


void vtkTimeCoursePlotActor::SetVoxelIndex(int i, int j, int k)
{
    this->i = i;
    this->j = j;
    this->k = k;
}


