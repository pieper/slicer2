#include "vtkResliceImage.h"
#include "vtkImageToImageFilter.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"

#define vtkTrilinFuncMacro(v,x,y,z,a,b,c,d,e,f,g,h)         \
        t00 =   a + (x)*(b-a);      \
        t01 =   c + (x)*(d-c);      \
        t10 =   e + (x)*(f-e);      \
        t11 =   g + (x)*(h-g);      \
        t0  = t00 + (y)*(t01-t00);  \
        t1  = t10 + (y)*(t11-t10);  \
        v   = (T)( t0 + (z)*(t1-t0));

//------------------------------------------------------------------------------
vtkResliceImage* vtkResliceImage::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkResliceImage");
  if(ret)
    {
    return (vtkResliceImage*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkResliceImage;
}

//----------------------------------------------------------------------------
vtkResliceImage::vtkResliceImage()
{
  Background = 0;
  OutSpacing[0] = OutSpacing[1] = OutSpacing[2] = 1.0;
  OutOrigin[0] = OutOrigin[1] = OutOrigin[2] = 0.0;
  OutExtent[0] = OutExtent[2] = OutExtent[4] = 0;
  OutExtent[1] = OutExtent[3] = OutExtent[5] = 1;
  TransformOutputToInput = NULL;
}
//----------------------------------------------------------------------------
void vtkResliceImage::SetOutputImageParam(vtkImageData *VolumeToCopyParam)
{
  VolumeToCopyParam->GetSpacing(this->OutSpacing);
  VolumeToCopyParam->GetOrigin(this->OutOrigin);
  VolumeToCopyParam->GetWholeExtent(this->OutExtent);
}
//----------------------------------------------------------------------------
vtkMatrix4x4 *vtkResliceImage::GetIJKtoIJKMatrix(float Spacing2[3],
                                                 float Origin2[3],
                                                 vtkMatrix4x4 *MM2toMM1,
                                                 float Spacing1[3],
                                                 float Origin1[3])
{
  // The IJK to MM matrix for Coordinate System 2
  vtkMatrix4x4 *XformIJKtoMM = vtkMatrix4x4::New();

  // First Adjust for Spacing2 and Origin2
  XformIJKtoMM->Zero();
  XformIJKtoMM->Element[0][0] = Spacing2[0];
  XformIJKtoMM->Element[1][1] = Spacing2[1];
  XformIJKtoMM->Element[2][2] = Spacing2[2];

  XformIJKtoMM->Element[0][4] = Origin2[0];
  XformIJKtoMM->Element[1][4] = Origin2[1];
  XformIJKtoMM->Element[2][4] = Origin2[2];
  XformIJKtoMM->Element[3][4] = 1;

  // The MM to IJK on coordinate system 1
  vtkMatrix4x4 *XformMMtoIJK = vtkMatrix4x4::New();
  XformMMtoIJK->Zero();
  XformMMtoIJK->Element[0][0] = 1/Spacing1[0];
  XformMMtoIJK->Element[1][1] = 1/Spacing1[1];
  XformMMtoIJK->Element[2][2] = 1/Spacing1[2];

  XformMMtoIJK->Element[0][4] = Origin1[0]/Spacing1[0];
  XformMMtoIJK->Element[1][4] = Origin1[1]/Spacing1[1];
  XformMMtoIJK->Element[2][4] = Origin1[2]/Spacing1[2];
  XformMMtoIJK->Element[3][4] = 1;

  vtkMatrix4x4 *Xform2to1 = vtkMatrix4x4::New();

  // If the MM2toMM1 transform is NULL, assume the identity
  if (MM2toMM1 != NULL)
    {
      vtkMatrix4x4::Multiply4x4(MM2toMM1,XformIJKtoMM,Xform2to1);
    }
  else
    {
      Xform2to1->DeepCopy(XformIJKtoMM);
    }
  vtkMatrix4x4::Multiply4x4(XformMMtoIJK,Xform2to1,Xform2to1);

  XformIJKtoMM->Delete();
  XformMMtoIJK->Delete();
  return Xform2to1;
}
//----------------------------------------------------------------------------
inline void vtkResliceImage::FindInputIJK(float OtherIJK[4],
                                          vtkMatrix4x4 *IJKtoIJK,
                                          int i, int j, int k)
{
  float inpoint[4];
  inpoint[0] = (float) i; 
  inpoint[1] = (float) j; 
  inpoint[2] = (float) k;
  inpoint[3] = 1;
  IJKtoIJK->PointMultiply(inpoint,OtherIJK);
}

//----------------------------------------------------------------------------
void vtkResliceImage::ExecuteInformation(vtkImageData *inData, 
				      vtkImageData *outData)
{
  outData->SetOrigin(this->OutOrigin);
  outData->SetWholeExtent(this->OutExtent);
  outData->SetSpacing(this->OutSpacing);

  outData->SetNumberOfScalarComponents(inData->GetNumberOfScalarComponents());
  outData->SetScalarType(inData->GetScalarType());
}

//----------------------------------------------------------------------------
// What input should be requested.
// To be very conservative, I need the entire input range.
void vtkResliceImage::ComputeInputUpdateExtent(int InExt[6], 
                                               int OutExt[6])
{

  float InSpace[3];   this->GetInput() ->GetSpacing(InSpace);
  float OutSpace[3];  this->GetOutput()->GetSpacing(OutSpace);
  float InOrigin[3];  this->GetInput() ->GetOrigin(InOrigin);
  float OutOrigin[3]; this->GetOutput()->GetOrigin(OutOrigin);

  vtkMatrix4x4 *IJKtoIJK = 
    vtkResliceImage::GetIJKtoIJKMatrix(OutSpace,OutOrigin,
                                       this->GetTransformOutputToInput(),
                                       InSpace,InOrigin);

  // Get the wholeExtent
  int WholeExt[6];
  this->GetInput()->GetWholeExtent(WholeExt);

  InExt[0] = InExt[2] = InExt[4] = VTK_INT_MAX;
  InExt[1] = InExt[3] = InExt[5] = VTK_INT_MIN;

  float point[4],result[4];
  point[3] = 1;

  for(int i=0;i<2;i++)
    for(int j=0;j<2;j++)
      for(int k=0;k<2;k++)
        {
          point[0]=(float)OutExt[0+i];
          point[1]=(float)OutExt[2+j];
          point[2]=(float)OutExt[4+k];
          IJKtoIJK->MultiplyPoint(point,result);
          if (floor(result[0]) < InExt[0]) InExt[0] = (int)floor(result[0]);
          if (floor(result[1]) < InExt[2]) InExt[2] = (int)floor(result[1]);
          if (floor(result[2]) < InExt[4]) InExt[4] = (int)floor(result[2]);

          if (ceil(result[0]) > InExt[1]) InExt[1] = (int)ceil(result[0]);
          if (ceil(result[1]) > InExt[3]) InExt[3] = (int)ceil(result[1]);
          if (ceil(result[2]) > InExt[5]) InExt[5] = (int)ceil(result[2]);
        }
  if(InExt[0] < WholeExt[0]) InExt[0] = WholeExt[0];
  if(InExt[2] < WholeExt[2]) InExt[2] = WholeExt[2];
  if(InExt[4] < WholeExt[4]) InExt[4] = WholeExt[4];

  if(InExt[1] > WholeExt[1]) InExt[1] = WholeExt[1];
  if(InExt[3] > WholeExt[3]) InExt[3] = WholeExt[3];
  if(InExt[5] > WholeExt[5]) InExt[5] = WholeExt[5];

  IJKtoIJK->Delete();
}


//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
static void vtkResliceImageExecute(vtkResliceImage *self, int id,
                                  vtkImageData *inData, T *inPtr,
                                  int *InExt,
                                  vtkImageData *outData, T *outPtr,
                                  int *OutExt)
{
  //
  // Variables for handling the image data
  //

  int inIncX, inIncY, inIncZ;
  int outIncX, outIncY, outIncZ;

  // Get increments to march through data
  // Number of scalars had better be 1
  inData->GetIncrements(inIncX, inIncY, inIncZ);
  outData->GetIncrements(outIncX, outIncY, outIncZ);

  cout << "Input Extent: ";
  cout << InExt[0] << ' ' << InExt[1] << ' ' << InExt[2] << ' '
       << InExt[3] << ' ' << InExt[4] << ' ' << InExt[5] << '\n';

  cout << "Output Extent: ";
  cout << OutExt[0] << ' ' << OutExt[1] << ' ' << OutExt[2] << ' '
       << OutExt[3] << ' ' << OutExt[4] << ' ' << OutExt[5] << '\n';

  float InSpace[3];   inData->GetSpacing(InSpace);
  float OutSpace[3]; outData->GetSpacing(OutSpace);
  float InOrigin[3];  inData->GetOrigin(InOrigin);
  float OutOrigin[3]; outData->GetOrigin(OutOrigin);

  vtkMatrix4x4 *IJKtoIJK = 
    vtkResliceImage::GetIJKtoIJKMatrix(OutSpace,OutOrigin,
                                       self->GetTransformOutputToInput(),
                                       InSpace,InOrigin);

  float InputIJK[4];
  T *outVol1,*outVol2;
  T *InVol, *OutVol;
  OutVol = outVol2 = outVol1 = outPtr;
  T min,max;
  max = -1000;
  min = 1000;
  for(int i=OutExt[0];i<=OutExt[1];i++)
    {
      for(int j=OutExt[2];j<=OutExt[3];j++)
        {
          for(int k=OutExt[4];k<=OutExt[5];k++)
            {
              vtkResliceImage::FindInputIJK(InputIJK,IJKtoIJK,i,j,k);


              if ((InputIJK[0] >= InExt[0])&&(InputIJK[0] <= InExt[1])&&
                  (InputIJK[1] >= InExt[2])&&(InputIJK[1] <= InExt[3])&&
                  (InputIJK[2] >= InExt[4])&&(InputIJK[2] <= InExt[5]))
                {
                  int x0 = (int) floor(InputIJK[0]);
                  float x = InputIJK[0] - x0;
                  int y0 = (int) floor(InputIJK[1]);
                  float y = InputIJK[1] - y0;
                  int z0 = (int) floor(InputIJK[2]);
                  float z = InputIJK[2] - z0;
                  
                  InVol = inPtr + (x0-InExt[0])*inIncX
                    +(y0-InExt[2])*inIncY
                    +(z0-InExt[4])*inIncZ;

                  double a,b,c,d,e,f,g,h,t00,t01,t11,t10,t0,t1;
                  
                  // a = ?[x0  ][y0  ][z0  ];
                  a = (double) *InVol;
                  // b = ?[x0+1][y0  ][z0  ];
                  b = (double) *(InVol + inIncX);
                  // c = ?[x0  ][y0+1][z0  ];
                  c = (double) *(InVol          + inIncY);
                  // d = ?[x0+1][y0+1][z0  ];
                  d = (double) *(InVol + inIncX + inIncY);
                  // e = ?[x0  ][y0  ][z0+1];
                  e = (double) *(InVol                    + inIncZ);
                  // f = ?[x0+1][y0  ][z0+1];
                  f = (double) *(InVol + inIncX          + inIncZ);
                  // g = ?[x0  ][y0+1][z0+1];
                  g = (double) *(InVol +        + inIncY + inIncZ);
                  // h = ?[x0+1][y0+1][z0+1];
                  h = (double) *(InVol + inIncX + inIncY + inIncZ);
                  
                  vtkTrilinFuncMacro(*OutVol,x,y,z,a,b,c,d,e,f,g,h);

//                  cout << x << ' ' << y << ' ' << z << ' ' << '\n';
//                  cout << a << ' '
//                       << b << ' '
//                       << c << ' '
//                       << d << ' '
//                       << e << ' '
//                       << f << ' '
//                       << g << '\n';
//
                 cout << i << ' ' << j << ' ' << k << ":" 
                       << InputIJK[0] << ' ' << InputIJK[1] << ' ' 
                       << InputIJK[2] << ": ";
                  cout << *OutVol << '\n';
                }
              else
                {
                  *(OutVol) = (T) self->GetBackground();
                }
              if (*(OutVol) > max) max = *OutVol;
              if (*(OutVol) < min) min = *OutVol;
              OutVol += outIncZ;
            }
          outVol2 += outIncY;
          OutVol = outVol2;
        }
      outVol1 += outIncX;
      OutVol = outVol2 = outVol1;
    }
  cout << "min: " << min << '\n';
  cout << "max: " << max << '\n';
  IJKtoIJK->Delete();
}

/* ====================================================================== */


void vtkResliceImage::ThreadedExecute(vtkImageData *inData, 
				      vtkImageData *outData,
				      int outExt[6], int id)
{
  int inExt[6];
  this->ComputeInputUpdateExtent(inExt,outExt);
  void *inPtr = inData->GetScalarPointerForExtent(inExt);
  void *outPtr = outData->GetScalarPointerForExtent(outExt);
  
  vtkDebugMacro(<< "Execute: inData = " << inData 
  << ", outData = " << outData);
  
  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
    vtkErrorMacro(<< "Execute: input ScalarType, " << inData->GetScalarType()
    << ", must match out ScalarType " << outData->GetScalarType());
    return;
    }
  
  switch (inData->GetScalarType())
    {
    vtkTemplateMacro8(vtkResliceImageExecute, this, id, inData, 
                      (VTK_TT *)(inPtr), inExt, 
                      outData, (VTK_TT *)(outPtr), outExt);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
}

void vtkResliceImage::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkImageToImageFilter::PrintSelf(os,indent);

  os << indent << "Matrix is Null: " << 
    (this->TransformOutputToInput == NULL) << '\n';
  os << indent << "Background Scalar: " << Background << '\n';
  os << indent << "Output Spacing" 
     << OutSpacing[0] << ' ' << OutSpacing[1] << ' ' 
     << OutSpacing[2] << '\n';

  os << indent << "Output Origin: " 
     << OutOrigin[0] << ' ' << OutOrigin[1]<< ' ' 
     << OutOrigin[2] << '\n';

  os << indent << "Output Extent: " 
     << OutExtent[0] << ' ' << OutExtent[1] << ' ' << OutExtent[2] << ' '
     << OutExtent[3] << ' ' << OutExtent[4] << ' ' << OutExtent[5] << '\n';
}

