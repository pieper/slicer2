/*  ==================================================
    Module : vtkLevelSets
    Authors: Karl Krissian
    Email  : karl@bwh.harvard.edu

    This module implements a Active Contour evolution
    for segmentation of 2D and 3D images.
    It implements a 'codimension 2' levelsets as an
    option for the smoothing term.
    It comes with a Tcl/Tk interface for the '3D Slicer'.
    ==================================================
    Copyright (C) 2003  LMI, Laboratory of Mathematics in Imaging, 
    Brigham and Women's Hospital, Boston MA USA

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    ================================================== 
   The full GNU Lesser General Public License file is in vtkLevelSets/LesserGPL_license.txt
*/

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkImagePropagateDist2.h,v $
  Language:  C++
  Date:      $Date: 2003/04/28 18:13:24 $
  Version:   $Revision: 1.1 $
  Author:    Karl Krissian

=========================================================================*/
// .NAME vtkImageIsoContourDist - short description

// .SECTION Description
// Compute the distance to an isocontour by propagation
//


#ifndef _vtkImagePropagateDist2_h
#define _vtkImagePropagateDist2_h

#define POINT_NOT_PARSED    0
#define POINT_TRIAL         1
#define POINT_TRIAL_INLIST  2
#define POINT_SET_FRONT     3
#define POINT_SET           4

#include <vtkLevelSetsConfigure.h>
#include "vtkImageToImageFilter.h"
#include "vtkFloatArray.h"

//BTX
class PD_element2 {
  public:
    PD_element2() {  
      x=y=z=0;
      state=POINT_NOT_PARSED;
      // initialize track to -1, mean no track point yet
      track         = -1;
      prev_neighbor = -1;
      skeleton = 0;
    };

    void Init( const float& px,
      const float& py,
      const float& pz,
      const unsigned char& st,
      const int& t) 
   {
     x=px;
     y=py;
     z=pz;
     state=st;
     track=t;
   }
 
  const float& X() {return x;}
  const float& Y() {return y;}
  const float& Z() {return z;}

  const int&           GetTrack()        {return track;}
  const char&          GetPrevNeighbor() {return prev_neighbor;}
  const unsigned char& GetState()        {return state;}
  const unsigned char& GetSkeleton()     {return skeleton;}

  void SetPos( const float& px, 
           const float& py,
           const float& pz
      )
    {
      x = px;
      y = py;
      z = pz;
    }

  void SetPosTrack( const float& px, 
           const float& py,
           const float& pz,
           const int& t
      )
    {
      x = px;
      y = py;
      z = pz;
      track = t;
    }

  void SetState( const unsigned char& st)
  {
    state=st;
  }

  void SetTrack( const int& t)
  {
    track=t;
  }

  void SetSkeleton( const unsigned char& s)
  {
    skeleton=s;
  }

  void SetPrevNeighbor( const int& n)
  {
    prev_neighbor=n;
  }

  float x;
  float y;
  float z;
  unsigned char state;
  int track;
  char prev_neighbor;
  unsigned char skeleton;
};

//ETX

class VTK_LEVELSETS_EXPORT vtkImagePropagateDist2 : public vtkImageToImageFilter
{
public:
  static vtkImagePropagateDist2 *New();
  vtkTypeMacro(vtkImagePropagateDist2,vtkImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description

  // Intensity of the IsoContour
  vtkSetMacro(threshold,float);
  vtkGetMacro(threshold,float);

  // Maximal distance >0
  vtkSetMacro(maxdist,float);
  vtkGetMacro(maxdist,float);

  // Minimal distance <0
  vtkSetMacro(mindist,float);
  vtkGetMacro(mindist,float);


  void InitParam(       vtkImageData* in, vtkImageData* out);

  void UseOutputArray(float* array) { output_array = array; }

  void SetNarrowBand( int* band, int size);

  int* GetExtent() { return extent;}

  // 
  void GetSkeleton(   vtkImageData* skeleton);

protected:
  vtkImagePropagateDist2();
  ~vtkImagePropagateDist2();

  void ExecuteData(vtkDataObject *outData);

  void PreComputeDistanceArray();
  void FreeDistanceArray();

  void InitLists();
  void FreeLists();

  void IsoSurfDist2D( );
  void IsoSurfDist3D( );
  void IsoSurfDist3D_band(  int first_band, int last_band);
  void IsoSurfDistInit( );

  void PropagateDanielsson2D();
  void PropagateDanielsson3D();

  // Methods for saving information from the process as images
  void SaveTrajectories2D( int num);
  void SaveTrajectories3D( int num);
  void SaveProjection(     int num);
  void SaveState(          int num);
  void SaveSkeleton(       int num);
  void SaveDistance(       int num);

  //  void GetProjection( vtkImageData* im);


//BTX
  float maxdist;
  float mindist;

  int tx,ty,tz,txy;
  unsigned long imsize;

  // We copy input data in float format
  vtkImageData *inputImage;
  int inputImage_allocated;

  vtkImageData *outputImage;

  // allows to use an array for the output image without allocation
  float* output_array;

  // Extent
  int extent[6];

  // precomputed table for the square
  int*    sq;
  int     sq_size;
  double*  sqroot;
  int     sqroot_size;
  
  double***  distance;

  // List information
  int*      list0;  
  int*      list1;  
  int*      list_remaining_trial;  
  int       list0_size;
  int       list1_size;
  int       list_remaining_trial_size;  
  int       list_maxsize;
  PD_element2* list_elts;

  unsigned char save_intermediate_images;
  unsigned char trial_loop;

  float threshold;

  // Narrow Band Information
  int*           narrowband;
  int            bandsize;  

  // For use of output array
  vtkFloatArray* float_array;
  unsigned char  float_array_allocated;

//ETX
};


#endif // _vtkImagePropagateDist2_h
