// Copyright 2010-2014 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EAVL_SCENE_RENDERER_H
#define EAVL_SCENE_RENDERER_H

#include "eavlDataSet.h"
#include "eavlCellSet.h"
#include "eavlColor.h"
#include "eavlColorTable.h"


static inline eavlColor MapValueToColor(double value,
                                 double vmin, double vmax,
                                 eavlColorTable &ct)
{
    double norm = 0.5;
    if (vmin != vmax)
        norm = (value - vmin) / (vmax - vmin);
    eavlColor c = ct.Map(norm);
    return c;
}

static inline float MapValueToNorm(double value,
                            double vmin, double vmax)
{
    double norm = 0.5;
    if (vmin != vmax)
        norm = (value - vmin) / (vmax - vmin);
    return norm;
}


struct ColorByOptions
{
    bool singleColor; ///< if true, use one flat color, else field+colortable
    eavlColor color;  ///< color to use when singleColor==true
    eavlField *field; ///< field to color by when singleColor==false
    double vmin, vmax; ///< field min and max
    eavlColorTable *ct; ///< colortable to color by when singleColor==false
};

// ****************************************************************************
// Class:  eavlRenderer
//
// Purpose:
///   Base class for renderers.
//
// Programmer:  Jeremy Meredith
// Creation:    July 18, 2012
//
// Modifications:
//   Jeremy Meredith, Mon Mar  4 15:44:23 EST 2013
//   Big refactoring; more consistent internal code with less
//   duplication and cleaner external API.
//
// ****************************************************************************
class eavlSceneRenderer
{
    int ncolors;
    float colors[3*1024];
  public:
    eavlSceneRenderer()
    {
        ncolors = 1;
        colors[0] = colors[1] = colors[2] = 0.5;
    }
    virtual ~eavlSceneRenderer()
    {
    }

    virtual void StartScene() { }
    virtual void EndScene() { }

    virtual void StartTriangles() { }
    virtual void EndTriangles() { }

    //
    // per-plot properties (in essence, at least) follow:
    //

    virtual void SetActiveColor(eavlColor c)
    {
        ncolors = 1;
        colors[0] = c.c[0];
        colors[1] = c.c[1];
        colors[2] = c.c[2];
    }
    virtual void SetActiveColorTable(string ct)
    {
        ncolors = 1024;
        eavlColorTable colortable(ct);
        colortable.Sample(ncolors, colors);
    }
    //virtual void SetActiveMaterial() { } // diffuse, specular, ambient
    //virtual void SetActiveLighting() { } // etc.


    // ----------------------------------------
    // Vertex Normal
    //----------------------------------------

    // vertex scalar
    virtual void AddTriangleVnVs(double x0, double y0, double z0,
                                 double x1, double y1, double z1,
                                 double x2, double y2, double z2,
                                 double u0, double v0, double w0,
                                 double u1, double v1, double w1,
                                 double u2, double v2, double w2,
                                 double s0, double s1, double s2) = 0;

    // face scalar
    virtual void AddTriangleVnFs(double x0, double y0, double z0,
                                 double x1, double y1, double z1,
                                 double x2, double y2, double z2,
                                 double u0, double v0, double w0,
                                 double u1, double v1, double w1,
                                 double u2, double v2, double w2,
                                 double s)
    {
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u0,v0,w0,
                        u1,v1,w1,
                        u2,v2,w2,
                        s, s, s);
    }
    // no scalar
    virtual void AddTriangleVn(double x0, double y0, double z0,
                               double x1, double y1, double z1,
                               double x2, double y2, double z2,
                               double u0, double v0, double w0,
                               double u1, double v1, double w1,
                               double u2, double v2, double w2)
    {
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u0,v0,w0,
                        u1,v1,w1,
                        u2,v2,w2,
                        0, 0, 0);
    }

    // ----------------------------------------
    // Face Normal
    //----------------------------------------

    // vertex scalar
    virtual void AddTriangleFnVs(double x0, double y0, double z0,
                                 double x1, double y1, double z1,
                                 double x2, double y2, double z2,
                                 double u,  double v,  double w,
                                 double s0, double s1, double s2)
    {
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        s0,s1,s2);
    }
    // face scalar
    virtual void AddTriangleFnFs(double x0, double y0, double z0,
                                 double x1, double y1, double z1,
                                 double x2, double y2, double z2,
                                 double u,  double v,  double w,
                                 double s)
    {
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        s, s, s);
    }
    // no scalar
    virtual void AddTriangleFn(double x0, double y0, double z0,
                               double x1, double y1, double z1,
                               double x2, double y2, double z2,
                               double u,  double v,  double w)
    {
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        0, 0, 0);
    }

    // ----------------------------------------
    // No Normal
    // ----------------------------------------

    // vertex scalar
    virtual void AddTriangleVs(double x0, double y0, double z0,
                               double x1, double y1, double z1,
                               double x2, double y2, double z2,
                               double s0, double s1, double s2)
    {
        double u = 1;
        double v = 0;
        double w = 0;
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        s0,s1,s2);
    }
    // face scalar
    virtual void AddTriangleFs(double x0, double y0, double z0,
                               double x1, double y1, double z1,
                               double x2, double y2, double z2,
                               double s)
    {
        double u = 1;
        double v = 0;
        double w = 0;
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        s, s, s);
    }
    // no scalar
    virtual void AddTriangle(double x0, double y0, double z0,
                             double x1, double y1, double z1,
                             double x2, double y2, double z2)
    {
        double u = 1;
        double v = 0;
        double w = 0;
        AddTriangleVnVs(x0,y0,z0,
                        x1,y1,z1,
                        x2,y2,z2,
                        u ,v ,w ,
                        u ,v ,w ,
                        u ,v ,w ,
                        0, 0, 0);
    }

    // ----------------------------------------
    // Sphere
    // ----------------------------------------
    virtual void AddSphere(double x, double y, double z, double r)
    {
        AddSphereS(x,y,z,r,0);
    }
    virtual void AddSphereS(double x, double y, double z, double r, double s)
    {
        ///\todo: make this pure virtual
    }


    // -----------------------------------------------------------------------
    // -----------------------------------------------------------------------

    virtual void RenderPoints(int npts, double *pts,
                              ColorByOptions opts) { }
    virtual void RenderCells0D(eavlCellSet *cs,
                               int npts, double *pts,
                               ColorByOptions opts) { }
    virtual void RenderCells1D(eavlCellSet *cs,
                               int npts, double *pts,
                               ColorByOptions opts) { }
    virtual void RenderCells2D(eavlCellSet *cs,
                               int npts, double *pts,
                               ColorByOptions opts,
                               bool wireframe,
                               eavlField *normals)
    {
        eavlField *f = opts.field;
        bool NoColors = (opts.field == NULL);
        bool PointColors = (opts.field &&
                opts.field->GetAssociation() == eavlField::ASSOC_POINTS);
        bool CellColors = (opts.field &&
                opts.field->GetAssociation() == eavlField::ASSOC_CELL_SET &&
                opts.field->GetAssocCellSet() == cs->GetName());
        bool NoNormals = (normals == NULL);
        bool PointNormals = (normals &&
                normals->GetAssociation() == eavlField::ASSOC_POINTS);
        bool CellNormals = (normals &&
                normals->GetAssociation() == eavlField::ASSOC_CELL_SET &&
                normals->GetAssocCellSet() == cs->GetName());

        StartTriangles();

        int ncells = cs->GetNumCells();

        // triangles, quads, pixels, and polygons; all 2D shapes
        for (int j=0; j<ncells; j++)
        {
            eavlCell cell = cs->GetCellNodes(j);
            if (cell.type != EAVL_TRI &&
                cell.type != EAVL_QUAD &&
                cell.type != EAVL_PIXEL &&
                cell.type != EAVL_POLYGON)
            {
                continue;
            }

            // tesselate polygons with more than 3 points
            for (int pass = 3; pass <= cell.numIndices; ++pass)
            {
                int i0 = cell.indices[0];
                int i1 = cell.indices[pass-2];
                int i2 = cell.indices[pass-1];
                // pixel is a special case
                if (pass == 4 && cell.type == EAVL_PIXEL)
                {
                    i0 = cell.indices[1];
                    i1 = cell.indices[3];
                    i2 = cell.indices[2];
                }

                // get vertex coordinates
                double x0 = pts[i0*3+0];
                double y0 = pts[i0*3+1];
                double z0 = pts[i0*3+2];

                double x1 = pts[i1*3+0];
                double y1 = pts[i1*3+1];
                double z1 = pts[i1*3+2];

                double x2 = pts[i2*3+0];
                double y2 = pts[i2*3+1];
                double z2 = pts[i2*3+2];

                // get scalars (if applicable)
                double s, s0, s1, s2;
                if (CellColors)
                {
                    s = MapValueToNorm(f->GetArray()->
                                       GetComponentAsDouble(j,0),
                                       opts.vmin, opts.vmax);
                }
                else if (PointColors)
                {
                    s0 = MapValueToNorm(f->GetArray()->
                                        GetComponentAsDouble(i0,0),
                                        opts.vmin, opts.vmax);
                    s1 = MapValueToNorm(f->GetArray()->
                                        GetComponentAsDouble(i1,0),
                                        opts.vmin, opts.vmax);
                    s2 = MapValueToNorm(f->GetArray()->
                                        GetComponentAsDouble(i2,0),
                                        opts.vmin, opts.vmax);
                }

                // get normals (if applicable)
                double u,v,w, u0,v0,w0, u1,v1,w1, u2,v2,w2;
                if (CellNormals)
                {
                    u = normals->GetArray()->GetComponentAsDouble(j,0);
                    v = normals->GetArray()->GetComponentAsDouble(j,1);
                    w = normals->GetArray()->GetComponentAsDouble(j,2);
                }
                else if (PointNormals)
                {
                    u0 = normals->GetArray()->GetComponentAsDouble(i0,0);
                    v0 = normals->GetArray()->GetComponentAsDouble(i0,1);
                    w0 = normals->GetArray()->GetComponentAsDouble(i0,2);

                    u1 = normals->GetArray()->GetComponentAsDouble(i1,0);
                    v1 = normals->GetArray()->GetComponentAsDouble(i1,1);
                    w1 = normals->GetArray()->GetComponentAsDouble(i1,2);

                    u2 = normals->GetArray()->GetComponentAsDouble(i2,0);
                    v2 = normals->GetArray()->GetComponentAsDouble(i2,1);
                    w2 = normals->GetArray()->GetComponentAsDouble(i2,2);
                }


                // send the triangle down
                if (NoNormals)
                {
                    if (NoColors)
                    {
                        AddTriangle(x0,y0,z0, x1,y1,z1, x2,y2,z2);
                    }
                    else if (CellColors)
                    {
                        AddTriangleFs(x0,y0,z0, x1,y1,z1, x2,y2,z2, s);
                    }
                    else if (PointColors)
                    {
                        AddTriangleVs(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                      s0,s1,s2);
                    }
                }
                else if (CellNormals)
                {
                    if (NoColors)
                    {
                        AddTriangleFn(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                      u,v,w);
                    }
                    else if (CellColors)
                    {
                        AddTriangleFnFs(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                        u,v,w, s);
                    }
                    else if (PointColors)
                    {
                        AddTriangleFnVs(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                        u,v,w, s0,s1,s2);
                    }
                }
                else if (PointNormals)
                {
                    if (NoColors)
                    {
                        AddTriangleVn(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                      u0,v0,w0, u1,v1,w2, u2,v2,w2);
                    }
                    else if (CellColors)
                    {
                        AddTriangleVnFs(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                        u0,v0,w0, u1,v1,w2, u2,v2,w2,
                                        s);
                    }
                    else if (PointColors)
                    {
                        AddTriangleVnVs(x0,y0,z0, x1,y1,z1, x2,y2,z2,
                                        u0,v0,w0, u1,v1,w2, u2,v2,w2,
                                        s0,s1,s2);
                    }
                }
            }
        }

        EndTriangles();
    }
    virtual void RenderCells3D(eavlCellSet *cs,
                               int npts, double *pts,
                               ColorByOptions opts) { }
};


#endif