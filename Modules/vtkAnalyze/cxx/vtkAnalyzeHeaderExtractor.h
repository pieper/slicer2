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

// .NAME vtkAnalyzeHeaderExtractor - Extracts infor from Analyze hdr file 
// .SECTION Description
// vtkAnalyzeHeaderExtractor is used to read header information from 
// any Analyze header file (.hdr).


#ifndef __vtkAnalyzeHeaderExtractor_h
#define __vtkAnalyzeHeaderExtractor_h


#include <vtkAnalyzeConfigure.h>
#include "AnalyzeHeader.h"
#include "vtkObject.h"

class VTK_ANALYZE_EXPORT vtkAnalyzeHeaderExtractor : public vtkObject 
{
public:

    static vtkAnalyzeHeaderExtractor *New();
    vtkTypeMacro(vtkAnalyzeHeaderExtractor,vtkObject);

    vtkAnalyzeHeaderExtractor();
    ~vtkAnalyzeHeaderExtractor();

    // Description:
    // Gets data type. It could be:
    // DT_NONE (0), DT_UNKNOWN (0), DT_BINARY (1), 
    // DT_UNSIGNED_CHAR (2), DT_SIGNED_SHORT (4),
    // DT_SIGNED_INT (8), DT_FLOAT (16), DT_COMPLEX (32),
    // DT_DOUBLE (64), DT_RGB (128), and DT_ALL (255).
    vtkGetMacro(DataType, int);

    // Description:
    // Gets voxel order (orientation) 
    vtkGetMacro(Orient, int);

    // Description:
    // Gets Analyze format (3D or 4D). 
    vtkGetMacro(FileFormat, int);

    // Description:
    // Gets bits per pixel; 1, 8, 16, 32, or 64.
    vtkGetMacro(BitsPix, int);

    // Description:
    // Gets the image dimensions.  
    // ImageDim[0] - image X dimension; number of pixels in an image row.
    // ImageDim[1] - image Y dimension; number of pixels in an image column.
    // ImageDim[2] - volume Z dimension; number of slices in a volume.
    // ImageDim[3] - time points; number of volumes in the sequence.
    vtkGetVector4Macro(ImageDim, int);

    // Description:
    // Gets the pix dimensions.  
    // PixDim[0] - voxel width in mm. 
    // PixDim[1] - voxel height in mm. 
    // PixDim[2] - slice thickness in mm. 
    vtkGetVector3Macro(PixDim, float);

    // Description:
    // Gets the pix range.  
    // PixRange[0] - max pixel value for entire sequence. 
    // PixRange[1] - min pixel value for entire sequence. 
    vtkGetVector2Macro(PixRange, int);

    // Description:
    // Reads the Analyze header file. 
    void Read();

    // Description:
    // Determines the byte order. 
    int IsLittleEndian();

    // Description:
    // Displays all header infor. 
    void ShowHeader(); 

    // Description:
    // Specifies a header file name to read. 
    void SetFileName(const char *);
    vtkGetStringMacro(FileName);

private:

    void SwapLong(unsigned char *); 
    void SwapShort(unsigned char *); 
    void SwapHeader();

    char *FileName;

    int Orient;
    int FileFormat;
    int DataType;
    int BitsPix;

    int ImageDim[4];
    float PixDim[3];
    int PixRange[2];

    dsr Hdr;
    int Swapped;
};


#endif
