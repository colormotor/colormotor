#pragma once
#include "cm_gfx.h"

// Trans2d(const cm::M33& f) { x.x=f(0,0);  x.y=f(1,0); \
// y.x=f(0,1);  y.y=f(1,1); \
// pos.x=f(0,2);  pos.y=f(1,2); } \

#define UI_TRANS2D_CLASS_EXTRA \
Trans2d(const arma::mat& f) { x.x=f(0,0);  x.y=f(1,0); \
y.x=f(0,1);  y.y=f(1,1); \
pos.x=f(0,2);  pos.y=f(1,2); } \
        Trans2d(const cm::V2& p, const arma::mat& rot) { x=ImVec2(rot(0,0),rot(1,0));  x=ImVec2(rot(0,1),rot(1,1)); \
pos.x=p.x;  pos.y=p.y; } \
operator cm::M33() const { cm::M33 f; \
f.col(0) = cm::V3(x.x, x.y, 0.0); \
f.col(1) = cm::V3(y.x, y.y, 0.0); \
f.col(2) = cm::V3(pos.x, pos.y, 1.); return f; } \
operator arma::mat() const { arma::mat f; \
    f.col(0) = cm::V3(x.x, x.y, 0.0); \
    f.col(1) = cm::V3(y.x, y.y, 0.0); \
    f.col(2) = cm::V3(pos.x, pos.y, 1.); return f; }

#define UI_RECT_CLASS_EXTRA \
Rect( const cm::Box& r ) { min = r.min(); max = r.max(); } \
operator cm::Box() const { return cm::Box((cm::V2)min, (cm::V2)max); }
