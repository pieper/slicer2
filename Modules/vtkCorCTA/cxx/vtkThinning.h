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
// .NAME vtkThinning - short description

#ifndef __vtkThinning_h
#define __vtkThinning_h

// Authors: Karl Krissian, Arne Hans
//
// .SECTION Description
// vtkThinning 
// 3D thinning using simple points
//  
//


#include <vtkCorCTAConfigure.h>
#include "vtkImageToImageFilter.h"
#include "vtkImageData.h"

//BTX
#include "vtkMinHeap.h"

#define Boolean unsigned char


//----------------------------------------------------------------------
//
class TrialPoint {
//    ----------
public:
  int   x,y,z;
  float value;

  TrialPoint()
  {
    x = y = z = 0;
    value = 1000;
  }

  TrialPoint(int px, int py, int pz, float val)
  {
    x     = px;
    y     = py;
    z     = pz;
    value = val;
  }

  TrialPoint& operator=(const TrialPoint& p)
  {
    x     = p.x;
    y     = p.y;
    z     = p.z;
    value = p.value;
    return *this;
  }


  friend int operator < ( const TrialPoint& p1, const TrialPoint& p2)
  {
    return (p1.value < p2.value);
  }

  friend int operator > ( const TrialPoint& p1, const TrialPoint& p2)
  {
    return (p1.value > p2.value);
  }

/*  friend ostream& operator << (ostream&, const TrialPoint& p); */

}; // TrialPoint
//ETX


//----------------------------------------------------------------------
class VTK_CORCTA_EXPORT vtkThinning : public vtkImageToImageFilter
{
public:
  vtkTypeMacro(vtkThinning,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct object to extract all of the input data.
  static vtkThinning *New();

  // Minimum threshold
  vtkSetMacro(MinCriterionThreshold,float);
  vtkGetMacro(MinCriterionThreshold,float);

  // Maximum threshold
  vtkSetMacro(MaxEndpointThreshold,float);
  vtkGetMacro(MaxEndpointThreshold,float);

  // Criterion image
  vtkSetObjectMacro(Criterion,vtkImageData);
  vtkGetObjectMacro(Criterion,vtkImageData);

  // Endpoint Criterion image
  vtkSetObjectMacro(EndpointCriterion,vtkImageData);
  vtkGetObjectMacro(EndpointCriterion,vtkImageData);
  
  // Output image
  vtkSetObjectMacro(OutputImage,vtkImageData);
  vtkGetObjectMacro(OutputImage,vtkImageData);
  
  // Endpoints
  vtkSetMacro(UseLineEndpoints,char);
  vtkGetMacro(UseLineEndpoints,char);
  
  vtkSetMacro(UseFiducialEndpoints,char);
  vtkGetMacro(UseFiducialEndpoints,char);
  
  vtkSetMacro(UseSurfaceEndpoints,char);
  vtkGetMacro(UseSurfaceEndpoints,char);

  /*
  void ComputeInputUpdateExtent(int inExt[6], 
                int outExt[6]);
  */

  /*
  void ThreadedExecute(vtkImageData *inData, 
               vtkImageData *outData,
               int extent[6], int threadId);
  */

protected:

  vtkThinning();
  ~vtkThinning();

  Boolean IsSimple( vtkImageData* im,
            int x, int y, int z, int& cc1, int& cc2);
  int ComputeCstar ( vtkImageData* im,
             int x, int y, int z);
  int ComputeCbar ( vtkImageData* im,
            int x, int y, int z);
  void Init();

  void Thin_init_pos( );
  
  void init_neighborhoods();

  Boolean IsLineEndPoint(vtkImageData* im, int x, int y, int z);
  Boolean IsSurfaceEndPoint(vtkImageData* im, int x, int y, int z);
  Boolean IsFiducialEndPoint(vtkImageData* im, int x, int y, int z);
  Boolean IsEndPoint( vtkImageData* im, int x, int y, int z);

  unsigned char CoordOK(vtkImageData* im,int x,int y,int z);

  void ParseCC( int* domain, 
  //   -------
        int neighborhood[27][27], 
        int* cc,
        int point, 
        int num_cc );

  //
  void ExecuteData(vtkDataObject* output);

//BTX
  //
 int N26_star[27][27]; // 26 adjacent neighbors for each point in 
                             // the N26* neighborhood:
                             // N26_star[i][0] = number of neighbors

 int N18[27][27]; // 6 adjacent neighbors for each point in 
                        // the N18 neighborhood:
                        // N18[i][0] = number of neighbors

 int pos[3][3][3];
 int neighbors_pos  [27];
 int neighbors_place[27][3];

  int tx,ty,tz,txy;


  vtkImageData*  input_image;
  // Ordering of the Points (usually distance)
  vtkImageData*  Criterion;
  // Scalar value skeleton's location confidence (average outward flux)
  vtkImageData*  EndpointCriterion;
  vtkImageData*  OutputImage;

  // Points above this threshold will be removed no matter what
  float MinCriterionThreshold;
  // Points above this threshold will not be removed if they are endpoints
  float MaxEndpointThreshold;
  
  char UseLineEndpoints;
  char UseFiducialEndpoints;
  char UseSurfaceEndpoints;

//ETX
}; // vtkThinning()

#endif // __vtkThinning_h
