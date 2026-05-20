#ifndef NUMERIC_WRAPPER
#define NUMERIC_WRAPPER

#define USE_INDIRECT_PREDS
//#define USE_DOUBLE
//#define USE_LAZY_CORE
//#define USE_PLAIN_CORE
//#define USE_LAZY_GMPQ
//#define USE_PLAIN_GMPQ

#include "ImplicitPoint.h"

#ifdef USE_INDIRECT_PREDS
typedef GenericPoint PointType;
typedef ImplicitPoint3dLnc ImplicitPointLnc;
typedef ExplicitPoint3D ExplicitPoint;
#endif

#ifdef USE_DOUBLE
typedef double CoordType;
#define GET_DOUBLE_VAL(a) (a)
#define GET_SIGN(a) (((a)>0) - ((a)<0))
#define GET_FABS(a) (((a)>0) ? (a) : (-a))
#define GET_SQRT(a) (sqrt(a))
#endif

#ifdef USE_LAZY_CORE
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/CORE_Expr.h>
typedef CGAL::Lazy_exact_nt< CORE::Expr> CoordType;
#define GET_DOUBLE_VAL(a) (a).approx().inf()
#define GET_SIGN(a) (((a)>0) - ((a)<0))
#define GET_FABS(a) (((a)>0) ? (a) : (-a))
#define GET_SQRT(a) (sqrt(a))
#endif

#ifdef USE_PLAIN_CORE
#include <CGAL/CORE_Expr.h>
typedef CORE::Expr CoordType;
#define GET_DOUBLE_VAL(a) (a).doubleValue()
#define GET_SIGN(a) ((a).sign())
#define GET_FABS(a) (fabs(a))
#define GET_SQRT(a) (sqrt(a))
#endif

#ifdef USE_LAZY_GMPQ
#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/Gmpq.h>
typedef CGAL::Lazy_exact_nt< CGAL::Gmpq > CoordType;
#define GET_DOUBLE_VAL(a) (a).approx().inf()
#define GET_SIGN(a) (((a)>0) - ((a)<0))
#define GET_FABS(a) (((a)>0) ? (a) : (-a))
#define GET_SQRT(a) (::sqrt(GET_DOUBLE_VAL(a)))
#endif


#ifdef USE_PLAIN_GMPQ
#include <CGAL/Gmpxx.h>
typedef mpq_class CoordType;
#define GET_DOUBLE_VAL(a) (a).get_d()
#define GET_SIGN(a) (((a)>0) - ((a)<0))
#define GET_FABS(a) (((a)>0) ? (a) : (-a))
#define GET_SQRT(a) (::sqrt(GET_DOUBLE_VAL(a)))
#endif

#ifndef USE_INDIRECT_PREDS

#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

extern void ip_error(const char* msg);

#pragma intrinsic(fabs)

#define INFINITE_VERTEX UINT32_MAX
#define DT_UNKNOWN  0
#define DT_OUT  1
#define DT_IN  2
#define EXPECTED_VT_SIZE 128

class PointType {
public:
    CoordType coords[3];

    PointType() {}

    PointType(double a, double b, double c) : coords{ a, b, c } {}

    PointType(const PointType& a, const PointType& b, const CoordType& t) {
        for (int i = 0; i < 3; i++)
            coords[i] = (a.coords[i] * (1 - t) + (b.coords[i] * t));
    }

    const CoordType& X() const { return coords[0]; }
    const CoordType& Y() const { return coords[1]; }
    const CoordType& Z() const { return coords[2]; }

    bool isExplicit3D() const { return GET_DOUBLE_VAL(coords[0]) == coords[0] && GET_DOUBLE_VAL(coords[1]) == coords[1] && GET_DOUBLE_VAL(coords[2]) == coords[2]; }
    PointType toExplicit3D() const { return *this; }
    void apapExplicit(PointType& p) const { 
        p.coords[0] = GET_DOUBLE_VAL(coords[0]); 
        p.coords[1] = GET_DOUBLE_VAL(coords[1]);
        p.coords[2] = GET_DOUBLE_VAL(coords[2]);
    }

    CoordType& operator[](int i) { return coords[i]; }
    const CoordType& operator[](int i) const { return coords[i]; }

    bool operator==(const PointType& p) const {
        return coords[0] == p.coords[0] && coords[1] == p.coords[1] && coords[2] == p.coords[2]; 
    }

    void getApproxXYZCoordinates(double& x, double& y, double& z, bool apap=true) const {
        x = GET_DOUBLE_VAL(coords[0]);
        y = GET_DOUBLE_VAL(coords[1]);
        z = GET_DOUBLE_VAL(coords[2]);
    }

    static int dotProductSign3D(const PointType& a, const PointType& b, const PointType& c) {
        CoordType ac[3] = { a.coords[0] - c.coords[0], a.coords[1] - c.coords[1], a.coords[2] - c.coords[2] };
        CoordType bc[3] = { b.coords[0] - c.coords[0], b.coords[1] - c.coords[1], b.coords[2] - c.coords[2] };
        return GET_SIGN(ac[0] * bc[0] + ac[1] * bc[1] + ac[2] * bc[2]);
    }

    static int maxComponentInTriangleNormal(
        const CoordType& ov10, const CoordType& ov11, const CoordType& ov12,
        const CoordType& ov20, const CoordType& ov21, const CoordType& ov22,
        const CoordType& ov30, const CoordType& ov31, const CoordType& ov32)
    {
        const CoordType v3x = ov30 - ov20;
        const CoordType v3y = ov31 - ov21;
        const CoordType v3z = ov32 - ov22;
        const CoordType v2x = ov20 - ov10;
        const CoordType v2y = ov21 - ov11;
        const CoordType v2z = ov22 - ov12;
        const CoordType nvx1 = v2y * v3z;
        const CoordType nvx2 = v2z * v3y;
        const CoordType nvx = nvx1 - nvx2;
        const CoordType nvy1 = v3x * v2z;
        const CoordType nvy2 = v3z * v2x;
        const CoordType nvy = nvy1 - nvy2;
        const CoordType nvz1 = v2x * v3y;
        const CoordType nvz2 = v2y * v3x;
        const CoordType nvz = nvz1 - nvz2;
        const CoordType nvxc = GET_FABS(nvx);
        const CoordType nvyc = GET_FABS(nvy);
        const CoordType nvzc = GET_FABS(nvz);
        CoordType nv = nvxc;
        if (nvyc > nv) nv = nvyc;
        if (nvzc > nv) nv = nvzc;

        if (nv == nvxc) return 0;
        if (nv == nvyc) return 1;
        assert(nv == nvzc);
        return 2;
    }

    static int Orient2D(const CoordType& p0, const CoordType& p1, const CoordType& q0, const CoordType& q1, const CoordType& r0, const CoordType& r1)
    {
#ifdef USE_DOUBLE
        return orient2d(p0, p1, q0, q1, r0, r1);
#else
        const CoordType dl = (q0 - p0) * (r1 - p1);
        const CoordType dr = (q1 - p1) * (r0 - p0);
        const CoordType det = dl - dr;

        return (det < 0) - (det > 0);
#endif
    }

    static int Orient2Dxy(const PointType& p, const PointType& q, const PointType& r)
    {
        return Orient2D(p[0], p[1], q[0], q[1], r[0], r[1]);
    }

    static int Orient2Dyz(const PointType& p, const PointType& q, const PointType& r)
    {
        return Orient2D(p[1], p[2], q[1], q[2], r[1], r[2]);
    }

    static int Orient2Dzx(const PointType& p, const PointType& q, const PointType& r)
    {
        return Orient2D(p[2], p[0], q[2], q[0], r[2], r[0]);
    }

    static bool misaligned(const PointType& A, const PointType& B, const PointType& C) {
        return (Orient2Dxy(A, B, C) || Orient2Dyz(A, B, C) || Orient2Dzx(A, B, C));
    }

    static int Orient3D(const PointType& p, const PointType& q, const PointType& r, const PointType& s) {
#ifdef USE_DOUBLE
        return orient3d(p.X(), p.Y(), p.Z(), q.X(), q.Y(), q.Z(), r.X(), r.Y(), r.Z(), s.X(), s.Y(), s.Z());
#else
        const CoordType fadx = q[0] - p[0], fbdx = r[0] - p[0], fcdx = s[0] - p[0];
        const CoordType fady = q[1] - p[1], fbdy = r[1] - p[1], fcdy = s[1] - p[1];
        const CoordType fadz = q[2] - p[2], fbdz = r[2] - p[2], fcdz = s[2] - p[2];

        const CoordType fbdxcdy = fbdx * fcdy * fadz; const CoordType fcdxbdy = fcdx * fbdy * fadz;
        const CoordType fcdxady = fcdx * fady * fbdz; const CoordType fadxcdy = fadx * fcdy * fbdz;
        const CoordType fadxbdy = fadx * fbdy * fcdz; const CoordType fbdxady = fbdx * fady * fcdz;

        const CoordType det = (fbdxcdy - fcdxbdy) + (fcdxady - fadxcdy) + (fadxbdy - fbdxady);

        return (det < 0) - (det > 0);
#endif
    }

    static int InSphere(const PointType& pa, const PointType& pb, const PointType& pc, const PointType& pd, const PointType& pe)
    {
#ifdef USE_DOUBLE
        return ::InSphere(pa.X(), pa.Y(), pa.Z(), pb.X(), pb.Y(), pb.Z(), pc.X(), pc.Y(), pc.Z(), pd.X(), pd.Y(), pd.Z(), pe.X(), pe.Y(), pe.Z());
#else
        const CoordType aex = pa[0] - pe[0], bex = pb[0] - pe[0], cex = pc[0] - pe[0], dex = pd[0] - pe[0];
        const CoordType aey = pa[1] - pe[1], bey = pb[1] - pe[1], cey = pc[1] - pe[1], dey = pd[1] - pe[1];
        const CoordType aez = pa[2] - pe[2], bez = pb[2] - pe[2], cez = pc[2] - pe[2], dez = pd[2] - pe[2];

        const CoordType aexbey = aex * bey;
        const CoordType bexaey = bex * aey;
        const CoordType ab = aexbey - bexaey;
        const CoordType bexcey = bex * cey;
        const CoordType cexbey = cex * bey;
        const CoordType bc = bexcey - cexbey;
        const CoordType cexdey = cex * dey;
        const CoordType dexcey = dex * cey;
        const CoordType cd = cexdey - dexcey;
        const CoordType dexaey = dex * aey;
        const CoordType aexdey = aex * dey;
        const CoordType da = dexaey - aexdey;
        const CoordType aexcey = aex * cey;
        const CoordType cexaey = cex * aey;
        const CoordType ac = aexcey - cexaey;
        const CoordType bexdey = bex * dey;
        const CoordType dexbey = dex * bey;
        const CoordType bd = bexdey - dexbey;
        const CoordType abc1 = aez * bc;
        const CoordType abc2 = bez * ac;
        const CoordType abc3 = cez * ab;
        const CoordType abc4 = abc1 + abc3;
        const CoordType abc = abc4 - abc2;
        const CoordType bcd1 = bez * cd;
        const CoordType bcd2 = cez * bd;
        const CoordType bcd3 = dez * bc;
        const CoordType bcd4 = bcd1 + bcd3;
        const CoordType bcd = bcd4 - bcd2;
        const CoordType cda1 = cez * da;
        const CoordType cda2 = dez * ac;
        const CoordType cda3 = aez * cd;
        const CoordType cda4 = cda1 + cda3;
        const CoordType cda = cda4 + cda2;
        const CoordType dab1 = dez * ab;
        const CoordType dab2 = aez * bd;
        const CoordType dab3 = bez * da;
        const CoordType dab4 = dab1 + dab3;
        const CoordType dab = dab4 + dab2;
        const CoordType al1 = aex * aex;
        const CoordType al2 = aey * aey;
        const CoordType al3 = aez * aez;
        const CoordType al4 = al1 + al2;
        const CoordType alift = al4 + al3;
        const CoordType bl1 = bex * bex;
        const CoordType bl2 = bey * bey;
        const CoordType bl3 = bez * bez;
        const CoordType bl4 = bl1 + bl2;
        const CoordType blift = bl4 + bl3;
        const CoordType cl1 = cex * cex;
        const CoordType cl2 = cey * cey;
        const CoordType cl3 = cez * cez;
        const CoordType cl4 = cl1 + cl2;
        const CoordType clift = cl4 + cl3;
        const CoordType dl1 = dex * dex;
        const CoordType dl2 = dey * dey;
        const CoordType dl3 = dez * dez;
        const CoordType dl4 = dl1 + dl2;
        const CoordType dlift = dl4 + dl3;
        const CoordType ds1 = dlift * abc;
        const CoordType ds2 = clift * dab;
        const CoordType dl = ds2 - ds1;
        const CoordType dr1 = blift * cda;
        const CoordType dr2 = alift * bcd;
        const CoordType dr = dr2 - dr1;
        const CoordType det = dl + dr;

        return ((det < 0) - (det > 0));
#endif
    }

    static bool lineCrossesTriangle(const PointType& s1, const PointType& s2, const PointType& v1, const PointType& v2, const PointType& v3)
    {
        const int o1 = Orient3D(s1, s2, v1, v2);
        const int o2 = Orient3D(s1, s2, v2, v3);
        if ((o1 > 0 && o2 < 0) || (o1 < 0 && o2 > 0)) return false;
        const int o3 = Orient3D(s1, s2, v3, v1);
        if ((o1 > 0 && o3 < 0) || (o1 < 0 && o3 > 0)) return false;
        if ((o2 > 0 && o3 < 0) || (o2 < 0 && o3 > 0)) return false;
        return true;
    }

    static bool innerSegmentsCross(const PointType& A, const PointType& B, const PointType& P, const PointType& Q, int xyz)
    {
        int o11, o12, o21, o22;

        if (xyz == 2)
        {
            o11 = Orient2Dxy(P, A, B);
            o12 = Orient2Dxy(Q, B, A);
            o21 = Orient2Dxy(A, P, Q);
            o22 = Orient2Dxy(B, Q, P);
        }
        else if (xyz == 0)
        {
            o11 = Orient2Dyz(P, A, B);
            o12 = Orient2Dyz(Q, B, A);
            o21 = Orient2Dyz(A, P, Q);
            o22 = Orient2Dyz(B, Q, P);
        }
        else
        {
            o11 = Orient2Dzx(P, A, B);
            o12 = Orient2Dzx(Q, B, A);
            o21 = Orient2Dzx(A, P, Q);
            o22 = Orient2Dzx(B, Q, P);
        }

        return (o11 && o21 && o11 == o12 && o21 == o22);
    }

    static bool pointInInnerSegment(const PointType& p, const PointType& v1, const PointType& v2)
    {
        if (misaligned(p, v1, v2)) return false;

        int lt2, lt3;
        lt2 = v1[0] < p[0];
        lt3 = p[0] < v2[0];
        if (lt2) return (lt2 == lt3);
        lt2 = v1[1] < p[1];
        lt3 = p[1] < v2[1];
        if (lt2) return (lt2 == lt3);
        lt2 = v1[2] < p[2];
        lt3 = p[2] < v2[2];
        if (lt2) return (lt2 == lt3);
        return false;
    }

    static inline bool innerSegmentsCross(const PointType& A, const PointType& B, const PointType& P, const PointType& Q)
    {
        int o11, o12, o21, o22;

        o11 = Orient2Dxy(P, A, B);
        o12 = Orient2Dxy(Q, B, A);
        o21 = Orient2Dxy(A, P, Q);
        o22 = Orient2Dxy(B, Q, P);
        if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

        o11 = Orient2Dyz(P, A, B);
        o12 = Orient2Dyz(Q, B, A);
        o21 = Orient2Dyz(A, P, Q);
        o22 = Orient2Dyz(B, Q, P);
        if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

        o11 = Orient2Dzx(P, A, B);
        o12 = Orient2Dzx(Q, B, A);
        o21 = Orient2Dzx(A, P, Q);
        o22 = Orient2Dzx(B, Q, P);
        if (o11 || o21 || o12 || o22) return (o11 == o12 && o21 == o22);

        return false;
    }

    static inline bool lineCrossesInnerTriangle(const PointType& s1, const PointType& s2, const PointType& v1, const PointType& v2, const PointType& v3)
    {
        const int o1 = PointType::Orient3D(s1, s2, v1, v2);
        const int o2 = PointType::Orient3D(s1, s2, v2, v3);
        if ((o1 >= 0 && o2 <= 0) || (o1 <= 0 && o2 >= 0)) return false;
        const int o3 = PointType::Orient3D(s1, s2, v3, v1);
        if ((o1 >= 0 && o3 <= 0) || (o1 <= 0 && o3 >= 0)) return false;
        if ((o2 >= 0 && o3 <= 0) || (o2 <= 0 && o3 >= 0)) return false;
        return true;
    }

    static inline bool innerSegmentCrossesInnerTriangle(const PointType& s1, const PointType& s2, const PointType& v1, const PointType& v2, const PointType& v3)
    {
        int o1 = Orient3D(s1, v1, v2, v3); if (o1 == 0) return false;
        int o2 = Orient3D(s2, v1, v2, v3); if (o2 == 0) return false;

        if ((o1 > 0 && o2 > 0) || (o1 < 0 && o2 < 0)) return false;
        o1 = Orient3D(s1, s2, v1, v2);
        o2 = Orient3D(s1, s2, v2, v3);
        if ((o1 >= 0 && o2 <= 0) || (o1 <= 0 && o2 >= 0)) return false;
        int o3 = Orient3D(s1, s2, v3, v1);
        if ((o1 >= 0 && o3 <= 0) || (o1 <= 0 && o3 >= 0)) return false;
        if ((o2 >= 0 && o3 <= 0) || (o2 <= 0 && o3 >= 0)) return false;
        return true;
    }

    static inline bool pointInInnerTriangle(const PointType& P, const PointType& A, const PointType& B, const PointType& C)
    {
        int o1, o2, o3;
        o1 = Orient2Dxy(P, A, B);
        o2 = Orient2Dxy(P, B, C);
        o3 = Orient2Dxy(P, C, A);
        if (o1 || o2 || o3) return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
        o1 = Orient2Dyz(P, A, B);
        o2 = Orient2Dyz(P, B, C);
        o3 = Orient2Dyz(P, C, A);
        if (o1 || o2 || o3) return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
        o1 = Orient2Dzx(P, A, B);
        o2 = Orient2Dzx(P, B, C);
        o3 = Orient2Dzx(P, C, A);
        return ((o1 > 0 && o2 > 0 && o3 > 0) || (o1 < 0 && o2 < 0 && o3 < 0));
    }

};

typedef PointType ExplicitPoint;
typedef PointType ImplicitPointLnc;

inline std::ostream& operator<<(std::ostream& os, const PointType& p)
{
    return os << GET_DOUBLE_VAL(p[0]) << " " << GET_DOUBLE_VAL(p[1]) << " " << GET_DOUBLE_VAL(p[2]);
}

#endif // USE_INDIRECT_PREDS

#endif
