/*=auto==========================================================================
 (c) Copyright 2002 Massachusetts Institute of Technology

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
==========================================================================auto=*/
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
  }


  friend int operator < ( const TrialPoint& p1, const TrialPoint& p2)
  {
    return (p1.value < p2.value);
  }

  friend int operator > ( const TrialPoint& p1, const TrialPoint& p2)
  {
    return (p1.value > p2.value);
  }

  friend ostream& operator << (ostream&, const TrialPoint& p);

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
  vtkSetMacro(MinThreshold,float);
  vtkGetMacro(MinThreshold,float);

  // Maximum threshold
  vtkSetMacro(MaxThreshold,float);
  vtkGetMacro(MaxThreshold,float);

  // Criterion image
  vtkSetObjectMacro(Criterion,vtkImageData);
  vtkGetObjectMacro(Criterion,vtkImageData);
  
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

  void Init();

  void Thin_init_pos( );
  
  void init_neighborhoods();

  Boolean IsEndPoint( vtkImageData* im, int x, int y, int z);

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
  vtkImageData*  Criterion;
  vtkImageData* OutputImage;

  float MinThreshold;
  float MaxThreshold;
  
  char UseLineEndpoints;
  char UseFiducialEndpoints;
  char UseSurfaceEndpoints;

//ETX
}; // vtkThinning()

#endif // __vtkThinning_h
