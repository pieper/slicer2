// .NAME vtkGridToLinearTransform - 
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkGridToLinearTransform_h
#define __vtkGridToLinearTransform_h

#include <vtkAGConfigure.h>

#include <vtkImageData.h>
#include <vtkLinearTransform.h>
#include <vtkGridTransform.h>

#define VTK_LANDMARK_RIGIDBODY 6
#define VTK_LANDMARK_SIMILARITY 7
#define VTK_LANDMARK_AFFINE 12

class VTK_AG_EXPORT vtkGridToLinearTransform : public vtkLinearTransform {
public:
  static vtkGridToLinearTransform* New();
  vtkTypeMacro(vtkGridToLinearTransform,vtkLinearTransform);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetObjectMacro(GridTransform,vtkGridTransform);
  vtkGetObjectMacro(GridTransform,vtkGridTransform);
  vtkSetObjectMacro(Mask,vtkImageData);
  vtkGetObjectMacro(Mask,vtkImageData);
  vtkSetMacro(InverseFlag,bool);
  vtkGetMacro(InverseFlag,bool);

  vtkSetMacro(Mode,int);
  void SetModeToRigidBody() { this->SetMode(VTK_LANDMARK_RIGIDBODY); };
  void SetModeToSimilarity() { this->SetMode(VTK_LANDMARK_SIMILARITY); };
  void SetModeToAffine() { this->SetMode(VTK_LANDMARK_AFFINE); };
  vtkGetMacro(Mode,int);
  const char *GetModeAsString();

  vtkAbstractTransform *MakeTransform();
  void Inverse();

protected:
  vtkGridToLinearTransform();
  ~vtkGridToLinearTransform();
  vtkGridToLinearTransform(const vtkGridToLinearTransform&);
  void operator=(const vtkGridToLinearTransform&);

  void InternalUpdate();

  vtkGridTransform* GridTransform;
  vtkImageData* Mask;
  int Mode;
  bool InverseFlag;
};

//BTX
inline const char *vtkGridToLinearTransform::GetModeAsString()
{
  switch (this->Mode)
    {
    case VTK_LANDMARK_RIGIDBODY:
      return "RigidBody";
    case VTK_LANDMARK_SIMILARITY:
      return "Similarity";
    case VTK_LANDMARK_AFFINE:
      return "Affine";
    default:
      return "Unrecognized";
    }
}
//ETX
#endif
