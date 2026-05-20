/****************************************************************************
* Indirect predicates for geometric constructions					        *
*                                                                           *
* Consiglio Nazionale delle Ricerche                                        *
* Istituto di Matematica Applicata e Tecnologie Informatiche                *
* Sezione di Genova                                                         *
* IMATI-GE / CNR                                                            *
*                                                                           *
* Authors: Marco Attene                                                     *
* Copyright(C) 2019: IMATI-GE / CNR                                         *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation; either version 3 of the License, or (at  *
* your option) any later version.                                           *
*                                                                           *
* This program is distributed in the hope that it will be useful, but       *
* WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser  *
* General Public License for more details.                                  *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with this program.  If not, see http://www.gnu.org/licenses/.       *
*                                                                           *
****************************************************************************/

#ifndef IMPLICIT_POINT_H
#define IMPLICIT_POINT_H

#include "Numerics.h"
#include <iostream>

// An indirect predicate can assume one of the following values.
// UNDEFINED means that input parameters are degenerate and do not define an
// implicit point.
enum IP_Sign {
	ZERO = 0,
	POSITIVE = 1,
	NEGATIVE = -1,
	UNDEFINED = 2
};

// A filtered indirect predicate can assume an UNCERTAIN value.
// This means that precision is not enough to reach a conclusion.
enum Filtered_Sign {
	UNCERTAIN = 0
};

// Indirect predicates operate on points of the following types
enum class PointKind {
	UNDEF = 0,
	EXPLICIT2D = 1,
	SSI = 2, // This must be the last 2D config in this ordered list
	EXPLICIT3D = 3,
	LPI = 4, // Line-plane intersection
	TPI = 5, // Three-planes intersection
	LNC = 6  // Linear combination
};

// This is a generic point. It can be extended as either explicit or implicit point
class GenericPoint {
protected:
	PointKind type;

public:
	GenericPoint(const PointKind& t) : type(t) {}

	PointKind getType() const { return type; }
	bool is2D() const { return type <= PointKind::SSI; }
	bool is3D() const { return type > PointKind::SSI; }
	bool isExplicit2D() const { return (type == PointKind::EXPLICIT2D); }
	bool isExplicit3D() const { return (type == PointKind::EXPLICIT3D); }
	bool isSSI() const { return (type == PointKind::SSI); }
	bool isLPI() const { return (type == PointKind::LPI); }
	bool isTPI() const { return (type == PointKind::TPI); }
	bool isLNC() const { return (type == PointKind::LNC); }

	// The following functions convert to explicit points.
	// Use only after having verified the correct type through getType()
	//
	// Note that these violate strict aliasing rules, which has two consequences:
	// 1) Many compilers issue annoying warnings
	// 2) The optimizer may do wrong assumptions
	//
	// I do not see how an optimizer may spoil these simple functions or code around them,
	// but if you experience strange behaviour you may try replacing the type casts
	// with memcpy to an object and hope that the optimizer recognizes that an actual
	// copy is not necessary.
	class ExplicitPoint2D& toExplicit2D() { return (ExplicitPoint2D&)(*this); }
	class ImplicitPoint2dSsi& toSSI() { return (ImplicitPoint2dSsi&)(*this); }
	class ExplicitPoint3D& toExplicit3D() { return (ExplicitPoint3D&)(*this); }
	class ImplicitPoint3dLpi& toLPI() { return (ImplicitPoint3dLpi&)(*this); }
	class ImplicitPoint3dTpi& toTPI() { return (ImplicitPoint3dTpi&)(*this); }
	class ImplicitPoint3dLnc& toLNC() { return (ImplicitPoint3dLnc&)(*this); }

	const class ExplicitPoint2D& toExplicit2D() const { return (ExplicitPoint2D&)(*this); }
	const class ImplicitPoint2dSsi& toSSI() const { return (ImplicitPoint2dSsi&)(*this); }
	const class ExplicitPoint3D& toExplicit3D() const { return (ExplicitPoint3D&)(*this); }
	const class ImplicitPoint3dLpi& toLPI() const { return (ImplicitPoint3dLpi&)(*this); }
	const class ImplicitPoint3dTpi& toTPI() const { return (ImplicitPoint3dTpi&)(*this); }
	const class ImplicitPoint3dLnc& toLNC() const { return (ImplicitPoint3dLnc&)(*this); }

	// Calculates the first two cartesian coordinates. If the point is implicit, these
	// coordinates are approximated due to floating point roundoff.
	// If apap==true, the approximation is as precise as possible (slightly slower).
	// Returns 0 if the implicit point is undefined.
	bool getApproxXYCoordinates(double& x, double& y, bool apap =false) const;

	// Calculates the three cartesian coordinates. If the point is implicit, these
	// coordinates are approximated due to floating point roundoff.
	// If apap==true, the approximation is as precise as possible (slightly slower).
	// Returns 0 if the point is not 3D or if the implicit point is undefined.
	bool getApproxXYZCoordinates(double& x, double& y, double& z, bool apap = false) const;

	// Calculates the two/three cartesian coordinates exactly.
	bool getExactXYCoordinates(BigRational& x, BigRational& y) const;
	bool getExactXYZCoordinates(BigRational& x, BigRational& y, BigRational& z) const;

	std::string get_str() const {
		BigRational x, y, z;
		if (!getExactXYZCoordinates(x, y, z)) return "UNDEFINED_GENERIC_POINT";
		return x.get_str() + " " + y.get_str() + " " + z.get_str();
	}

	// These are the indirect predicates supported up to now.
	// In each predicate, it is assumed that input points are either all 2D or all 3D
	// as expected. No check is performed. Passing wrong implicit points may result in
	// unpredictable behaviour.

	// Orient2D - fully supported
	// Input points can be any combination of 2D points.
	static int Orient2D(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);

	// Orient2Dxy (resp. Orient2Dyz, Orient2Dzx) - fully supported
	// Input points can be any combination of 3D points. 
	// Orientation is computed on XY (resp. YZ, ZX).
	static int Orient2Dxy(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);
	static int Orient2Dyz(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);
	static int Orient2Dzx(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);

	// Orient3D - fully supported
	// Input can be any combination of 3D points
	static int Orient3D(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c, const GenericPoint& d);

	// InSphere - fully supported
	// Input can be any combination of 3D points
	static int InSphere(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c, const GenericPoint& d, const GenericPoint& e);

	// incircle - fully supported
	// Input can be any combination of 2D points
	static int incircle(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c, const GenericPoint& d);

	// incirclexy - fully supported
	// Input can be any combination of 3D points
	static int incirclexy(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c, const GenericPoint& d);

	// Sign of (a-c) dot (b-c)
	static int dotProductSign2D(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);
	static int dotProductSign3D(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c);

	// lessThanOnX (resp. Y, Z) - fully supported (only 3D)
	// Input points can be any combination of 3D points
	// lessThanOnX(a,b) =
	// -1 - if a.X < b.X
	// 0  - if a.X == b.X
	// 1  - if a.X > b.X
	static int lessThanOnX(const GenericPoint& a, const GenericPoint& b);
	static int lessThanOnY(const GenericPoint& a, const GenericPoint& b);
	static int lessThanOnZ(const GenericPoint& a, const GenericPoint& b);

	// lessThan - fully supported (only 3D)
	// Input points can be any combination of 3D points
	// lessThan(a,b) =
	// -1 - if a < b
	// 0  - if a == b
	// 1  - if a > b
	// in lexicographical order
	static int lessThan(const GenericPoint& a, const GenericPoint& b);

	// TRUE if the two points are coincident
	static bool coincident(const GenericPoint& a, const GenericPoint& b) { return lessThan(a, b) == 0; }

	// Let n = (x,y,z) be the normal of the triangle <v1,v2,v3>
	// and let m be the absolute value of its largest component.
	// That is, m = max(|x|, |y|, |z|).
	// maxComponentInTriangleNormal(v1,v2,v3) returns:
	// 0 - if m == |x|
	// 1 - if m == |y|
	// 2 - if m == |z|
	//
	// Warning: this function assumes that the triangle is not exactly degenerate. It may crash otherwise.
	static int maxComponentInTriangleNormal(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z);

	// TRUE if A-B-C are not collinear
	static bool misaligned(const GenericPoint& A, const GenericPoint& B, const GenericPoint& C) {
		return (Orient2Dxy(A, B, C) || Orient2Dyz(A, B, C) || Orient2Dzx(A, B, C));
	}

	// TRUE if 'p' is in the interior of v1-v2
	static bool pointInInnerSegment(const GenericPoint& p, const GenericPoint& v1, const GenericPoint& v2);

	// TRUE if 'p' is in the closure of v1-v2
	static bool pointInSegment(const GenericPoint& p, const GenericPoint& v1, const GenericPoint& v2);

	// TRUE if P is in the interior of <A,B,C>
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool pointInInnerTriangle(const GenericPoint& P, const GenericPoint& A, const GenericPoint& B, const GenericPoint& C);

	// TRUE if P is in the closure of <A,B,C>
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool pointInTriangle(const GenericPoint& P, const GenericPoint& A, const GenericPoint& B, const GenericPoint& C);
	// Same as above, but this version initializes oAB, oAC and oCA with the orientation of P wrt one of the edges (0 = on edge)
	static bool pointInTriangle(const GenericPoint& P, const GenericPoint& A, const GenericPoint& B, const GenericPoint& C, int& oAB, int& oBC, int& oCA);

	// TRUE if the interior of A-B intersects the interior of P-Q at a single point
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool innerSegmentsCross(const GenericPoint& A, const GenericPoint& B, const GenericPoint& P, const GenericPoint& Q);

	// TRUE if the closure of A-B intersects the closure of P-Q at a single point
	// Points are assumed to be coplanar. Undetermined otherwise.
	static bool segmentsCross(const GenericPoint& A, const GenericPoint& B, const GenericPoint& P, const GenericPoint& Q);

	// TRUE if interior of s1-s2 intersects interior of <v1,v2,v3> at a single point
	static bool innerSegmentCrossesInnerTriangle(const GenericPoint& s1, const GenericPoint& s2, const GenericPoint& v1, const GenericPoint& v2, const GenericPoint& v3);

	// TRUE if the infinite straight line by s1-s2 intersects triangle <v1,v2,v3> at a single internal point
	static bool lineCrossesInnerTriangle(const GenericPoint& s1, const GenericPoint& s2, const GenericPoint& v1, const GenericPoint& v2, const GenericPoint& v3);

	// TRUE if the infinite straight line by s1-s2 intersects triangle <v1,v2,v3> at a single point
	static bool lineCrossesTriangle(const GenericPoint& s1, const GenericPoint& s2, const GenericPoint& v1, const GenericPoint& v2, const GenericPoint& v3);

	// TRUE if interior of s1-s2 intersects <v1,v2,v3> at a single point
	static bool innerSegmentCrossesTriangle(const GenericPoint& s1, const GenericPoint& s2, const GenericPoint& v1, const GenericPoint& v2, const GenericPoint& v3);

    // The following methods are equivalent to the corresponding functions hereabove,
	// but faster. They assume that points are coplanar and the dominant normal component 
	// is n_max (see maxComponentInTriangleNormal()).
	static int Orient2D(const GenericPoint& a, const GenericPoint& b, const GenericPoint& c, int n_max)
	{
		if (n_max == 0) return Orient2Dyz(a, b, c);
		else if (n_max == 1) return Orient2Dzx(a, b, c);
		else return Orient2Dxy(a, b, c);
	}

	static bool misaligned(const GenericPoint& A, const GenericPoint& B, const GenericPoint& C, int n_max)
	{
		return ((n_max == 2 && Orient2Dxy(A, B, C)) || (n_max == 0 && Orient2Dyz(A, B, C)) || (n_max == 1 && Orient2Dzx(A, B, C)));
	}

	static bool pointInInnerSegment(const GenericPoint& p, const GenericPoint& v1, const GenericPoint& v2, int n_max);
	static bool pointInSegment(const GenericPoint& p, const GenericPoint& v1, const GenericPoint& v2, int n_max);
	static bool pointInInnerTriangle(const GenericPoint& P, const GenericPoint& A, const GenericPoint& B, const GenericPoint& C, int n_max);
	static bool pointInTriangle(const GenericPoint& P, const GenericPoint& A, const GenericPoint& B, const GenericPoint& C, int n_max);
	static bool innerSegmentsCross(const GenericPoint& A, const GenericPoint& B, const GenericPoint& P, const GenericPoint& Q, int n_max);
	static bool segmentsCross(const GenericPoint& A, const GenericPoint& B, const GenericPoint& P, const GenericPoint& Q, int n_max);

	// Calculates an explicit approximation of the implicit point.
    // Returns false if point is undefined
	bool approxExplicit(class ExplicitPoint2D&) const;
	bool approxExplicit(class ExplicitPoint3D&) const;

	// Same as above, but the approximation is as precise as possible.
	// Slightly slower.
	bool apapExplicit(class ExplicitPoint2D&) const;
	bool apapExplicit(class ExplicitPoint3D&) const;


	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber& d) const;
	void getExactLambda(double** lx, int& lxl, double** ly, int& lyl, double** d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& d) const;
	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber& lz, IntervalNumber& d) const;
	void getExactLambda(double** lx, int& lxl, double** ly, int& lyl, double** lz, int& lzl, double** d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& lz, BigFloat& d) const;
};


///////////////////////////////////////////////////////////////////////////////////
//
// 2 D   P O I N T S
//
///////////////////////////////////////////////////////////////////////////////////

class ExplicitPoint2D : public GenericPoint {
	double x, y;

public:
	ExplicitPoint2D() : GenericPoint(PointKind::EXPLICIT2D) {}
	ExplicitPoint2D(double _x, double _y) : GenericPoint(PointKind::EXPLICIT2D), x(_x), y(_y) {}
	ExplicitPoint2D(const ExplicitPoint2D& b) : GenericPoint(PointKind::EXPLICIT2D), x(b.x), y(b.y) {}

	void operator=(const ExplicitPoint2D& b) { type = PointKind::EXPLICIT2D; x = b.x; y = b.y; }
	bool operator==(const ExplicitPoint2D& e) const { return x == e.x && y == e.y; }
	void set(double a, double b) { x = a; y = b; }

	double X() const { return x; }
	double Y() const { return y; }

	const double* ptr() const { return &x; }

	bool getExactXYCoordinates(BigRational& _x, BigRational& _y) const { _x = BigFloat(x); _y = BigFloat(y); return true; }
};


// Implicit 2D point defined by the intersection of two lines l1 and l2
class ImplicitPoint2dSsi : public GenericPoint{
	const ExplicitPoint2D &l1_1, &l1_2, &l2_1, &l2_2;

public:
	ImplicitPoint2dSsi(const ExplicitPoint2D& l11, const ExplicitPoint2D& l12,
		const ExplicitPoint2D& l21, const ExplicitPoint2D& l22);

	const ExplicitPoint2D& L1_1() const { return l1_1; }
	const ExplicitPoint2D& L1_2() const { return l1_2; }
	const ExplicitPoint2D& L2_1() const { return l2_1; }
	const ExplicitPoint2D& L2_2() const { return l2_2; }

private: // Cached values
	mutable IntervalNumber dfilter_lambda_x, dfilter_lambda_y, dfilter_denominator;
	bool needsIntervalLambda() const { return (dfilter_denominator.isNAN()); } // TRUE if NAN

public:
	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber &d) const;
	void getExactLambda(double **lx, int& lxl, double **ly, int& lyl, double **d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& d) const;
	bool getExactXYCoordinates(BigRational& x, BigRational& y) const;
};


///////////////////////////////////////////////////////////////////////////////////
//
// 3 D   P O I N T S
//
///////////////////////////////////////////////////////////////////////////////////

class ExplicitPoint3D : public GenericPoint {
	double x, y, z;

public:
	ExplicitPoint3D() : GenericPoint(PointKind::EXPLICIT3D) {}
	ExplicitPoint3D(double _x, double _y, double _z) : GenericPoint(PointKind::EXPLICIT3D), x(_x), y(_y), z(_z) {}
	ExplicitPoint3D(const ExplicitPoint3D& b) : GenericPoint(PointKind::EXPLICIT3D), x(b.x), y(b.y), z(b.z) {}

	void operator=(const ExplicitPoint3D& b) { type = PointKind::EXPLICIT3D; x = b.x; y = b.y; z = b.z; }
	bool operator==(const ExplicitPoint3D& e) const { return x == e.x && y == e.y && z == e.z; }
	void set(double a, double b, double c) { x = a; y = b; z = c; }

	double X() const { return x; }
	double Y() const { return y; }
	double Z() const { return z; }

	const double* ptr() const { return &x; }

	bool getExactXYZCoordinates(BigRational& _x, BigRational& _y, BigRational& _z) const { _x = BigFloat(x); _y = BigFloat(y); _z = BigFloat(z); return true; }
};

// Implicit point defined by the intersection of a line and a plane
class ImplicitPoint3dLpi : public GenericPoint{
	const ExplicitPoint3D &ip, &iq; // The line
	const ExplicitPoint3D &ir, &is, &it; // The plane

public:
	ImplicitPoint3dLpi(const ExplicitPoint3D& _p, const ExplicitPoint3D& _q,
		const ExplicitPoint3D& _r, const ExplicitPoint3D& _s, const ExplicitPoint3D& _t);

	const ExplicitPoint3D& P() const { return ip; }
	const ExplicitPoint3D& Q() const { return iq; }
	const ExplicitPoint3D& R() const { return ir; }
	const ExplicitPoint3D& S() const { return is; }
	const ExplicitPoint3D& T() const { return it; }

private: // Cached values
	IntervalNumber dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator;

public:
	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber& lz, IntervalNumber &d) const;
	void getExactLambda(double **lx, int& lxl, double **ly, int& lyl, double **lz, int& lzl, double **d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& lz, BigFloat& d) const;
	bool getExactXYZCoordinates(BigRational& x, BigRational& y, BigRational& z) const;
};


// Implicit point defined by the intersection of three planes
class ImplicitPoint3dTpi : public GenericPoint{
	const ExplicitPoint3D &iv1, &iv2, &iv3; // Plane 1
	const ExplicitPoint3D &iw1, &iw2, &iw3; // Plane 2
	const ExplicitPoint3D &iu1, &iu2, &iu3; // Plane 3

public:
	ImplicitPoint3dTpi(const ExplicitPoint3D& _v1, const ExplicitPoint3D& _v2, const ExplicitPoint3D& _v3,
		const ExplicitPoint3D& _w1, const ExplicitPoint3D& _w2, const ExplicitPoint3D& _w3,
		const ExplicitPoint3D& _u1, const ExplicitPoint3D& _u2, const ExplicitPoint3D& _u3);

	const ExplicitPoint3D& V1() const { return iv1; }
	const ExplicitPoint3D& V2() const { return iv2; }
	const ExplicitPoint3D& V3() const { return iv3; }
	const ExplicitPoint3D& W1() const { return iw1; }
	const ExplicitPoint3D& W2() const { return iw2; }
	const ExplicitPoint3D& W3() const { return iw3; }
	const ExplicitPoint3D& U1() const { return iu1; }
	const ExplicitPoint3D& U2() const { return iu2; }
	const ExplicitPoint3D& U3() const { return iu3; }

private: // Cached values
	IntervalNumber dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator;

public:
	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber& lz, IntervalNumber &d) const;
	void getExactLambda(double **lx, int& lxl, double **ly, int& lyl, double **lz, int& lzl, double **d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& lz, BigFloat& d) const;
	bool getExactXYZCoordinates(BigRational& x, BigRational& y, BigRational& z) const;
};


// Implicit point defined as a linear combination of two points
class ImplicitPoint3dLnc : public GenericPoint {
	const ExplicitPoint3D& ip, & iq; // The two points
	const double t; // The parameter (0 = ip, 1 = iq)

public:
	ImplicitPoint3dLnc(const ExplicitPoint3D& _p, const ExplicitPoint3D& _q,
		const double _t);

	const ExplicitPoint3D& P() const { return ip; }
	const ExplicitPoint3D& Q() const { return iq; }
	const double T() const { return t; }

private: // Cached values
	IntervalNumber dfilter_lambda_x, dfilter_lambda_y, dfilter_lambda_z, dfilter_denominator;

public:
	bool getIntervalLambda(IntervalNumber& lx, IntervalNumber& ly, IntervalNumber& lz, IntervalNumber& d) const;
	void getExactLambda(double** lx, int& lxl, double** ly, int& lyl, double** lz, int& lzl, double** d, int& dl) const;
	void getBigfloatLambda(BigFloat& lx, BigFloat& ly, BigFloat& lz, BigFloat& d) const;
	bool getExactXYZCoordinates(BigRational& x, BigRational& y, BigRational& z) const;
};


//////////////////////////////////////////////////////////////////////////////////////
//
// OUTPUT TO STD STREAMS
//
//////////////////////////////////////////////////////////////////////////////////////

using namespace ::std;

ostream& operator<<(ostream& os, const GenericPoint& p);

inline ostream& operator<<(ostream& os, const ExplicitPoint2D& p)
{
	return os << p.X() << " " << p.Y() << " 0";
}

inline ostream& operator<<(ostream& os, const ImplicitPoint2dSsi& p)
{
	ExplicitPoint2D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEFINED_SSI";
}

inline ostream& operator<<(ostream& os, const ExplicitPoint3D& p)
{
	return os << p.X() << " " << p.Y() << " " << p.Z();
}

inline ostream& operator<<(ostream& os, const ImplicitPoint3dLpi& p)
{
	ExplicitPoint3D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEFINED_LPI";
}

inline ostream& operator<<(ostream& os, const ImplicitPoint3dTpi& p)
{
	ExplicitPoint3D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEFINED_TPI";
}

inline ostream& operator<<(ostream& os, const ImplicitPoint3dLnc& p)
{
	ExplicitPoint3D e;
	if (p.apapExplicit(e)) return os << e;
	else return os << "UNDEFINED_LNC";
}

#include "hand_optimized_predicates.hpp"
#include "ImplicitPoint.hpp"

#endif // IMPLICIT_POINT_H
