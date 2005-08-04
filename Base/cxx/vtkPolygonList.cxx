/*=========================================================================

    vtkPolygonList
    Created by Chand T. John for Slicer/NMBL Pipeline

=========================================================================*/
#include "vtkPolygonList.h"
#include "vtkObjectFactory.h"

vtkPolygonList* vtkPolygonList::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkPolygonList");
  if (ret)
    {
    return (vtkPolygonList*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkPolygonList;
}

vtkPolygonList::vtkPolygonList()
{
    for (int p = 0; p < NUM_POLYGONS; p++)
        this->Polygons[p] = vtkPoints::New();
    this->Samples = vtkPoints::New();
}

vtkPolygonList::~vtkPolygonList()
{
    for (int p = 0; p < NUM_POLYGONS; p++)
        this->Polygons[p]->Delete();
    this->Samples->Delete();
}

void vtkPolygonList::Reset(int p)
{
    if (0 <= p && p < NUM_POLYGONS)
        this->Polygons[p]->Reset();
}

int vtkPolygonList::GetNumberOfPoints(int p)
{
    return this->Polygons[p]->GetNumberOfPoints();
}

vtkPoints* vtkPolygonList::GetPolygon(int p)
{
    if (p < 0 || p > NUM_POLYGONS - 1) return NULL;
    return this->Polygons[p];
}

void vtkPolygonList::GetPoint(int p, int i)
{
    vtkDebugMacro("Entering GetPoint(" << p << "," << i << "). ");
    if (0 <= p && p < NUM_POLYGONS)
    {
        vtkFloatingPointType *ras = this->Polygons[p]->GetPoint(i);
        vtkDebugMacro("Got point. ");
        this->ctlpoint.x = (int)ras[0];
        vtkDebugMacro("Got x. ");
        this->ctlpoint.y = (int)ras[1];
        vtkDebugMacro("Got y. ");
    }
}

int vtkPolygonList::GetInsertPosition()
{
    int p = 0;
    for (; p < NUM_POLYGONS; p++)
    {
        int n = this->Polygons[p]->GetNumberOfPoints();
        if (n < 1) break;
    }
    if (p > NUM_POLYGONS - 1) p = -1; // no slots available
    return p;
}

int vtkPolygonList::GetNextInsertPosition(int p)
{
    int q = p;
    if (q < 0) q = 0;
    for (; q < NUM_POLYGONS; q++)
    {
        int n = this->Polygons[q]->GetNumberOfPoints();
        if (n < 1) break;
    }
    if (q > NUM_POLYGONS - 1)
    {
        // If ran off end of array, go back to beginning
        for (q = 0; q <= p; q++)
        {
            int n = this->Polygons[q]->GetNumberOfPoints();
            if (n < 1) break;
        }
        // Back to original spot in array? Then no slots available
        if (q > p - 1) q = -1;
    }
    return q;
}

int vtkPolygonList::GetRetrievePosition()
{
    int p = 0;
    for (; p < NUM_POLYGONS; p++)
    {
        int n = this->Polygons[p]->GetNumberOfPoints();
        if (n > 0) break;
    }
    if (p > NUM_POLYGONS - 1) p = -1; // no nonempty polygons
    return p;
}

int vtkPolygonList::GetNextRetrievePosition(int p)
{
    int q = p + 1;
    for (; q < NUM_POLYGONS; q++)
    {
        int n = this->Polygons[q]->GetNumberOfPoints();
        if (n > 0) break;
    }
    if (q > NUM_POLYGONS - 1)
    {
        // If ran off end of array, go back to beginning
        for (q = 0; q <= p; q++)
        {
            int n = this->Polygons[q]->GetNumberOfPoints();
            if (n > 0) break;
        }
        // Passed original spot in array? Then no nonempty polygons
        if (q > p) q = -1;
    }
    return q;
}

int vtkPolygonList::InsertNextPoint (int p, double x, double y, double z)
{
    return this->Polygons[p]->InsertNextPoint(x, y, z);
}

int vtkPolygonList::GetDensity (int p)
{
    return this->densities[p];
}

int vtkPolygonList::GetClosed (int p)
{
    return this->closed[p];
}

int vtkPolygonList::GetPreshape (int p)
{
    return this->preshape[p];
}

void vtkPolygonList::SetDensity (int p, int d)
{
    this->densities[p] = d;
}

void vtkPolygonList::SetClosed (int p, int closed)
{
    this->closed[p] = closed;
}

void vtkPolygonList::SetPreshape (int p, int preshape)
{
    this->preshape[p] = preshape;
}

void vtkPolygonList::Clear ()
{
    for (int p = 0; p < NUM_POLYGONS; p++)
        this->Polygons[p]->Reset();
}

static void Interpolate (vtkFloatingPointType p[3], double t, double x0,
                         double y0, double z0, double x1, double y1, double z1,
                         double x2, double y2, double z2, double x3, double y3,
                         double z3)
{
    double x01 = (1.0 - t) * x0 + t * x1;
    double y01 = (1.0 - t) * y0 + t * y1;
    double z01 = (1.0 - t) * z0 + t * z1;
    double x11 = (1.0 - t) * x1 + t * x2;
    double y11 = (1.0 - t) * y1 + t * y2;
    double z11 = (1.0 - t) * z1 + t * z2;
    double x21 = (1.0 - t) * x2 + t * x3;
    double y21 = (1.0 - t) * y2 + t * y3;
    double z21 = (1.0 - t) * z2 + t * z3;
    double x02 = (1.0 - t) * x01 + t * x11;
    double y02 = (1.0 - t) * y01 + t * y11;
    double z02 = (1.0 - t) * z01 + t * z11;
    double x12 = (1.0 - t) * x11 + t * x21;
    double y12 = (1.0 - t) * y11 + t * y21;
    double z12 = (1.0 - t) * z11 + t * z21;
    double x03 = (1.0 - t) * x02 + t * x12;
    double y03 = (1.0 - t) * y02 + t * y12;
    double z03 = (1.0 - t) * z02 + t * z12;
    p[0] = x03;
    p[1] = y03;
    p[2] = z03;
}

static bool IsValidIndex(int i, int n)
{
    return 0 <= i && i < n;
}

vtkPoints* vtkPolygonList::GetSampledPolygon(int p)
{
    // Samples closed curves only right now
    // Can sample open curves by simply omitting lastPoint-firstPoint curve
    Samples->Reset();
    vtkPoints *poly = this->GetPolygon(p);

    // Connect up the points in polygon p as a linked list of Points
    if (p < 0 || p > NUM_POLYGONS - 1) // out of range index
        return this->Samples;
    int n = poly->GetNumberOfPoints();
    if (n < 1) // empty polygon
        return this->Samples;
    // Polygon has one or more points

    // And now, the original GetPoints(density) function modified
    int p0 = 0;
    int p1 = 0;
    int p2 = -1;
    int p3 = -1;
    double oneThird = 0.333333333333;
    int density = this->GetDensity(p);
    int closed = this->GetClosed(p);

    // Zero points--redundant, but logic matches vtkImageDrawROI's sampler
    if (n < 1) return this->Samples;
    p2 = p1 + 1;

    // One point
    if (!IsValidIndex (p2, n))
    {
        vtkFloatingPointType *ras = poly->GetPoint(p1);
        Samples->InsertNextPoint(ras[0], ras[1], ras[2]);
        return this->Samples;
    }
    p3 = p2 + 1;

    // Two points
    if (!IsValidIndex (p3, n))
    {
        vtkFloatingPointType *ras;
        vtkFloatingPointType ras1[3];
        ras = poly->GetPoint(p1);
        ras1[0] = ras[0];
        ras1[1] = ras[1];
        ras1[2] = ras[2];
        vtkFloatingPointType ras2[3];
        ras = poly->GetPoint(p2);
        ras2[0] = ras[0];
        ras2[1] = ras[1];
        ras2[2] = ras[2];
        Samples->InsertNextPoint(ras1[0], ras1[1], ras1[2]);
        // Linearly interpolate between the two points
        for (int j = 1; j <= density; j++)
        {
            double t = (double)j / (double)(density + 1.0);
            double Xt = (1.0 - t) * (ras1[0]) + t * (ras2[0]);
            double Yt = (1.0 - t) * (ras1[1]) + t * (ras2[1]);
            double Zt = (1.0 - t) * (ras1[2]) + t * (ras2[2]);
            Samples->InsertNextPoint(Xt, Yt, Zt);
        }
        Samples->InsertNextPoint(ras2[0], ras2[1], ras2[2]);
        return this->Samples;
    }

    // Three or more points
    // p0 == p1 == this->firstPoint, p2 == second point, p3 == third point
    while (IsValidIndex (p1, n))
    {
        vtkFloatingPointType *ras;
        vtkFloatingPointType ras0[3];
        ras = poly->GetPoint(p0);
        ras0[0] = ras[0];
        ras0[1] = ras[1];
        ras0[2] = ras[2];
        vtkFloatingPointType ras1[3];
        ras = poly->GetPoint(p1);
        ras1[0] = ras[0];
        ras1[1] = ras[1];
        ras1[2] = ras[2];
        vtkFloatingPointType ras2[3];
        ras = poly->GetPoint(p2);
        ras2[0] = ras[0];
        ras2[1] = ras[1];
        ras2[2] = ras[2];
        vtkFloatingPointType ras3[3];
        ras = poly->GetPoint(p3);
        ras3[0] = ras[0];
        ras3[1] = ras[1];
        ras3[2] = ras[2];
        // Draw curve connecting p1 and p2
        if (p1 == 0)
        {
            // First curve--don't increment p0
            double p2dx = 0.5 * (ras3[0] - ras1[0]);
            double p2dy = 0.5 * (ras3[1] - ras1[1]);
            double p2dz = 0.5 * (ras3[2] - ras1[2]);
            double p2_p1x = ras2[0] - ras1[0];
            double p2_p1y = ras2[1] - ras1[1];
            double p2_p1z = ras2[2] - ras1[2];
            double p2_p1sq = p2_p1x * p2_p1x + p2_p1y * p2_p1y +
                             p2_p1z * p2_p1z;
            double Ax = p2_p1x;
            double Ay = p2_p1y;
            double Az = p2_p1z;
            double C = 0.5 * ((ras1[0] - ras2[0]) * (ras1[0] + ras2[0]) +
                              (ras1[1] - ras2[1]) * (ras1[1] + ras2[1]) +
                              (ras1[2] - ras2[2]) * (ras1[2] + ras2[2]));
            double x0 = ras2[0] + p2dx;
            double y0 = ras2[1] + p2dy;
            double z0 = ras2[2] + p2dz;
            double ax0by0c = Ax * x0 + Ay * y0 + Az * z0 + C;
            // Derivative at p0 is reflection of derivative at p2
            // (p2dx, p2dy, p2dz) over the line bisecting the edge connecting
            // p1 and p2
            double p1dx = ras2[0] - ras1[0] + p2dx + 2.0 *
                          (ras1[0] - ras2[0]) / p2_p1sq * ax0by0c;
            double p1dy = ras2[1] - ras1[1] + p2dy + 2.0 *
                          (ras1[1] - ras2[1]) / p2_p1sq * ax0by0c;
            double p1dz = ras2[2] - ras1[2] + p2dz + 2.0 *
                          (ras1[2] - ras2[2]) / p2_p1sq * ax0by0c;
            // Plot left endpoint
            Samples->InsertNextPoint(ras1[0], ras1[1], ras1[2]);
            // Plot intermediate points
            for (int j = 1; j <= density; j++)
            {
                vtkFloatingPointType p[3];
                double t = (double)j / (double)(density + 1.0);
                Interpolate(p, t, ras1[0], ras1[1], ras1[2],
                            ras1[0] + oneThird * p1dx,
                            ras1[1] + oneThird * p1dy,
                            ras1[2] + oneThird * p1dz,
                            ras2[0] - oneThird * p2dx,
                            ras2[1] - oneThird * p2dy,
                            ras2[2] - oneThird * p2dz,
                            ras2[0], ras2[1], ras2[2]);
                Samples->InsertNextPoint(p[0], p[1], p[2]);
            }
            p1++;
            p2++;
            p3++;
        }
        else if (IsValidIndex (p3, n))
        {
            // Middle curve
            double p1dx = 0.5 * (ras2[0] - ras0[0]);
            double p1dy = 0.5 * (ras2[1] - ras0[1]);
            double p1dz = 0.5 * (ras2[2] - ras0[2]);
            double p2dx = 0.5 * (ras3[0] - ras1[0]);
            double p2dy = 0.5 * (ras3[1] - ras1[1]);
            double p2dz = 0.5 * (ras3[2] - ras1[2]);
            double plx = oneThird * p1dx + ras1[0];
            double ply = oneThird * p1dy + ras1[1];
            double plz = oneThird * p1dz + ras1[2];
            double prx = ras2[0] - oneThird * p2dx;
            double pry = ras2[1] - oneThird * p2dy;
            double prz = ras2[2] - oneThird * p2dz;
            // Plot left endpoint
            Samples->InsertNextPoint(ras1[0], ras1[1], ras1[2]);
            // Plot intermediate points
            for (int j = 1; j <= density; j++)
            {
                vtkFloatingPointType p[3];
                double t = (double)j / (double)(density + 1.0);
                Interpolate(p, t, ras1[0], ras1[1], ras1[2], plx, ply, plz,
                            prx, pry, prz, ras2[0], ras2[1], ras2[2]);
                Samples->InsertNextPoint(p[0], p[1], p[2]);
            }
            p0++;
            p1++;
            p2++;
            p3++;
        }
        else if (IsValidIndex(p2,n) && !IsValidIndex(p3,n))
        {
            // Curve connecting last two control points
            double p1dx = 0.5 * (ras2[0] - ras0[0]);
            double p1dy = 0.5 * (ras2[1] - ras0[1]);
            double p1dz = 0.5 * (ras2[2] - ras0[2]);
            double p1_p2x = ras1[0] - ras2[0];
            double p1_p2y = ras1[1] - ras2[1];
            double p1_p2z = ras1[2] - ras2[2];
            double p1_p2sq = p1_p2x * p1_p2x + p1_p2y * p1_p2y +
                             p1_p2z * p1_p2z;
            double Ax = ras1[0] - ras2[0];
            double Ay = ras1[1] - ras2[1];
            double Az = ras1[2] - ras2[2];
            double C = 0.5 * ((ras2[0] - ras1[0]) * (ras2[0] + ras1[0]) +
                              (ras2[1] - ras1[1]) * (ras2[1] + ras1[1]) +
                              (ras2[2] - ras1[2]) * (ras2[2] + ras1[2]));
            double x0 = ras1[0] + p1dx;
            double y0 = ras1[1] + p1dy;
            double z0 = ras1[2] + p1dz;
            double ax0by0c = Ax * x0 + Ay * y0 + Az * z0 + C;
            // Derivative at p2 is reflection of derivative at p1
            // (p1dx, p1dy, p1dz) over the line bisecting the edge connecting
            // p2 and p1
            double p2dx = ras1[0] - ras2[0] + p1dx + 2.0 *
                          (ras2[0] - ras1[0]) / p1_p2sq * ax0by0c;
            double p2dy = ras1[1] - ras2[1] + p1dy + 2.0 *
                          (ras2[1] - ras1[1]) / p1_p2sq * ax0by0c;
            double p2dz = ras1[2] - ras2[2] + p1dz + 2.0 *
                          (ras2[2] - ras1[2]) / p1_p2sq * ax0by0c;
            double plx = oneThird * p1dx + ras1[0];
            double ply = oneThird * p1dy + ras1[1];
            double plz = oneThird * p1dz + ras1[2];
            double prx = ras2[0] - oneThird * p2dx;
            double pry = ras2[1] - oneThird * p2dy;
            double prz = ras2[2] - oneThird * p2dz;
            // Plot left endpoint
            Samples->InsertNextPoint(ras1[0], ras1[1], ras1[2]);
            // Plot intermediate points
            for (int j = 1; j <= density; j++)
            {
                vtkFloatingPointType p[3];
                double t = (double)j / (double)(density + 1.0);
                Interpolate(p, t, ras1[0], ras1[1], ras1[2], plx, ply, plz,
                            prx, pry, prz, ras2[0], ras2[1], ras2[2]);
                Samples->InsertNextPoint(p[0], p[1], p[2]);
            }
            if (!closed)
            {
                // Plot right endpoint if curve is open--this is the last
                // point to plot on the curve
                Samples->InsertNextPoint(ras2[0], ras2[1], ras2[2]);
            }
            p0++;
            p1++;
            p2++;
            // p3 is already an invalid index
        }
        else if (!closed) // p1 is lastPoint
        {
            p1 = -1; // this is the last iteration: exit the loop now
        }
        else // !p2 && !p3; p1 is lastPoint
        {
            // Curve connecting lastPoint (p1) and firstPoint (p2)
            p2 = 0;
            p3 = p2 + 1;
            ras = poly->GetPoint(p2);
            ras2[0] = ras[0];
            ras2[1] = ras[1];
            ras2[2] = ras[2];
            ras = poly->GetPoint(p3);
            ras3[0] = ras[0];
            ras3[1] = ras[1];
            ras3[2] = ras[2];
            double p1dx = 0.5 * (ras2[0] - ras0[0]);
            double p1dy = 0.5 * (ras2[1] - ras0[1]);
            double p1dz = 0.5 * (ras2[2] - ras0[2]);
            double p2dx = 0.5 * (ras3[0] - ras1[0]);
            double p2dy = 0.5 * (ras3[1] - ras1[1]);
            double p2dz = 0.5 * (ras3[2] - ras1[2]);
            double plx = oneThird * p1dx + ras1[0];
            double ply = oneThird * p1dy + ras1[1];
            double plz = oneThird * p1dz + ras1[2];
            double prx = ras2[0] - oneThird * p2dx;
            double pry = ras2[1] - oneThird * p2dy;
            double prz = ras2[2] - oneThird * p2dz;
            // Plot left endpoint
            Samples->InsertNextPoint(ras1[0], ras1[1], ras1[2]);
            // Plot intermediate points
            for (int j = 1; j <= density; j++)
            {
                vtkFloatingPointType p[3];
                double t = (double)j / (double)(density + 1.0);
                Interpolate(p, t, ras1[0], ras1[1], ras1[2], plx, ply, plz,
                            prx, pry, prz, ras2[0], ras2[1], ras2[2]);
                Samples->InsertNextPoint(p[0], p[1], p[2]);
            }
            p1 = -1; // this is the last iteration: exit the loop now
        }
    }

    return this->Samples;
}

void vtkPolygonList::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  for (int p = 0; p < NUM_POLYGONS; p++)
  {
      os << indent << "Polygon " << p << ":" << endl;
      this->Polygons[p]->PrintSelf(os,indent);
  }
}
