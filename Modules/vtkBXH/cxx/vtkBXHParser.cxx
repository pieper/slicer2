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
#include "vtkObjectFactory.h"
#include "vtkBXHParser.h"
#include <ctype.h>

vtkStandardNewMacro(vtkBXHParser);


vtkBXHParser::vtkBXHParser()
{
    this->ElementValue = NULL;    
}


vtkBXHParser::~vtkBXHParser()
{
    if (ElementValue != NULL)
    {
        delete [] this->ElementValue;
    }
    
    istream *fs = this->GetStream();
    if (fs != NULL)
    {
        delete fs;
    }
}


char *vtkBXHParser::ReadElementValue(vtkXMLDataElement *element)
{
    if (element == NULL)
    {
        cout << "vtkBXHParser::ReadElementValue: element is NULL.\n";
        return NULL;
    }

    int num = element->GetNumberOfNestedElements();
    if (num > 0)
    {
        cout << "TvtkBXHParser::ReadElementValue: element is NOT a leaf element.\n";
        return NULL;
    }
    else
    {
        istream *stream = this->GetStream();
        if (stream == NULL)
        {
            ifstream *ifs = new ifstream;  
            ifs->open(this->GetFileName(), ios::in); 
            this->SetStream(ifs);
            stream = this->GetStream();
        } 

        // Scans for the start of the actual inline data.
        char c;
        stream->seekg(element->GetXMLByteIndex());
        stream->clear(stream->rdstate() & ~ios::eofbit);
        stream->clear(stream->rdstate() & ~ios::failbit);
        while(stream->get(c) && (c != '>')) ;
        while(stream->get(c) && isspace(c)) ;
        unsigned long pos = stream->tellg();

        // Value length in number of chars.
        stream->seekg(pos-1);
        int count = 0;
        while (stream->get(c) && (c != '<'))
        {
            count++;            
        }
        if (this->ElementValue != NULL)
        {
            delete [] this->ElementValue;
        }
        this->ElementValue = new char [count+2];
 
        // Reads value
        stream->seekg(pos-1);
        stream->get(this->ElementValue, count+1, '<');
        this->ElementValue[count+1] = '\0';
        return this->ElementValue;
    }
}
