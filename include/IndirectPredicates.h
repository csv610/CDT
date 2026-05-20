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

/* This code was generated automatically. Do not edit unless you exactly   */
/* know what you are doing!                                                */

#include "ImplicitPoint.h"

int dotProductSign2D(double px, double py, double rx, double ry, double qx, double qy);
int dotProductSign3D(double px, double py, double pz, double rx, double ry, double rz, double qx, double qy, double qz);
int incircle(double pax, double pay, double pbx, double pby, double pcx, double pcy, double pdx, double pdy);
int InSphere(double pax, double pay, double paz, double pbx, double pby, double pbz, double pcx, double pcy, double pcz, double pdx, double pdy, double pdz, double pex, double pey, double pez);
int dotProductSign2D_EEI(const GenericPoint& q, double px, double py, double rx, double ry);
int dotProductSign2D_IEE(const GenericPoint& p, double rx, double ry, double qx, double qy);
int dotProductSign2D_IEI(const GenericPoint& p, const GenericPoint& q, double rx, double ry);
int dotProductSign2D_IIE(const GenericPoint& p, const GenericPoint& r, double qx, double qy);
int dotProductSign2D_III(const GenericPoint& p, const GenericPoint& r, const GenericPoint& q);
int dotProductSign3D_EEI(const GenericPoint& q, double px, double py, double pz, double rx, double ry, double rz);
int dotProductSign3D_IEE(const GenericPoint& p, double rx, double ry, double rz, double qx, double qy, double qz);
int dotProductSign3D_IEI(const GenericPoint& p, const GenericPoint& q, double rx, double ry, double rz);
int dotProductSign3D_IIE(const GenericPoint& p, const GenericPoint& r, double qx, double qy, double qz);
int dotProductSign3D_III(const GenericPoint& p, const GenericPoint& r, const GenericPoint& q);
int incirclexy_indirect_IEEE(const GenericPoint& p1, double pbx, double pby, double pcx, double pcy, double pdx, double pdy);
int incirclexy_indirect_IIEE(const GenericPoint& p1, const GenericPoint& p2, double pcx, double pcy, double pdx, double pdy);
int incirclexy_indirect_IIIE(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, double pdx, double pdy);
int incirclexy_indirect_IIII(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, const GenericPoint& p4);
int incircle_indirect_IEEE(const GenericPoint& p1, double pbx, double pby, double pcx, double pcy, double pdx, double pdy);
int incircle_indirect_IIEE(const GenericPoint& p1, const GenericPoint& p2, double pcx, double pcy, double pdx, double pdy);
int incircle_indirect_IIIE(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, double pdx, double pdy);
int incircle_indirect_IIII(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, const GenericPoint& p4);
int InSphere_IEEEE(const GenericPoint& p1, double pbx, double pby, double pbz, double pcx, double pcy, double pcz, double pdx, double pdy, double pdz, double pex, double pey, double pez);
int InSphere_IIEEE(const GenericPoint& p1, const GenericPoint& p2, double pcx, double pcy, double pcz, double pdx, double pdy, double pdz, double pex, double pey, double pez);
int InSphere_IIIEE(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, double pdx, double pdy, double pdz, double pex, double pey, double pez);
int InSphere_IIIIE(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, const GenericPoint& p4, double pex, double pey, double pez);
int InSphere_IIIII(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, const GenericPoint& p4, const GenericPoint& p5);
bool lambda2d_SSI_interval(IntervalNumber ea1x, IntervalNumber ea1y, IntervalNumber ea2x, IntervalNumber ea2y, IntervalNumber eb1x, IntervalNumber eb1y, IntervalNumber eb2x, IntervalNumber eb2y, IntervalNumber& lambda_x, IntervalNumber& lambda_y, IntervalNumber& lambda_det);
void lambda2d_SSI_exact(double ea1x, double ea1y, double ea2x, double ea2y, double eb1x, double eb1y, double eb2x, double eb2y, double **lambda_x, int& lambda_x_len, double **lambda_y, int& lambda_y_len, double **lambda_det, int& lambda_det_len);
void lambda2d_SSI_BigFloat(BigFloat ea1x, BigFloat ea1y, BigFloat ea2x, BigFloat ea2y, BigFloat eb1x, BigFloat eb1y, BigFloat eb2x, BigFloat eb2y, BigFloat& lambda_x, BigFloat& lambda_y, BigFloat& lambda_det);
bool lambda3d_LNC_interval(IntervalNumber px, IntervalNumber py, IntervalNumber pz, IntervalNumber qx, IntervalNumber qy, IntervalNumber qz, IntervalNumber t, IntervalNumber& lambda_x, IntervalNumber& lambda_y, IntervalNumber& lambda_z, IntervalNumber& lambda_d);
void lambda3d_LNC_exact(double px, double py, double pz, double qx, double qy, double qz, double t, double **lambda_x, int& lambda_x_len, double **lambda_y, int& lambda_y_len, double **lambda_z, int& lambda_z_len, double **lambda_d, int& lambda_d_len);
void lambda3d_LNC_BigFloat(BigFloat px, BigFloat py, BigFloat pz, BigFloat qx, BigFloat qy, BigFloat qz, BigFloat t, BigFloat& lambda_x, BigFloat& lambda_y, BigFloat& lambda_z, BigFloat& lambda_d);
bool lambda3d_LPI_interval(IntervalNumber px, IntervalNumber py, IntervalNumber pz, IntervalNumber qx, IntervalNumber qy, IntervalNumber qz, IntervalNumber rx, IntervalNumber ry, IntervalNumber rz, IntervalNumber sx, IntervalNumber sy, IntervalNumber sz, IntervalNumber tx, IntervalNumber ty, IntervalNumber tz, IntervalNumber& lambda_x, IntervalNumber& lambda_y, IntervalNumber& lambda_z, IntervalNumber& lambda_d);
void lambda3d_LPI_exact(double px, double py, double pz, double qx, double qy, double qz, double rx, double ry, double rz, double sx, double sy, double sz, double tx, double ty, double tz, double **lambda_x, int& lambda_x_len, double **lambda_y, int& lambda_y_len, double **lambda_z, int& lambda_z_len, double **lambda_d, int& lambda_d_len);
void lambda3d_LPI_BigFloat(BigFloat px, BigFloat py, BigFloat pz, BigFloat qx, BigFloat qy, BigFloat qz, BigFloat rx, BigFloat ry, BigFloat rz, BigFloat sx, BigFloat sy, BigFloat sz, BigFloat tx, BigFloat ty, BigFloat tz, BigFloat& lambda_x, BigFloat& lambda_y, BigFloat& lambda_z, BigFloat& lambda_d);
bool lambda3d_TPI_interval(IntervalNumber ov1x, IntervalNumber ov1y, IntervalNumber ov1z, IntervalNumber ov2x, IntervalNumber ov2y, IntervalNumber ov2z, IntervalNumber ov3x, IntervalNumber ov3y, IntervalNumber ov3z, IntervalNumber ow1x, IntervalNumber ow1y, IntervalNumber ow1z, IntervalNumber ow2x, IntervalNumber ow2y, IntervalNumber ow2z, IntervalNumber ow3x, IntervalNumber ow3y, IntervalNumber ow3z, IntervalNumber ou1x, IntervalNumber ou1y, IntervalNumber ou1z, IntervalNumber ou2x, IntervalNumber ou2y, IntervalNumber ou2z, IntervalNumber ou3x, IntervalNumber ou3y, IntervalNumber ou3z, IntervalNumber& lambda_x, IntervalNumber& lambda_y, IntervalNumber& lambda_z, IntervalNumber& lambda_d);
void lambda3d_TPI_exact(double ov1x, double ov1y, double ov1z, double ov2x, double ov2y, double ov2z, double ov3x, double ov3y, double ov3z, double ow1x, double ow1y, double ow1z, double ow2x, double ow2y, double ow2z, double ow3x, double ow3y, double ow3z, double ou1x, double ou1y, double ou1z, double ou2x, double ou2y, double ou2z, double ou3x, double ou3y, double ou3z, double **lambda_x, int& lambda_x_len, double **lambda_y, int& lambda_y_len, double **lambda_z, int& lambda_z_len, double **lambda_d, int& lambda_d_len);
void lambda3d_TPI_BigFloat(BigFloat ov1x, BigFloat ov1y, BigFloat ov1z, BigFloat ov2x, BigFloat ov2y, BigFloat ov2z, BigFloat ov3x, BigFloat ov3y, BigFloat ov3z, BigFloat ow1x, BigFloat ow1y, BigFloat ow1z, BigFloat ow2x, BigFloat ow2y, BigFloat ow2z, BigFloat ow3x, BigFloat ow3y, BigFloat ow3z, BigFloat ou1x, BigFloat ou1y, BigFloat ou1z, BigFloat ou2x, BigFloat ou2y, BigFloat ou2z, BigFloat ou3x, BigFloat ou3y, BigFloat ou3z, BigFloat& lambda_x, BigFloat& lambda_y, BigFloat& lambda_z, BigFloat& lambda_d);
int lessThanOnX_IE(const GenericPoint& p1, double bx);
int lessThanOnX_II(const GenericPoint& p1, const GenericPoint& p2);
int lessThanOnY_IE(const GenericPoint& p1, double by);
int lessThanOnY_II(const GenericPoint& p1, const GenericPoint& p2);
int lessThanOnZ_IE(const GenericPoint& p1, double bz);
int lessThanOnZ_II(const GenericPoint& p1, const GenericPoint& p2);
int orient2dxy_indirect_IEE(const GenericPoint& p1, double p2x, double p2y, double p3x, double p3y);
int orient2dxy_indirect_IIE(const GenericPoint& p1, const GenericPoint& p2, double op3x, double op3y);
int orient2dxy_indirect_III(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3);
int orient2dyz_indirect_IEE(const GenericPoint& p1, double p2x, double p2y, double p3x, double p3y);
int orient2dyz_indirect_IIE(const GenericPoint& p1, const GenericPoint& p2, double op3x, double op3y);
int orient2dyz_indirect_III(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3);
int orient2dzx_indirect_IEE(const GenericPoint& p1, double p2x, double p2y, double p3x, double p3y);
int orient2dzx_indirect_IIE(const GenericPoint& p1, const GenericPoint& p2, double op3x, double op3y);
int orient2dzx_indirect_III(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3);
int orient2d_indirect_IEE(const GenericPoint& p1, double p2x, double p2y, double p3x, double p3y);
int orient2d_indirect_IIE(const GenericPoint& p1, const GenericPoint& p2, double p3x, double p3y);
int orient2d_indirect_III(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3);
int orient3d_indirect_IEEE(const GenericPoint& p1, double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz);
int orient3d_indirect_IIEE(const GenericPoint& p1, const GenericPoint& p2, double p3x, double p3y, double p3z, double p4x, double p4y, double p4z);
int orient3d_indirect_IIIE(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, double p4x, double p4y, double p4z);
int orient3d_indirect_IIII(const GenericPoint& p1, const GenericPoint& p2, const GenericPoint& p3, const GenericPoint& p4);

#include "IndirectPredicates.hpp" 
