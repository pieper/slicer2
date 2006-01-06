/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkThinning.cxx,v $
  Date:      $Date: 2006/01/06 17:57:24 $
  Version:   $Revision: 1.7 $

=========================================================================auto=*/

#include "vtkThinning.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkPointData.h"

//
//  Code by Karl Krissian
//  University of Las Palmas of Gran Canaria
//  and Surgical Planning Lab, BWH
//  transferred to VTK by Arne Hans
//  Surgical Planning Lab
//  Brigham and Women's Hospital
//


#define FALSE    0
#define TRUE     1




//-----------------------------------------------------------------------
vtkThinning* vtkThinning::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkThinning");
  if(ret)
    {
    return (vtkThinning*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkThinning;

} // vtkThinning::New()


//----------------------------------------------------------------------
// Construct object to extract all of the input data.
//
vtkThinning::vtkThinning()
{

  input_image   = NULL;
  OutputImage  = NULL;
  Criterion     = NULL;

  MinThreshold = 0;
  MaxThreshold = 1000;
  
  UseLineEndpoints = TRUE;
  UseFiducialEndpoints = FALSE;
  UseSurfaceEndpoints = FALSE;

} // vtkThinning::vtkThinning()


//----------------------------------------------------------------------
// Construct object to extract all of the input data.
//
vtkThinning::~vtkThinning()
{
  if (input_image) input_image->Delete();
  //if (OutputImage) OutputImage->Delete();

} // vtkThinning::~vtkThinning()


//----------------------------------------------------------------------
void vtkThinning::Init()
{

  if (this->GetInput() == NULL) {
    vtkErrorMacro("Missing input");
    return;
  }
  else {

    //input_image = new_image(this->GetInput());
    input_image = vtkImageData::New();
    input_image->SetDimensions(this->GetInput()->GetDimensions());
    input_image->SetSpacing(this->GetInput()->GetSpacing());
    input_image->SetScalarType(VTK_UNSIGNED_SHORT);
    input_image->SetNumberOfScalarComponents(1);
    input_image->AllocateScalars();
    input_image->DeepCopy(this->GetInput());
    
    
    tx = input_image->GetDimensions()[0];
    ty = input_image->GetDimensions()[1];
    tz = input_image->GetDimensions()[2];
    txy = tx*ty;
    
    //--- OutputImage

    /*
    output=this->GetOutput();
    output->SetDimensions(this->GetInput()->GetDimensions() );
    output->SetSpacing(   this->GetInput()->GetSpacing() );
    output->SetScalarType(VTK_UNSIGNED_SHORT); 
    output->SetNumberOfScalarComponents(1);
    output->AllocateScalars();*/

    
    //OutputImage  = new_image(this->GetOutput());
    OutputImage = this->GetOutput();
    OutputImage->SetDimensions(input_image->GetDimensions());
    OutputImage->SetSpacing(input_image->GetSpacing());
    OutputImage->SetScalarType(VTK_UNSIGNED_SHORT);
    OutputImage->SetNumberOfScalarComponents(1);
    OutputImage->AllocateScalars();
    //OutputImage->DeepCopy(input_image);
  }
    

} // vtkThinning::InitParam()






//----------------------------------------------------------------------
void vtkThinning::Thin_init_pos( )
//                -------------
{

  int i,j,k,n;

  for(i=0;i<=2;i++) {
    for(j=0;j<=2;j++) {
      for(k=0;k<=2;k++) {

        n                   = i+j*3+k*9;
        pos[i][j][k]        = n;
        neighbors_pos[n]    = i-1+((j-1)+(k-1)*ty)*tx;
        neighbors_place[n][0] = i-1;
        neighbors_place[n][1] = j-1;
        neighbors_place[n][2] = k-1;

      }
    }
  }

} // Thin_init_pos()


//----------------------------------------------------------------------
void vtkThinning::init_neighborhoods()
//                ------------------
{

  
    int i,j,k;
    int i1,j1,k1;
    int n,nb1,nb2;
    Boolean n1_ok,n2_ok;

  for(i=0;i<=2;i++) {
  for(j=0;j<=2;j++) {
  for(k=0;k<=2;k++) {

    n  = pos[i][j][k];
    nb1 = 0;
    nb2 = 0;

    n1_ok = ((i!=1)||(j!=1)||(k!=1));
    n2_ok = ((i==1)||(j==1)||(k==1));

    for(i1=-1;i1<=1;i1++) {
    for(j1=-1;j1<=1;j1++) {
    for(k1=-1;k1<=1;k1++) {

      if (i+i1<0) continue;
      if (j+j1<0) continue;
      if (k+k1<0) continue;

      if (i+i1>2) continue;
      if (j+j1>2) continue;
      if (k+k1>2) continue;

      if ((i1==0)&&(j1==0)&&(k1==0)) continue;
  
      if (n1_ok)
      if ((i+i1 != 1)||(j+j1 != 1)||(k+k1 != 1)) {
        nb1++;
        N26_star[n][nb1] = pos[i+i1][j+j1][k+k1];
      }

      if (n2_ok)
      if ((i+i1 == 1)||(j+j1 == 1)||(k+k1 == 1)) {
        if ( ((i1==0)&&((j1==0)||(k1==0))) || ((j1==0)&&(k1==0)) ) {
      nb2++;
      N18[n][nb2] = pos[i+i1][j+j1][k+k1];
    }
      }

    }
    }
    }

    N26_star[n][0] = nb1;
    N18     [n][0] = nb2;

  }
  }
  }

} // init_neighborhoods()


//----------------------------------------------------------------------
unsigned char vtkThinning::CoordOK(vtkImageData* im,int x,int y,int z)
{
  return (x>=0 && y>=0 && z>=0 && 
        x<im->GetDimensions()[0] &&
    y<im->GetDimensions()[1] && 
    z<im->GetDimensions()[2]);
}


//----------------------------------------------------------------------
Boolean vtkThinning::IsEndPoint(vtkImageData* im, int x, int y, int z)
//                   ----------
{
  
    int n,nb;
    int x1,y1,z1;

  if (!(*(unsigned short*)im->GetScalarPointer(x,y,z))) return FALSE;

  nb = 0;
  for(n=0;n<=26;n++) {
    x1 = x+neighbors_place[n][0];
    y1 = y+neighbors_place[n][1];
    z1 = z+neighbors_place[n][2];
    
    if (CoordOK(im,x1,y1,z1) && (*(unsigned short*)im->GetScalarPointer(x1,y1,z1))>0  ) nb++;
    if (nb>2) break;

  }

  return (nb==2);
}


//----------------------------------------------------------------------
void vtkThinning::ParseCC( int* domain, 
//                -------
               int neighborhood[27][27], 
               int* cc,
               int point, 
               int num_cc )
{

   
    int l;
    int neighbor;
    int list[27];
    int size;

  cc[point] = num_cc;
  list[0] = point;
  size = 1;

  while (size>0) {
    size--;
    point = list[size];
    
    for(l=1;l<=neighborhood[point][0];l++) {
      neighbor = neighborhood[point][l];
      if ((domain[neighbor])&&(cc[neighbor] == 0)) {
        cc[neighbor] = num_cc;
    list[size]   = neighbor;
        size++;
      }
    }
  }

} // ParseCC()




//----------------------------------------------------------------------
Boolean vtkThinning::IsSimple(vtkImageData* im, int x, int y, int z, int& cc1, int& cc2)
//                   --------
{

  int cc[27];
  int i,j,k,n,l,n1;
  int nb_cc;
  int neighbor;
  int domain[27];

  // position of the points 6-adjacents to the central point
  int six_adj[6] = {4,10,12,14,16,22};

  cc1 = cc2 = 0;

  if (!(CoordOK(im,x,y,z))) return FALSE;

  // First Check: C*(P) = 1

  nb_cc = 0;

  for(n=0;n<=26;n++) {
    cc[n] = 0;
  }

  for(i=0;i<=2;i++) {
  for(j=0;j<=2;j++) {
  for(k=0;k<=2;k++) {
    n = pos[i][j][k];
    if (CoordOK(im,x+i-1,y+j-1,z+k-1))
      domain[n] = (N26_star[n][0]) && 
              (*(unsigned short*)im->GetScalarPointer(x+i-1,y+j-1,z+k-1));
    else
      domain[n] = 0;
  }
  }
  }

  for(n=0;n<=26;n++) {
    // if the point is in the domain
    // and is not yet connected: create new connected component
    if ((domain[n])&&(cc[n] == 0)) {
      nb_cc++;
      // Parse the connected component
      ParseCC(domain,N26_star,cc,n,nb_cc);
    }
  }

  cc1 = nb_cc;
  if (nb_cc != 1) return 0;

  // Second Check: C-(P) = 1

  nb_cc = 0;

  for(n=0;n<=26;n++) {
    cc[n] = 0;
  }

  for(i=0;i<=2;i++) {
  for(j=0;j<=2;j++) {
  for(k=0;k<=2;k++) {
    n = pos[i][j][k];
    if (CoordOK(im,x+i-1,y+j-1,z+k-1))
      domain[n] = (N18[n][0]) && 
              (!(*(unsigned short*)im->GetScalarPointer(x+i-1,y+j-1,z+k-1)));
    else
      domain[n] = 0;
  }
  }
  }

  for(n=0;n<=5;n++) {
    n1 = six_adj[n];
    // if the point is in the domain
    // and is not yet connected: create new connected component
    if (( domain[n1]) && (cc[n1] == 0)) {
      nb_cc++;
      // Parse the connect component
      ParseCC(domain,N18,cc,n1,nb_cc);
    }
  }

  cc2 = nb_cc;
  return (nb_cc == 1);

} // IsSimple()





//----------------------------------------------------------------------
void vtkThinning::ExecuteData(vtkDataObject* output)
{
    vtkImageData*   im_heap;
    vtkImageData*   im_Criterion;

    int             x,y,z;
    int             x1,y1,z1;
    int             i,j,k;
    Boolean         contour;
    TrialPoint      p;
    int             it;
    char            name[100];
    int             remove_number;
    int             cc1,cc2;
    vtkMinHeap<TrialPoint>  heap;
    unsigned long   n;
    unsigned short *heapPtr,*outputPtr,*inputPtr;
 
  //fprintf(stderr,"vtkThinning execution...\n");
  
  Init();
  //fprintf(stderr,"init done...\n");

  Thin_init_pos();
  //fprintf(stderr,"init_pos done...\n");

  init_neighborhoods();
  //fprintf(stderr,"init_neighborhoods done...\n");

  im_heap = vtkImageData::New();
  im_heap->SetDimensions(this->GetInput()->GetDimensions());
  im_heap->SetSpacing(this->GetInput()->GetSpacing());
  im_heap->SetScalarType(VTK_UNSIGNED_SHORT);
  im_heap->SetNumberOfScalarComponents(1);
  im_heap->AllocateScalars();
  //fprintf(stderr,"heap image allocated...\n");

  //im_Criterion = new_image(Criterion);
  im_Criterion = Criterion;
  
  //fprintf(stderr,"criterion image allocated and copied, extent %d,%d,%d...\n",im_Criterion->GetDimensions()[0],im_Criterion->GetDimensions()[1],im_Criterion->GetDimensions()[2]);
  this->UpdateProgress(0.1);

  heapPtr = (unsigned short*)im_heap->GetScalarPointer();

  for(n=0; n<im_heap->GetPointData()->GetScalars()->GetNumberOfTuples(); n++) {
    *heapPtr = 0;
    heapPtr++;
  }


  outputPtr = (unsigned short*)OutputImage->GetScalarPointer();
  inputPtr = (unsigned short*)input_image->GetScalarPointer();

  for(n=0; n<input_image->GetPointData()->GetScalars()->GetNumberOfTuples(); n++) {

    if (*inputPtr>0)
      *outputPtr=255;
    else
      *outputPtr=0;

    outputPtr++;
    inputPtr++;
  }

  // Initialize the heap to the contour points which are simple
  for(z=1;z<=OutputImage->GetDimensions()[2]-2;z++) {
  for(y=1;y<=OutputImage->GetDimensions()[1]-2;y++) {
    outputPtr=(unsigned short*)OutputImage->GetScalarPointer(1,y,z);
    for(x=1;x<=OutputImage->GetDimensions()[0]-2;x++) {
      contour = FALSE;
      if (*outputPtr) {
        for(n=0;n<=26;n++) {
          if (!(*(outputPtr+neighbors_pos[n]))) {
            contour=TRUE;
            break;
          }
        }
      }
      if ((contour) && IsSimple(OutputImage,x,y,z,cc1,cc2) ) {
        heap += TrialPoint(x,y,z,im_Criterion->GetScalarComponentAsFloat(x,y,z,0));
    heapPtr=(unsigned short*)im_heap->GetScalarPointer(x,y,z);
    *heapPtr=1;
      }
      outputPtr++;
    }
  }
  }
  //fprintf(stderr,"heap initialized, size is %d...\n",heap.Size());
  this->UpdateProgress(0.2);


  it = 0;
  remove_number = 1;

  
  while (heap.Size()>0) {

    it++;
    //if ((it%1000==0))
    //  fprintf(stderr,"iteration %5d, heap size %5d \n",it,heap.Size());

    p = heap.GetMin();
    if (p.value > MaxThreshold) break;

    if ( IsSimple(OutputImage,p.x,p.y,p.z,cc1,cc2) ) {
      if ( !(IsEndPoint(OutputImage,p.x,p.y,p.z)) ||
         (im_Criterion->GetScalarComponentAsFloat(p.x,p.y,p.z,0) < MinThreshold)) { 
        // remove P
    outputPtr=(unsigned short*)OutputImage->GetScalarPointer(p.x,p.y,p.z);
    *outputPtr=0;

        // set im_heap to 2 to say the point has already been parsed
    heapPtr=(unsigned short*)im_heap->GetScalarPointer(p.x,p.y,p.z);
    *heapPtr=2;

    //    im_removed->BufferPos(p.x,p.y,p.z);
    //    im_removed->SetValue(remove_number);
    //        remove_number++;

    // Add neighbors to the heap
        for(n=0;n<=26;n++) {
      x1 = p.x+neighbors_place[n][0];
          y1 = p.y+neighbors_place[n][1];
          z1 = p.z+neighbors_place[n][2];
          if ( !(CoordOK(OutputImage,x1,y1,z1))) continue;
          if (*(outputPtr+neighbors_pos[n])==255 ) {
            if ( ((*(unsigned short*)im_heap->GetScalarPointer(x1,y1,z1))==0) &&
            IsSimple(OutputImage,x1,y1,z1,cc1,cc2)) {
              heap += TrialPoint(x1,y1,z1,im_Criterion->GetScalarComponentAsFloat(x1,y1,z1,0));
          heapPtr=(unsigned short*)im_heap->GetScalarPointer(x1,y1,z1);
          *heapPtr=1;
            }
          }
        }
      } else {
    outputPtr=(unsigned short*)OutputImage->GetScalarPointer(p.x,p.y,p.z);
    *outputPtr=127;
      }
    } else {
      // set im_heap to 2 to say the point has already been parsed
      heapPtr=(unsigned short*)im_heap->GetScalarPointer(p.x,p.y,p.z);
      *heapPtr=0;
    }    

  }
  this->UpdateProgress(0.9);

  im_heap->Delete();


  outputPtr=(unsigned short*)OutputImage->GetScalarPointer();

  for(z=0;z<=OutputImage->GetDimensions()[2]-1;z++) {
  for(y=0;y<=OutputImage->GetDimensions()[1]-1;y++) {
  for(x=0;x<=OutputImage->GetDimensions()[0]-1;x++) {

    if (*outputPtr && im_Criterion->GetScalarComponentAsFloat(x,y,z,0)>MaxThreshold ) {
      *outputPtr=0;
    }
    
    if (*outputPtr) {
        *outputPtr=127;
    }

    outputPtr++;
  }
  }
  }

  //fprintf(stderr,"Done!\n");
  this->UpdateProgress(1.0);
  

} // vtkThinning::Execute()




//----------------------------------------------------------------------
void vtkThinning::PrintSelf(ostream& os, vtkIndent indent)
{
  // Nothing for the moment ...
}
