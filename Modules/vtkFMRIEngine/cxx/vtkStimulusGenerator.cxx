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


#include "vtkStimulusGenerator.h"
#include "vtkObjectFactory.h"

#ifdef _WIN32
#include <fstream>
#include <ostream>
#include <string>
#else
#include <fstream.h>
#include <ostream.h>
#include <string.h>
#endif

using std::string;


vtkStandardNewMacro(vtkStimulusGenerator);


void vtkStimulusGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
    vtkObject::PrintSelf(os, indent);

    os << indent << "Paradigm design file name: " << this->FileName << endl;
    os << indent << "Total volumes per run: " << this->TotalVolumesPerRun << endl;
    os << indent << "Volumes per baseline: " << this->TotalVolumesPerRun << endl;
    os << indent << "Volumes per task: " << this->VolumesPerTask << endl;
    os << indent << "Starts with: " << this->Start << "(0 = baseline; 1 = task)" << endl;
}


vtkStimulusGenerator::vtkStimulusGenerator()
{
    this->FileName = NULL;
    this->TotalVolumesPerRun = 0;
    this->VolumesPerBaseline = 0;
    this->VolumesPerTask = 0;
    this->Start = false;
    this->StimulusArray = NULL;
}


vtkStimulusGenerator::~vtkStimulusGenerator()
{
    if (this->StimulusArray != NULL)
    {
        this->StimulusArray->Delete();
    }
}


vtkFloatArray *vtkStimulusGenerator::CreateStimulusFromFile()
{
    if (ParseParadigmFile() == 1)  // failed
    {
        return NULL;
    }

    int count = 0;
    float *stim = new float [this->TotalVolumesPerRun];
    bool doBaseline = (this->Start ? false : true);
    bool doTask = (! doBaseline);
    while (count < this->TotalVolumesPerRun)
    {
        if (doBaseline)
        {
            int baselineLength 
                = (count == 0 ? this->VolumesAtStart : this->VolumesPerBaseline);
            for (int i = 0; i < baselineLength ; i++)
            {
                stim[count++] = 0.0;
            }
            doBaseline = false;
            doTask = true;
        }
        if (doTask && count < this->TotalVolumesPerRun)
        {
            int taskLength 
                = (count == 0 ? this->VolumesAtStart : this->VolumesPerTask);

            for (int i = 0; i < taskLength; i++)
            {
                stim[count++] = 1.0;
            }
            doBaseline = true;
            doTask = false;
        }
    }

    this->StimulusArray = vtkFloatArray::New();
    this->StimulusArray->SetNumberOfTuples(this->TotalVolumesPerRun);
    this->StimulusArray->SetNumberOfComponents(1);
    for (int i = 0; i < this->TotalVolumesPerRun; i++)
    {
        this->StimulusArray->SetComponent(i, 0, stim[i]);
    }

    delete [] stim;
    return this->StimulusArray;
}


vtkFloatArray *vtkStimulusGenerator::GetStimulus()
{
    return this->StimulusArray;
}


// 0 - successful; 1 - failed
int vtkStimulusGenerator::ParseParadigmFile()
{
    ifstream openFile(this->FileName, ios::in);
    if (openFile == 0)
    {
        vtkErrorMacro(<<"Can't open file: " << this->FileName << endl);
        return 1;
    }

    // Reads and parses file contents
    static const string COLON = ":";
    static const string START = "start with";
    static const string TOTAL_VOLUMES = "total volumes";
    static const string VOLUMES_PER_TASK = "volumes per task";
    static const string VOLUMES_AT_START = "volumes at start";
    static const string NUMBER_OF_PREDICTORS = "number of stimuli";
    static const string VOLUMES_PER_BASELINE = "volumes per baseline";

    string var;
    string val;
    char arr[51];
    string paradigm = "";
    int count = 0;
    while(! openFile.eof())
    {
        openFile.getline(arr, 50, '\n');
        paradigm = arr;
        string::size_type pos = paradigm.find (COLON, 0);   
        if (pos > 0)
        { 
            var = paradigm.substr(0, pos);
            val = paradigm.substr(pos + 1, paradigm.length());
            if (var == TOTAL_VOLUMES)
            {
                this->TotalVolumesPerRun = atoi(val.c_str());
                count++;
            }
            else if (var == NUMBER_OF_PREDICTORS)
            {
                this->NumberOfPredictors = atoi(val.c_str());
                count++;
            }
            else if (var == VOLUMES_PER_BASELINE)
            {
                this->VolumesPerBaseline = atoi(val.c_str());
                count++;
            }
            else if (var == VOLUMES_AT_START)
            {
                this->VolumesAtStart = atoi(val.c_str());
                count++;
            }
            else if (var == VOLUMES_PER_TASK)
            {
                this->VolumesPerTask = atoi(val.c_str());
                count++;
            }
            else if (var == START)
            {
                this->Start = (atoi(val.c_str()) == 0 ? false : true);
                count++;
            }
        }
    }
    openFile.close();

    if (count != 6) // all parameters are parsed
    {
        vtkErrorMacro(<<"Paradigm design file content is bad.\n");
        return 1;    
    }

    return 0;
}
