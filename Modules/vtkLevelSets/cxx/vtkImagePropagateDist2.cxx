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


#include "vtkImagePropagateDist2.h"
#include "vtkObjectFactory.h"
//#include "vtkScalars.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkStructuredPointsWriter.h"

#define TRUE    1
#define FALSE   0
#define EPSILON 1E-5

#define Si                if(
#define Alors                 ){
#define Sinon                  }else{
#define FinSi                  }
#define AlorsFait               )


#define SelonQue             switch (
#define Vaut                 ){
#define Valeur               case
#define FinValeur            break;
#define FinSelonQue          }
#define Defaut                  default

// POUR
#define Pour(nombre, min, max)  for(nombre=min; nombre<=max; nombre++) {
#define FinPour                 }


#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))


#define dist2(a,b,c)  (sq[abs(a)]+sq[abs(b)]+sq[abs(c)])
#define dist(a,b,c)  (distance[abs(a)][abs(b)][abs(c)])



//
//---------------------------------------------------------------------------
vtkImagePropagateDist2* vtkImagePropagateDist2::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImagePropagateDist2");
  if(ret)
    {
    return (vtkImagePropagateDist2*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImagePropagateDist2;
}


//----------------------------------------------------------------------------
// Constructor sets default values
vtkImagePropagateDist2::vtkImagePropagateDist2()
{

  maxdist   = 10.0;
  mindist   = -10.0;

  // by default set the coefficients given in Borgefors96

  inputImage           = NULL;
  inputImage_allocated = 0;

  //  save_intermediate_images = 1;
  save_intermediate_images = 0;
  trial_loop = 0;

  narrowband = NULL;
  bandsize   = 0;

  output_array    = NULL;

  float_array = NULL;
  float_array_allocated = 0;

  list0=NULL;
  list1=NULL;
  list_remaining_trial=NULL;
  list_elts=NULL;

} // vtkImagePropagateDist2()


//---------------------------------------------------------------------------
// Destructor
vtkImagePropagateDist2::~vtkImagePropagateDist2()
//              ---------------
{

  FreeLists();

  if (inputImage_allocated) {
    inputImage->Delete();
    inputImage=NULL;
  }

  if (float_array_allocated) {
    float_array->Delete();
    float_array = NULL;
    float_array_allocated = 0;
  }

} // ~vtkImagePropagateDist2()


//---------------------------------------------------------------------------
// Init params and copy data to float
void vtkImagePropagateDist2::InitParam( vtkImageData* input, vtkImageData* output)
//                   ---------
{
  int type;
  int i;


  inputImage = input;

  if (inputImage == NULL) {
    vtkErrorMacro("Missing input");
    return;
  }
  else {

    // check the image is in float format, or convert
    type = input->GetScalarType();
    if (type != VTK_FLOAT) {
      vtkDebugMacro(<<"making a copy of the input into float format");
      // Create a copy of the data
      inputImage = vtkImageData::New();

      inputImage->SetScalarType( VTK_FLOAT);
      inputImage->SetNumberOfScalarComponents(1);
      inputImage->SetDimensions( input->GetDimensions());
      inputImage->SetOrigin(     input->GetOrigin());
      inputImage->SetSpacing(    input->GetSpacing());
      
      inputImage->CopyAndCastFrom(input,input->GetExtent());
      inputImage_allocated = 1;
    }
    else
      inputImage = input;

    tx = this->inputImage->GetDimensions()[0];
    ty = this->inputImage->GetDimensions()[1];
    tz = this->inputImage->GetDimensions()[2];
    txy = tx*ty;
    imsize = txy*tz;
    
    extent[0] = 0;
    extent[1] = tx-1;
    extent[2] = 0;
    extent[3] = ty-1;
    extent[4] = 0;
    extent[5] = tz-1;
                                                      
    //--- outputImage
    outputImage      = output;
    
    outputImage->SetDimensions(inputImage->GetDimensions() );
    outputImage->SetSpacing(   inputImage->GetSpacing() );
    outputImage->SetScalarType(VTK_FLOAT); 
    outputImage->SetNumberOfScalarComponents(1);

    if (output_array != NULL) {
      if (float_array_allocated) {
    float_array->Delete();
    float_array = NULL;
    float_array_allocated = 0;
      }
      float_array = vtkFloatArray::New();
      float_array_allocated = 1;
      float_array->SetArray(output_array,imsize,1);
      outputImage->GetPointData()->SetScalars(float_array);
    } 
    else {
      outputImage->AllocateScalars();
    }

    //    outputImage->CopyAndCastFrom(this->inputImage,
    //                                 this->inputImage->GetExtent());
  }
    
} //  InitParam()


//----------------------------------------------------------------------------
// This method is passed  input and output data, and executes the filter
// algorithm to fill the output from the input.
void vtkImagePropagateDist2::ExecuteData(vtkDataObject *outData)
//                   -------
{

  InitParam( this->GetInput(), this->GetOutput());

  InitLists();

  if (tz == 1) {
    IsoSurfDist2D();
    PropagateDanielsson2D();
  }
  else {
    if (narrowband==NULL) IsoSurfDist3D();
    // no multithread yet ...
    else                  IsoSurfDist3D_band( 0, bandsize-1);
    PropagateDanielsson3D();
  }

  //  FreeLists();


} // ExecuteData()

//----------------------------------------------------------------------
void vtkImagePropagateDist2::IsoSurfDist2D( )
{

  register int          x,y,p;
  register int          i,j,k,n;
  register int          sign, neigh_sign;
  register float        val,val0,val1,diff;
  register float        val0_new,val1_new;
  register int          displace[2];  
  register float        Grad[2];
  register float        vs[3];
  register float        norm;
  unsigned char         grad_computed;
  register float*       inPtr;
  register float*       outPtr;

  this->inputImage->GetSpacing(vs);

  IsoSurfDistInit( );

  displace[0] = 1;
  displace[1] = tx;

  for(y=0; y<=ty-2; y++) {
  inPtr  = (float*) inputImage ->GetScalarPointer(0,y,0);
  for(x=0; x<=tx-2; x++) {

    val0 = *inPtr-threshold;
    sign = (val0>0); 
    grad_computed = FALSE;

    for(n=0;n<=1;n++) {

      val1 = *(inPtr+displace[n])-threshold;
      neigh_sign =  (val1>0);

      if (sign != neigh_sign) {

    if (!grad_computed) {
      // gradient estimation
      Grad[0] = (*(inPtr+1)  - *inPtr)/vs[0];
      Grad[1] = (*(inPtr+tx) - *inPtr)/vs[1];
      
      // Normalization
      norm = sqrt(Grad[0]*Grad[0]+Grad[1]*Grad[1]);
      grad_computed = TRUE;
      if (norm<EPSILON) 
        fprintf(stderr, " %d %d %d norm=%f \n",x,y,0,norm);
    }

    if (sign)
      diff = val0-val1;
    else
      diff = val1-val0;

        if (diff<EPSILON) {
      fprintf(stderr," %d %d %d %d  %f diff< %f \n",x,y,0,n,diff, 
          EPSILON);
      continue;
    }

    if (norm>EPSILON) {
          val = fabs(Grad[n])*vs[n]/norm/diff;
      val0_new = val0*val;
      val1_new = val1*val;

      if (fabs(val0_new)*vs[n]>1) {
        fprintf(stderr," %d %d %d %d  val0=%f val1=%f val0_new=%f \n",
            x,y,0,n,val0,val1,val0_new);
      }

      if (fabs(val1_new)*vs[n]>1) {
        fprintf(stderr," %d %d %d %d  val0=%f val1=%f val1_new=%f \n",
            x,y,0,n,val0,val1,val1_new);
      }

      outPtr  = (float*) outputImage ->GetScalarPointer(x,y,0);
      if (fabs(val0_new)<fabs(*outPtr)) {
        *outPtr = val0_new;
            p = x+y*tx;
        if (list_elts[p].GetTrack()==-1) {
          list0[list0_size++] = p;
        }
        list_elts[p].Init(val0_new*Grad[0]/norm,
                  val0_new*Grad[1]/norm,
                  0,
                  POINT_SET_FRONT,
                  p);
      } // end if
      
      outPtr  +=  displace[n];
      if (fabs(val1_new)<fabs(*outPtr)) {
        *outPtr  = val1_new;
            p = x+y*tx+displace[n];
        if (list_elts[p].GetTrack()==-1) list0[list0_size++] = p;
        list_elts[p].Init(val1_new*Grad[0]/norm,
                  val1_new*Grad[1]/norm,
                  0,
                  POINT_SET_FRONT,
                  p);
      } // end if

    } else
      fprintf(stderr,"Func_IsoSurfDist2D() \t norm<=EPSILON \n");

      } // end if (sign != sign_neigh)
    } // end for n

    inPtr++;

  } // end for x
  } // end for y


} // IsoSurfDist2D()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::IsoSurfDist3D( )
{

  register int          x,y,z,p;
  register int          i,j,k,n;
  register int          sign, neigh_sign;
  register float        val,val0,val1,diff;
  register float        val0_new,val1_new;
  register int          displace[3];  
  register float        Grad[3];
  register float        vs[3];
  register float        alpha_0;
  register float        alpha_1;
  register float        norm;
  unsigned char         grad_computed;
  register float*       inPtr;
  register float*       inPtr1;
  register float*       outPtr;

  fprintf(stderr,"IsoSurf3D() begin \n");

  this->inputImage->GetSpacing(vs);


  fprintf(stderr,"IsoSurf3D() initfar \n");

  IsoSurfDistInit( );

  displace[0] = 1;
  displace[1] = tx;
  displace[2] = txy;

  fprintf(stderr,"IsoSurf3D() compute \n");

  for(z=1; z<=tz-2; z++) {
  for(y=1; y<=ty-2; y++) {
  inPtr  = (float*) inputImage ->GetScalarPointer(1,y,z);
  for(x=1; x<=tx-2; x++) {

    val0 = *inPtr-threshold;
    sign = (val0>0); 
    grad_computed = FALSE;

    for(n=0;n<=2;n++) {

      inPtr1 = inPtr+displace[n];
      val1 = *inPtr1-threshold;
      neigh_sign =  (val1>0);

      if (sign != neigh_sign) {

    if (!grad_computed) {
      // gradient estimation
      Grad[0] = (*(inPtr+1)  - *inPtr)/vs[0];
      Grad[1] = (*(inPtr+tx) - *inPtr)/vs[1];
      Grad[2] = (*(inPtr+txy)- *inPtr)/vs[2];

      // Normalization
      norm = sqrt(Grad[0]*Grad[0]+Grad[1]*Grad[1]+Grad[2]*Grad[2]);
      grad_computed = TRUE;
    }
      

    if (sign)
      diff = val0-val1;
    else
      diff = val1-val0;

        if (diff<EPSILON) {
      fprintf(stderr," %d %d %d %d  %f diff< %f \n",x,y,z,n,diff,
          EPSILON);
      continue;
    }


    if (norm<EPSILON)
      fprintf(stderr, " %d %d %d norm=%f \n",x,y,z,norm);


    if (norm>EPSILON) {
          val = fabs(Grad[n])*vs[n]/norm/diff;

      val0_new = val0*val;
      val1_new = val1*val;

      outPtr  = (float*) outputImage ->GetScalarPointer(x,y,z);
      if (fabs(val0_new)<fabs(*outPtr)) {
        *outPtr = val0_new;
            p = x+y*tx+z*txy;
        if (list_elts[p].GetTrack()==-1) list0[list0_size++] = p;
        list_elts[p].Init(val0_new*Grad[0]/norm,
                  val0_new*Grad[1]/norm,
                  val0_new*Grad[2]/norm,
                  POINT_SET_FRONT,
                  p);
      } // end if
      
      outPtr  +=  displace[n];
      if (fabs(val1_new)<fabs(*outPtr)) {
        *outPtr  = val1_new;
            p = x+y*tx+z*txy+displace[n];
        if (list_elts[p].GetTrack()==-1) list0[list0_size++] = p;
        list_elts[p].Init(val1_new*Grad[0]/norm,
                  val1_new*Grad[1]/norm,
                  val1_new*Grad[2]/norm,
                  POINT_SET_FRONT,
                  p);
      } // end if
    } else
      fprintf(stderr,"Func_IsoSurfDist3D() \t norm<=EPSILON \n");

      } // end if  (sign != sign_neigh)
    } // end for n

    inPtr++;

  } // end for x
  } // end for y
  } // end for z

  fprintf(stderr,"IsoSurf3D() end \n");

} // IsoSurfDist3D()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::IsoSurfDist3D_band( int first_band, int last_band)
{

  register int          x,y,z,p;
  register int          nbp; // narrow band position
  register int          i,j,k,n;
  register int          sign, neigh_sign;
  register float        val,val0,val1,diff;
  register float        val0_new,val1_new;
  register int          displace[3];  
  register float        Grad[3];
  register float        vs[3];
  register float        alpha_0;
  register float        alpha_1;
  register float        norm;
  unsigned char         grad_computed;
  register float*       inPtr0;
  register float*       outPtr0;
  register float*       inPtr;
  register float*       outPtr;
  register float*       inPtr1;

  fprintf(stderr,"IsoSurfDist3D_band() begin \n");

  this->inputImage->GetSpacing(vs);


  fprintf(stderr,"IsoSurfDist3D_band() initfar \n");

  IsoSurfDistInit( );

  displace[0] = 1;
  displace[1] = tx;
  displace[2] = txy;

  inPtr0   = (float*) inputImage  ->GetScalarPointer();
  outPtr0  = (float*) outputImage ->GetScalarPointer();

  fprintf(stderr,"IsoSurfDist3D_band() compute \n");

  for(i=first_band;i<=last_band;i++) {

    nbp = narrowband[i];
    inPtr  = inPtr0+nbp;

    val0 = *inPtr-threshold;
    sign = (val0>0); 
    grad_computed = FALSE;

    for(n=0;n<=2;n++) {

      inPtr1 = inPtr+displace[n];
      val1 = *inPtr1-threshold;
      neigh_sign =  (val1>0);

      if (sign != neigh_sign) {

    if (!grad_computed) {
      // gradient estimation
      Grad[0] = (*(inPtr+1)  - *inPtr)/vs[0];
      Grad[1] = (*(inPtr+tx) - *inPtr)/vs[1];
      Grad[2] = (*(inPtr+txy)- *inPtr)/vs[2];

      // Normalization
      norm = sqrt(Grad[0]*Grad[0]+Grad[1]*Grad[1]+Grad[2]*Grad[2]);
      grad_computed = TRUE;
    }
      

    if (sign)
      diff = val0-val1;
    else
      diff = val1-val0;

        if (diff<EPSILON) {
      fprintf(stderr," %d %d %d %d  %f diff< %f \n",x,y,z,n,diff,
          EPSILON);
      continue;
    }


    if (norm<EPSILON)
      fprintf(stderr, " %d %d %d norm=%f \n",x,y,z,norm);


    if (norm>EPSILON) {
          val = fabs(Grad[n])*vs[n]/norm/diff;

      val0_new = val0*val;
      val1_new = val1*val;

      outPtr  =  outPtr0+nbp;
      if (fabs(val0_new)<fabs(*outPtr)) {
        *outPtr = val0_new;
        p = nbp;
        if (list_elts[p].GetTrack()==-1) 
          list0[list0_size++] = p;
        list_elts[nbp].Init(val0_new*Grad[0]/norm,
                  val0_new*Grad[1]/norm,
                  val0_new*Grad[2]/norm,
                  POINT_SET_FRONT,
                  p);
      } // end if
      
      outPtr  +=  displace[n];
      if (fabs(val1_new)<fabs(*outPtr)) {
        *outPtr  = val1_new;
            p = nbp+displace[n];
        if (list_elts[p].GetTrack()==-1) list0[list0_size++] = p;
        list_elts[p].Init(val1_new*Grad[0]/norm,
                  val1_new*Grad[1]/norm,
                  val1_new*Grad[2]/norm,
                  POINT_SET_FRONT,
                  p);
      } // end if
    } else
      fprintf(stderr,"Func_IsoSurfDist3D() \t norm<=EPSILON \n");

      } // end if  (sign != sign_neigh)
    } // end for n


  } // end for i, parsing narrow band

  fprintf(stderr,"IsoSurfDist3D_band() end \n");

} // IsoSurfDist3D_band()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::IsoSurfDistInit( )
{

  register float*   inPtr;
  register float*   outPtr;
  register int      i,p;

  inPtr  = (float*) inputImage ->GetScalarPointer();
  outPtr = (float*) outputImage->GetScalarPointer();

  //  if (narrowband==NULL) {
    for (i=0; i<imsize; i++) {
      if (*inPtr > threshold)      
        *outPtr = maxdist;
      else
        if (*inPtr < threshold)      
          *outPtr = mindist;
        else                   
          *outPtr = 0;
      inPtr++;
      outPtr++;
    }
    /*
  } else {
    for(i=0;i<bandsize;i++) {
      p = narrowband[i];
      if (inPtr[p] > threshold)      
        outPtr[p] = maxdist;
      else
      if (inPtr[p] < threshold)      
        outPtr[p] = mindist;
      else                   
        outPtr[p] = 0;
    }
  }
    */

} // IsoSurfDistInit( )



//----------------------------------------------------------------------
void vtkImagePropagateDist2::PropagateDanielsson2D( )
{

    // 0: know values in the front
    // 1: value to compute in the front  
    register float     dx0,dy0;
    register float     dx,dy;
    register int       n[8];
    register int       nx[8];
    register int       ny[8];
    register int       l;
    register int       tp,px,py;
    register int       p,k,pn,size;

    int                x0,y0,x1,y1;
    int                xpn0,ypn0,tpn;
    float                dxp,dyp;
    float                dxpn,dypn;
    int                i,j;
    register float known_dist_pos;
    register float known_dist_neg;
    register float next_dist_pos;
    register float next_dist_neg;
    register float step_dist;

    int       iteration;
    char      name[100];
    float     val_min_pos,val_max_pos;
    float     val_min_neg,val_max_neg;
    float     val_min;
    register float     val;
    register float     val0;

    int posupdated;
    int negupdated;

    PD_element2 pt0;
    float* buf;
    float* bufn;
    int distmap_count = 0;


    //  PreComputeDistanceArray();

    //  memcpy(outputImage->GetScalarPointer(),
    //     inputImage->GetScalarPointer(),
    //     imsize*sizeof(float));

  buf    = (float*) outputImage ->GetScalarPointer();

  //------- initialize neighbors
  l = 0;
  for(i=-1;i<=1;i+=1)
    for(j=-tx;j<=tx;j+=tx)
      {
    if ((i==0)&&(j==0)) continue;
        ((i<0)?nx[l]=-1:((i>0)?nx[l]=1:nx[l]=0));
        ((j<0)?ny[l]=-1:((j>0)?ny[l]=1:ny[l]=0));
    n[l]=i+j;
    l++;
      }


  if (mindist>0) mindist = 0;
  if (maxdist<0) maxdist = 0;

  step_dist  = 0.7;
  known_dist_pos = 0;
  known_dist_neg = 0;

  //--- First list list0 already created by IsoSurfDist2D

  iteration = 0;

  // Already done ...
  //  for(k=0;k<list0_size;k++) list_elts[list0[k]].SetState(POINT_SET_FRONT);
  known_dist_pos = 0;
  known_dist_neg = 0;

  printf("list0_size = %5d \n",list0_size);

  SaveDistance(      distmap_count);
  SaveState(         distmap_count);
  SaveProjection(    distmap_count++);

    //  SaveTrajectories2D(distmap_count++);

  list_remaining_trial_size = 0;

  while ((known_dist_pos<maxdist)||(known_dist_neg>mindist)) {

    val_min_pos = maxdist; 
    val_min_neg = 1;
    val_max_pos = -1;
    val_max_neg = mindist;

    iteration++;
    //    printf("known dist [%3.3f,%3.3f] \n",known_dist_neg,known_dist_pos);
    if (known_dist_pos<maxdist) next_dist_pos = known_dist_pos+step_dist;
    if (known_dist_neg>mindist) next_dist_neg = known_dist_neg-step_dist;

    //------ Create the trial points
    list1_size = 0;

    // Put the remaining trial points in the list
    for(k=0;k<list_remaining_trial_size;k++) {
      p = list_remaining_trial[k];
      list1[list1_size++]=p;
      list_elts[p].SetState(POINT_TRIAL_INLIST);
    }
    list_remaining_trial_size = 0;

    for(k=0;k<list0_size;k++) {
      p    = list0[k];
      pt0  = list_elts[p];
      tp   = pt0.GetTrack();
      val0 = buf[tp];

      if (buf[p]>0) {
    val_min_pos = min(val_min_pos,buf[p]);
    val_max_pos = max(val_max_pos,buf[p]);
      } else {
    val_min_neg = min(val_min_neg,buf[p]);
    val_max_neg = max(val_max_neg,buf[p]);
      }


      x0 = p%tx;
      y0 = ((p-x0)/tx)%ty;

      dxp = pt0.X();
      dyp = pt0.Y();

      // Check the neighbors for trial
      for(l=0;l<8;l++) {

    // checking the scalar product
    // I should check the equivalence ...
    //        if ((dxp*nx[l]+dyp*ny[l])<0) continue;

    pn = p+n[l];
        x1 = x0+nx[l];
        y1 = y0+ny[l];

        // Check the limits
        if ((x1>=0)&&(x1<tx)&&(y1>=0)&&(y1<ty))
      {

      PD_element2& neighbor = list_elts[pn];

          switch (neighbor.GetState()) {
      case POINT_NOT_PARSED:
            neighbor.SetState(POINT_TRIAL);
      case POINT_TRIAL:
        list1[list1_size++]=pn;
            neighbor.SetState(POINT_TRIAL_INLIST);
      case POINT_TRIAL_INLIST:
        // Update here the values of the trial points
        dx = pt0.X()+nx[l];
        dy = pt0.Y()+ny[l];
        bufn = buf+pn;
        // Positive side
        if (*bufn>0) {
          val = sqrt(dx*dx+dy*dy);
          if (val<*bufn) {
        if (val<known_dist_pos) {
          printf("val<known_dist_pos; val0=%f val=%f (dx,dy)=(%3.2f,%3.2f)\n",val0, val,dx,dy);
          printf("point (%3d,%3d) \n",x1,y1);
        }
        *bufn = val;
        neighbor.SetPosTrack( dx,dy,0, tp);
                neighbor.SetPrevNeighbor(l);
          }
        } else
        // Negative side
        if (*bufn<0) {
          val = - sqrt(dx*dx+dy*dy);
          if (val>*bufn) {
        if (val>known_dist_neg) {
          printf("val>known_dist_neg; val0=%f val=%f (dx,dy)=(%3.2f,%3.2f)\n",val0,val,dx,dy);
          printf("point (%3d,%3d) \n",x1,y1);
        }
        *bufn = val;
        neighbor.SetPosTrack( dx,dy,0, tp);
                neighbor.SetPrevNeighbor(l);
          }
        } // end if
        else
          fprintf(stderr," *bufn == 0 ! \n");
        break;
        
      case POINT_SET_FRONT:
      case POINT_SET:
        // check for skeleton ...
        if ((buf[p]<0)&&
        (buf[pn]<0)&&
        (!pt0.GetSkeleton())&&
        (!neighbor.GetSkeleton())) 
          {
        dxpn = neighbor.X();
        dypn = neighbor.Y();
        if ((dxp*dxpn+dyp*dypn)<0) {
          if (buf[p]<buf[pn])
            list_elts[p].SetSkeleton(1);
          else
            list_elts[pn].SetSkeleton(1);
        }
          } 

      } // end switch

    } // if pn in [0,size-1]
      } // for l in [0,7]
    } // for k in [0,list0_size-1]


    SaveDistance(      distmap_count);
    SaveState(         distmap_count);
    SaveProjection(    distmap_count++);
    //    SaveTrajectories2D(distmap_count++);

     printf("pos [%2.2f, %2.2f]  neg [%2.2f, %2.2f] \n",
         val_min_pos,val_max_pos,
          val_min_neg,val_max_neg
          );

    for(k=0;k<list0_size;k++)  list_elts[list0[k]].SetState( POINT_SET);

    // First iteration, we complete list0 ...
    if (iteration>1) list0_size = 0;



    known_dist_pos = next_dist_pos;
    known_dist_neg = next_dist_neg;

    //    val_min = maxdist;
    list_remaining_trial_size = 0;
    for(k=0;k<list1_size;k++) {
      p = list1[k];
      // update the list of known points in the front
      if (((buf[p]>0)&&(buf[p]<=known_dist_pos)&&(known_dist_pos<maxdist))||
          ((buf[p]<0)&&(buf[p]>=known_dist_neg)&&(known_dist_neg>mindist))) {
    list0[list0_size++] = p;
    list_elts[p].SetState( POINT_SET_FRONT);
    //    if (fabs(buf[p])<val_min)
    //      val_min = fabs(buf[p]);
      }
      else 
    if (((buf[p]>0)&&(known_dist_pos<maxdist))||
        ((buf[p]<0)&&(known_dist_neg>mindist)))
      {
        list_remaining_trial[list_remaining_trial_size++] = p;
        list_elts[p].SetState( POINT_TRIAL);
      }
    }

    SaveDistance(      distmap_count);
    SaveState(         distmap_count);
    SaveProjection(    distmap_count++);

    //    printf("list0_size = %5d  min = %f, borne max = %f \n",
    //       list0_size, val_min, known_dist_pos+step_dist);

    

  } // end while

  //  save_intermediate_images = 1;
  //  SaveProjection(0);
  //  SaveSkeleton(0);


  //  FreeDistanceArray();

  fprintf(stderr,"vtkImagePropagateDist2::PropagateDanielsson2D() end \n");

} // PropagateDanielsson2D()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::PreComputeDistanceArray()
{
  int i;
  int x,y,z;
  float maxd;

  maxd = (maxdist>-mindist?maxdist:-mindist);

  //------ Precomputes distances
  sq_size = (int) (maxd+2); 
  printf("sq size = %d \n",sq_size);
  sq = new int[sq_size];
  for(i=0;i<sq_size;i++) sq[i]=i*i;

  sqroot_size = 3*sq_size*sq_size;
  sqroot = new double[sqroot_size];
  printf("sqroot size = %d \n",sqroot_size);
  for(i=0;i<sqroot_size;i++)  sqroot[i] = sqrt((double)i);

  distance = new double**[sq_size];
  for(x=0;x<sq_size;x++) distance[x] = new double*[sq_size];
  for(x=0;x<sq_size;x++)
    for(y=0;y<sq_size;y++) distance[x][y] = new double[sq_size];

  for(x=0;x<sq_size;x++)
    for(y=0;y<sq_size;y++) 
      for(z=0;z<sq_size;z++) 
    distance[x][y][z] = sqroot[sq[x]+sq[y]+sq[z]];

}

//----------------------------------------------------------------------
void vtkImagePropagateDist2::FreeDistanceArray()
{
  int x,y;

  delete [] sq;
  delete [] sqroot;

  for(x=0;x<sq_size;x++)
    for(y=0;y<sq_size;y++) delete [] distance[x][y];
  for(x=0;x<sq_size;x++) delete [] distance[x];
  delete [] distance;

} // FreeDistanceArray()

//----------------------------------------------------------------------
void vtkImagePropagateDist2::InitLists()
{

  fprintf(stderr,"vtkImagePropagateDist2::InitLists()\n");

  //------ Allocate the information

  list_maxsize = 500000;

  // list of points in the front
  if (list0==NULL)  list0 = new int[list_maxsize]; 

  // list of trial points
  if (list1==NULL)  list1 = new int[list_maxsize]; 

  // list of trial points
  if (list_remaining_trial==NULL)  
    list_remaining_trial = new int[list_maxsize]; 

  // Use a lot of memory: to be improved ...
  if (list_elts==NULL)  
    list_elts = new PD_element2[imsize];
  else
    // lent ... mais ok pour l'instant !
    for(int i=0;i<imsize;i++) {
      list_elts[i].Init(0,0,0,POINT_NOT_PARSED,-1);
      list_elts[i].SetPrevNeighbor(-1);
      list_elts[i].SetSkeleton(0);
    }

  list0_size = 0;
  list1_size = 0;
  list_remaining_trial_size = 0;

} // InitLists()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::FreeLists()
{

  //------ Allocate the information

  if (list0!=NULL) {
    delete [] list0;
    list0 = NULL;
  }

  if (list1!=NULL) {
    delete [] list1;
    list1 = NULL;
  }

  if (list_remaining_trial!=NULL) {
    delete [] list_remaining_trial;
    list_remaining_trial = NULL;
  }

  if (list_elts!=NULL) {
    delete [] list_elts;
    list_elts = NULL;
  }


} // FreeLists()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::PropagateDanielsson3D( )
{

    // 0: know values in the front
    // 1: value to compute in the front  
    register float     dx0,dy0,dz0;
    register float     dx,dy,dz;
    register int       n[26];
    register int       nx[26];
    register int       ny[26];
    register int       nz[26];
    register int       l;
    register int       tp,px,py,pz;
    register int       p,k,pn,size;
    register float     ps;

    int                x0,y0,z0,x1,y1,z1,p0;
    int                xpn0,ypn0,zpn0,tpn;
    float                dxp,dyp,dzp;
    float                dxpn,dypn,dzpn;
    int                i,j;
    register float known_dist_pos;
    register float known_dist_neg;
    register float next_dist_pos;
    register float next_dist_neg;
    register float step_dist;

    int       iteration;
    char      name[100];
    float     val_min_pos,val_max_pos;
    float     val_min_neg,val_max_neg;
    float     val_min;
    register float     val;
    register float     val0;

    int posupdated;
    int negupdated;

    PD_element2 pt0;
    float* buf;
    float* bufn;
    int distmap_count = 0;


    //  PreComputeDistanceArray();

    //  memcpy(outputImage->GetScalarPointer(),
    //     inputImage->GetScalarPointer(),
    //     imsize*sizeof(float));

  buf    = (float*) outputImage ->GetScalarPointer();

  //------- initialize neighbors
  l = 0;
  for(i=-1;i<=1;i+=1)
    for(j=-tx;j<=tx;j+=tx)
      for(k=-txy;k<=txy;k+=txy) {
    if ((i==0)&&(j==0)&&(k==0)) continue;
        ((i<0)?nx[l]=-1:((i>0)?nx[l]=1:nx[l]=0));
        ((j<0)?ny[l]=-1:((j>0)?ny[l]=1:ny[l]=0));
        ((k<0)?nz[l]=-1:((k>0)?nz[l]=1:nz[l]=0));
    n[l]=i+j+k;
    l++;
      }

  if (mindist>0) mindist = 0;
  if (maxdist<0) maxdist = 0;

  step_dist  = 0.6;
  known_dist_pos = 0;
  known_dist_neg = 0;

  //--- First list list0 already created by IsoSurfDist3D

  iteration = 0;

  // Already done ...
  //  for(k=0;k<list0_size;k++) list_elts[list0[k]].SetState(POINT_SET_FRONT);
  known_dist_pos = 0;
  known_dist_neg = 0;

  SaveDistance(      distmap_count);
  SaveState(         distmap_count);
  SaveTrajectories3D(distmap_count++);

  list_remaining_trial_size = 0;

  while ((known_dist_pos<maxdist)||(known_dist_neg>mindist)) {

    val_min_pos = maxdist; 
    val_min_neg = 1;
    val_max_pos = -1;
    val_max_neg = mindist;

    iteration++;
    printf("known dist [%3.3f,%3.3f] \n",known_dist_neg,known_dist_pos);
    if (known_dist_pos<maxdist) next_dist_pos = known_dist_pos+step_dist;
    if (known_dist_neg>mindist) next_dist_neg = known_dist_neg-step_dist;

    //------ Create the trial points
    list1_size = 0;

    // Put the remaining trial points in the list
    for(k=0;k<list_remaining_trial_size;k++) {
      p = list_remaining_trial[k];
      list1[list1_size++]=p;
      list_elts[p].SetState(POINT_TRIAL_INLIST);
    }
    list_remaining_trial_size = 0;

    for(k=0;k<list0_size;k++) {
      p    = list0[k];
      pt0  = list_elts[p];
      tp   = pt0.GetTrack();
      val0 = buf[tp];

      if (buf[p]>0) {
    val_min_pos = min(val_min_pos,buf[p]);
    val_max_pos = max(val_max_pos,buf[p]);
      } else {
    val_min_neg = min(val_min_neg,buf[p]);
    val_max_neg = max(val_max_neg,buf[p]);
      }

      x0 = p%tx;
      p0 = (p-x0)/tx;
      y0 = p0%ty;
      z0 = (p0-y0)/ty;

      dxp = pt0.X();
      dyp = pt0.Y();
      dzp = pt0.Z();

      // Check the neighbors for trial
      for(l=0;l<26;l++) {

    //        if ((dxp*nx[l]+dyp*ny[l]+dzp*nz[l])<0) continue;

    pn = p+n[l];
    x1 = x0+nx[l];
    y1 = y0+ny[l];
    z1 = z0+nz[l];

        // Check the limits
        if ((x1>=0)&&(x1<tx)&&
            (y1>=0)&&(y1<ty)&&
            (z1>=0)&&(z1<tz))
      {

        PD_element2& neighbor = list_elts[pn];

        if (neighbor.GetTrack()==tp) continue;


          switch (neighbor.GetState()) {
      case POINT_NOT_PARSED:
        neighbor.SetState(POINT_TRIAL);
      case POINT_TRIAL:
        list1[list1_size++]=pn;
            neighbor.SetState(POINT_TRIAL_INLIST);
      case POINT_TRIAL_INLIST:
        dx = pt0.X()+nx[l];
        dy = pt0.Y()+ny[l];
        dz = pt0.Z()+nz[l];
        bufn = buf+pn;
        // Positive side
        if (*bufn>0) {
          val =  sqrt(dx*dx+dy*dy+dz*dz);
          if (val<*bufn) {
        if (val<known_dist_pos) {
          printf("val<known_dist_pos; val0=%f val=%f (dx,dy,dz)=(%d,%d,%d)\n",val0, val,dx,dy,dz);
          printf("point (%3d,%3d,%3d) \n",x1,y1,z1);
        }
        *bufn = val;
        neighbor.SetPosTrack( dx,dy,dz, tp);
                neighbor.SetPrevNeighbor(l);
          }
        } else
        // Negative side
        if (*bufn<0) {
          val =  - sqrt(dx*dx+dy*dy+dz*dz);
          if (val>*bufn) {
        if (val>known_dist_neg) {
          printf("val>known_dist_neg; val0=%f val=%f (dx,dy,dz)=(%d,%d,%d)\n",val0,val,dx,dy,dz);
          printf("point (%3d,%3d,%3d) \n",x1,y1,z1);
        }
        *bufn = val;
        neighbor.SetPosTrack( dx,dy,dz, tp);
                neighbor.SetPrevNeighbor(l);
          }
        } // end if
        else
          fprintf(stderr," *bufn == 0 ! \n");
        break;
        
      case POINT_SET_FRONT:
      case POINT_SET:
        // check for skeleton ...
        if ((buf[p]<0)&&
        (buf[pn]<0)&&
        (!pt0.GetSkeleton())&&
        (!neighbor.GetSkeleton())) 
          {

        dxpn = neighbor.X();
        dypn = neighbor.Y();
        dzpn = neighbor.Z();
        if ((dxp*dxpn+dyp*dypn+dzp*dzpn)<0) {
          if (buf[p]<buf[pn])
            list_elts[p].SetSkeleton(1);
          else
            list_elts[pn].SetSkeleton(1);
        }
          } 

      } // end switch

    } // if pn in [0,size-1]
      } // for l in [0,25]
    } // for k in [0,list0_size-1]


    SaveDistance(      distmap_count);
    SaveState(         distmap_count);
    SaveTrajectories3D(distmap_count++);

    printf("pos [%2.2f, %2.2f]  neg [%2.2f, %2.2f] \n",
      val_min_pos,val_max_pos,
      val_min_neg,val_max_neg
      );

    for(k=0;k<list0_size;k++)  list_elts[list0[k]].SetState( POINT_SET);

    // First iteration, we complete list0 ...
    if (iteration>1) list0_size = 0;


    known_dist_pos = next_dist_pos;
    known_dist_neg = next_dist_neg;

    //    val_min = maxdist;
    list_remaining_trial_size = 0;
    for(k=0;k<list1_size;k++) {
      p = list1[k];
      // update the list of known points in the front
      if (((buf[p]>0)&&(buf[p]<=known_dist_pos)&&(known_dist_pos<maxdist))||
          ((buf[p]<0)&&(buf[p]>=known_dist_neg)&&(known_dist_neg>mindist))) {
    list0[list0_size++] = p;
    list_elts[p].SetState( POINT_SET_FRONT);
    //if (fabs(buf[p])<val_min)
    //  val_min = fabs(buf[p]);
      }
      else 
    if (((buf[p]>0)&&(known_dist_pos<maxdist))||
        ((buf[p]<0)&&(known_dist_neg>mindist)))
    {
      list_remaining_trial[list_remaining_trial_size++] = p;
      list_elts[p].SetState( POINT_TRIAL);
    }
    }

    SaveDistance(      distmap_count);
    SaveState(         distmap_count);
    SaveTrajectories3D(distmap_count++);

    //    printf("list0_size = %5d  min = %f, borne max = %f \n",
    //       list0_size, val_min, known_dist+step_dist);


  } // end while

  //  save_intermediate_images = 1;
  //  SaveProjection(0);
  //  SaveSkeleton(0);

  //  FreeDistanceArray();

} // PropagateDanielsson3D()



//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveTrajectories2D( int num)
{

  if (!save_intermediate_images) return;  

  register int       l;
  register int       nx[8];
  register int       ny[8];
  
  
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  vtkImageData* copyImage = vtkImageData::New();
  float* ptr;
  char name[255];
  int  i,j;
  
  // Initialize neighbors
  l = 0;
  for(i=-1;i<=1;i+=1)
    for(j=-tx;j<=tx;j+=tx)
      {
    if ((i==0)&&(j==0)) continue;
    ((i<0)?nx[l]=-1:((i>0)?nx[l]=1:nx[l]=0));
    ((j<0)?ny[l]=-1:((j>0)?ny[l]=1:ny[l]=0));
    l++;
      }
  
  copyImage->SetScalarType( VTK_FLOAT);
  copyImage->SetNumberOfScalarComponents(1);
  copyImage->SetDimensions( outputImage->GetDimensions());
  copyImage->SetOrigin(     outputImage->GetOrigin());
  copyImage->SetSpacing(    outputImage->GetSpacing());
  
  copyImage->AllocateScalars();
  //     copyImage->CopyAndCastFrom(outputImage,
  //                outputImage->GetExtent());

  // Save X component  
  ptr = (float*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    if (list_elts[i].GetPrevNeighbor()!=-1)
      *ptr = nx[list_elts[i].GetPrevNeighbor()];
    else
      *ptr = 0;
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"trajectory%d_X.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  // Save Y component  
  ptr = (float*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    if (list_elts[i].GetPrevNeighbor()!=-1)
      *ptr = ny[list_elts[i].GetPrevNeighbor()];
    else
      *ptr = 0;
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"trajectory%d_Y.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();


  copyImage->Delete();
  writer->Delete();

  fprintf(stderr,"%s saved \n",name);

} // SaveTrajectories2D()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveTrajectories3D( int num)
{

  if (!save_intermediate_images) return;  

  register int       l;
  register int       nx[26];
  register int       ny[26];
  register int       nz[26];
  
  
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  vtkImageData* copyImage = vtkImageData::New();
  float* ptr;
  char name[255];
  int  i,j,k;
  
  // Initialize neighbors
  l = 0;
  for(i=-1;i<=1;i+=1)
    for(j=-tx;j<=tx;j+=tx)
      for(k=-txy;k<=txy;k+=txy) {
    if ((i==0)&&(j==0)&&(k==0)) continue;
        ((i<0)?nx[l]=-1:((i>0)?nx[l]=1:nx[l]=0));
        ((j<0)?ny[l]=-1:((j>0)?ny[l]=1:ny[l]=0));
        ((k<0)?nz[l]=-1:((k>0)?nz[l]=1:nz[l]=0));
    l++;
      }
  
  copyImage->SetScalarType( VTK_FLOAT);
  copyImage->SetNumberOfScalarComponents(1);
  copyImage->SetDimensions( outputImage->GetDimensions());
  copyImage->SetOrigin(     outputImage->GetOrigin());
  copyImage->SetSpacing(    outputImage->GetSpacing());
  
  copyImage->AllocateScalars();
  //     copyImage->CopyAndCastFrom(outputImage,
  //                outputImage->GetExtent());

  // Save X component  
  ptr = (float*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    if (list_elts[i].GetPrevNeighbor()!=-1)
      *ptr = nx[list_elts[i].GetPrevNeighbor()];
    else
      *ptr = 0;
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"trajectory%d_X.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  // Save Y component  
  ptr = (float*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    if (list_elts[i].GetPrevNeighbor()!=-1)
      *ptr = ny[list_elts[i].GetPrevNeighbor()];
    else
      *ptr = 0;
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"trajectory%d_Y.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  // Save Z component  
  ptr = (float*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    if (list_elts[i].GetPrevNeighbor()!=-1)
      *ptr = nz[list_elts[i].GetPrevNeighbor()];
    else
      *ptr = 0;
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"trajectory%d_Z.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();


  copyImage->Delete();
  writer->Delete();

  fprintf(stderr,"%s saved \n",name);

} // SaveTrajectories3D()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveProjection( int num)
{

  if (!save_intermediate_images) return;  

  
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  vtkImageData* copyImageX = vtkImageData::New();
  vtkImageData* copyImageY = vtkImageData::New();
  vtkImageData* copyImageZ = vtkImageData::New();
  float* ptrX;
  float* ptrY;
  float* ptrZ;
  char name[255];
  int  i,j,k,track;
  int  x,y,z,p;
  int  x0,y0,z0,p0;
  
  
  copyImageX->SetScalarType( VTK_FLOAT);
  copyImageX->SetNumberOfScalarComponents(1);
  copyImageX->SetDimensions( outputImage->GetDimensions());
  copyImageX->SetOrigin(     outputImage->GetOrigin());
  copyImageX->SetSpacing(    outputImage->GetSpacing());
  copyImageX->AllocateScalars();

  copyImageY->SetScalarType( VTK_FLOAT);
  copyImageY->SetNumberOfScalarComponents(1);
  copyImageY->SetDimensions( outputImage->GetDimensions());
  copyImageY->SetOrigin(     outputImage->GetOrigin());
  copyImageY->SetSpacing(    outputImage->GetSpacing());
  copyImageY->AllocateScalars();

  copyImageZ->SetScalarType( VTK_FLOAT);
  copyImageZ->SetNumberOfScalarComponents(1);
  copyImageZ->SetDimensions( outputImage->GetDimensions());
  copyImageZ->SetOrigin(     outputImage->GetOrigin());
  copyImageZ->SetSpacing(    outputImage->GetSpacing());
  copyImageZ->AllocateScalars();


  ptrX = (float*) copyImageX->GetScalarPointer();
  ptrY = (float*) copyImageY->GetScalarPointer();
  ptrZ = (float*) copyImageZ->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    *ptrX = -list_elts[i].X();
    *ptrY = -list_elts[i].Y();
    *ptrZ = -list_elts[i].Z();
    
    ptrX++;
    ptrY++;
    ptrZ++;
  }
  
  writer->SetInput(copyImageX);
  sprintf(name,"projection%d_X.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  writer->SetInput(copyImageY);
  sprintf(name,"projection%d_Y.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  writer->SetInput(copyImageZ);
  sprintf(name,"projection%d_Z.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  copyImageX->Delete();
  copyImageY->Delete();
  copyImageZ->Delete();
  writer->Delete();

} // SaveProjection()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveState( int num)
{

  if (!save_intermediate_images) return;  

  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  vtkImageData* copyImage = vtkImageData::New();
  unsigned char* ptr;
  char name[255];
  int  i,j;
  
  copyImage->SetScalarType( VTK_UNSIGNED_CHAR);
  copyImage->SetNumberOfScalarComponents(1);
  copyImage->SetDimensions( outputImage->GetDimensions());
  copyImage->SetOrigin(     outputImage->GetOrigin());
  copyImage->SetSpacing(    outputImage->GetSpacing());
  
  copyImage->AllocateScalars();
  //     copyImage->CopyAndCastFrom(outputImage,
  //                outputImage->GetExtent());

  // Save state
  ptr = (unsigned char*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    *ptr = list_elts[i].GetState();
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"State%d.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  copyImage->Delete();
  writer->Delete();


} // SaveState()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveSkeleton( int num)
{

  if (!save_intermediate_images) return;  

  
  vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
  vtkImageData* copyImage = vtkImageData::New();
  unsigned char* ptr;
  char name[255];
  int  i,j;
  
  
  copyImage->SetScalarType( VTK_UNSIGNED_CHAR);
  copyImage->SetNumberOfScalarComponents(1);
  copyImage->SetDimensions( outputImage->GetDimensions());
  copyImage->SetOrigin(     outputImage->GetOrigin());
  copyImage->SetSpacing(    outputImage->GetSpacing());
  
  copyImage->AllocateScalars();
  //     copyImage->CopyAndCastFrom(outputImage,
  //                outputImage->GetExtent());

  // Save skeleton
  ptr = (unsigned char*) copyImage->GetScalarPointer();
  for(i=0;i<imsize;i++) {
    *ptr = list_elts[i].GetSkeleton();
    ptr++;
  }
  
  writer->SetInput(copyImage);
  sprintf(name,"Skeleton%d.vtk",num);
  writer->SetFileName(name);
  writer->SetFileTypeToBinary();
  writer->Write();

  fprintf(stderr,"%s saved \n",name);

  copyImage->Delete();
  writer->Delete();


} // SaveSkeleton()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::SaveDistance( int num)
{

  if (!save_intermediate_images) return;  

    vtkStructuredPointsWriter *writer = vtkStructuredPointsWriter::New();
    vtkImageData* copyImage = vtkImageData::New();
    float* ptr;
    char name[255];
    int  i;
    float* buf;

    buf    = (float*) outputImage ->GetScalarPointer();
    
    copyImage->SetScalarType( VTK_FLOAT);
    copyImage->SetNumberOfScalarComponents(1);
    copyImage->SetDimensions( outputImage->GetDimensions());
    copyImage->SetOrigin(     outputImage->GetOrigin());
    copyImage->SetSpacing(    outputImage->GetSpacing());
    
    copyImage->AllocateScalars();
    //     copyImage->CopyAndCastFrom(outputImage,
    //                outputImage->GetExtent());
    
    ptr = (float*) copyImage->GetScalarPointer();
    for(i=0;i<imsize;i++) {
      *ptr = buf[i];
      ptr++;
    }
    
    writer->SetInput(copyImage);
    sprintf(name,"distmap%d.vtk",num);
    writer->SetFileName(name);
    writer->SetFileTypeToBinary();
    writer->Write();

    copyImage->Delete();
    writer->Delete();


    fprintf(stderr,"%s saved \n",name);

} // SaveDistance()


//----------------------------------------------------------------------------
void vtkImagePropagateDist2::SetNarrowBand( int* band, int size)
{
  narrowband = band;
  bandsize = size;
}

//----------------------------------------------------------------------
void vtkImagePropagateDist2::GetSkeleton( vtkImageData* skeleton)
{

  float* ptr;
  char name[255];
  int  i,j;
  float n;
  
  
  if (skeleton->GetScalarType() != VTK_FLOAT) {
    fprintf(stderr,"vtkImagePropagateDist2::GetSkeleton() skeleton image must be VTK_FLOAT \n");
  }

  // Save skeleton
  ptr = (float*) skeleton->GetScalarPointer();
  for(i=0;i<imsize;i++) {
     n = list_elts[i].GetSkeleton();
     ptr[i] = n;
  }
  

} // GetSkeleton()


//----------------------------------------------------------------------
void vtkImagePropagateDist2::PrintSelf(ostream& os, vtkIndent indent)
{
   vtkImageToImageFilter::PrintSelf(os,indent);

} // PrintSelf()
