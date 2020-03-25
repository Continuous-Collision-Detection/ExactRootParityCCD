#include "double_Utils.hpp"
#include <predicates/indirect_predicates.h>
#include <fstream>

namespace ccd {
	bilinear::bilinear(
		const Vector3d& v0,
		const Vector3d& v1,
		const Vector3d& v2,
		const Vector3d& v3)
	{
		v = { { v0, v1, v2, v3 } };
		int ori = orient_3d(
			v0, v1, v2, v3);
		if (ori == 0) {
			is_degenerated = true;
		}
		else {
			is_degenerated = false;
		}
		if (ori >= 0) {
			facets.resize(4);// right hand outside order
			facets[0] = { { 1,2,0 } }; // 0,1 are one pair
			facets[1] = { { 3,0,2 } };

			facets[2] = { { 0,3,1 } }; // 2,3 are one pair
			facets[3] = { { 2,1,3 } };
		}
		if (ori == -1) {
			facets.resize(4);
			facets[0] = { { 1,0,2 } }; // 0,1 are one pair
			facets[1] = { { 3,2,0 } };

			facets[2] = { { 0,1,3 } }; // 2,3 are one pair
			facets[3] = { { 2,3,1 } };
		}
	}

	bool same_point(const Vector3d& p1, const Vector3d& p2)
	{
		if (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]) {
			return true;
		}
		return false;
	}
	int dot_product_sign(const Vector3d& a, const Vector3d& b) {
		xx//double n = a.dot(b);
		//if (n > 1e-8) return 1;
		//if (n < -1e-8) return -1;
		//expansionObject o;
		//double ab0[2], ab1[2], ab2[2];
		//double zero2[2]; zero2[0] = 0; zero2[1] = 0;
		//o.Two_Prod(a[0], b[0], ab0);
		//o.Two_Prod(a[1], b[1], ab1);
		//o.Two_Prod(a[2], b[2], ab2);
		//double sum1[4];
		//int nbr1 = o.Gen_Sum(2, ab0, 2, ab1, sum1);
		//double *sum2;
		//int nbr2= o.Gen_Sum(2, ab2, nbr1, sum1, sum2);// i really should not implement this
		//if
	}
	//int orient2d(
	//    const Vector3r& a, const Vector3r& b, const Vector3r& c, const int axis)
	//{
	//    Vector3r v1, v2;
	//    v1 = a - b;
	//    v2 = c - b;
	//    int i1, i2;
	//    if (axis == 0) {
	//        i1 = 1;
	//        i2 = 2;
	//    }
	//    if (axis == 1) {
	//        i1 = 0;
	//        i2 = 2;
	//    }
	//    if (axis == 2) {
	//        i1 = 0;
	//        i2 = 1;
	//    }
	//    const Rational det = v1[i1] * v2[i2] - v1[i2] * v2[i1];
	//    return det.get_sign();
	//}
	Rational phi(const Vector3d x, const std::array<Vector3d, 4>& corners)
	{
		static const std::array<int, 4> vv = { { 0, 1, 2, 3 } };
		const Rational g012 = func_g(x, corners, { { vv[0], vv[1], vv[2] } });
		const Rational g132 = func_g(x, corners, { { vv[1], vv[3], vv[2] } });
		const Rational g013 = func_g(x, corners, { { vv[0], vv[1], vv[3] } });
		const Rational g032 = func_g(x, corners, { { vv[0], vv[3], vv[2] } });

		const Rational h12 = g012 * g032;
		const Rational h03 = g132 * g013;

		const Rational phi = h12 - h03;

		return phi;
	}
	
	
	Rational func_g(
		const Vector3d& x,
		const std::array<Vector3d, 4>& corners,
		const std::array<int, 3>& indices)
	{
		const int p = indices[0];
		const int q = indices[1];
		const int r = indices[2];
		Vector3r xr(x[0], x[1], x[2]), pr(corners[p][0], corners[p][1], corners[p][2]), 
			qr(corners[q][0], corners[q][1], corners[q][2]), rr(corners[r][0], corners[r][1], corners[r][2]);
		return (xr - pr)
			.dot(cross(qr - pr, rr - pr));// TODO maybe minus is not allowed
	}
	bool int_seg_XOR(const int a, const int b)
	{
		if (a == 2 || b == 2) return true;
		if (a == 0 && b == 1) return true;
		if (a == 1 && b == 0) return true;
		if (a == 3 || b == 3) return false;
		if (a == b) return false;
		return false;
		/*if (a == -1 || b == -1)
			return -1;
		if (a == b)
			return 0;
		if (a == 0)
			return b;
		if (b == 0)
			return a;
		std::cout << "impossible XOR cases" << std::endl;*/
	}
	int int_ray_XOR(const int a, const int b) {
		if (a == -1 || b == -1) return -1;
		if (a == 0) return b;
		if (b == 0) return a;
		if (a == b) return 0;
		if (a == 2 || b == 2) return 2;//this is case 2-3
		std::cout << "impossible to go here " << std::endl;
		return -1;
	}
	
	int is_line_cut_triangle(
		const Vector3d& e0,
		const Vector3d& e1,
		const Vector3d& t1,
		const Vector3d& t2,
		const Vector3d& t3,
		const bool halfopen) {

		////if (orient3d(n, t1, t2, t3) == 0) {
		//	//std::cout << "Degeneration happens" << std::endl;
		//	n = Vector3r(rand(), rand(), rand());
		//}

		Vector3d np = Vector3d::Random();
		while (orient_3d(np, t1, t2, t3) == 0) {// if coplanar, random
			np = Vector3d::Random();
		}

		explicitPoint3D p(e0[0], e0[1], e0[2]);
		explicitPoint3D q(e1[0], e1[1], e1[2]);
		explicitPoint3D a(t1[0], t1[1], t1[2]);
		explicitPoint3D b(t2[0], t2[1], t2[2]);
		explicitPoint3D c(t3[0], t3[1], t3[2]);
		implicitPoint3D_LPI l(p, q, a, b, c);
		if (genericPoint::pointInInnerTriangle(l, a, b, c))
			return 1;
		if (genericPoint::pointInSegment(l, a, b))
			return 2;
		if (genericPoint::pointInSegment(l, a, c))
			return 2;
		if (genericPoint::pointInSegment(l, b, c))
			if (halfopen)
				return 3;// on open edge t2-t3
			else
				return 2;

		return 0;
	}
	int line_triangle_inter_return_t(
		const Vector3r& e0,
		const Vector3r& e1,
		const Vector3r& t1,
		const Vector3r& t2,
		const Vector3r& t3,
		Rational& t)

	{
		const Rational d = e0[0] * t1[1] * t2[2] - e0[0] * t1[1] * t3[2]
			- e0[0] * t1[2] * t2[1] + e0[0] * t1[2] * t3[1] + e0[0] * t2[1] * t3[2]
			- e0[0] * t2[2] * t3[1] - e0[1] * t1[0] * t2[2] + e0[1] * t1[0] * t3[2]
			+ e0[1] * t1[2] * t2[0] - e0[1] * t1[2] * t3[0] - e0[1] * t2[0] * t3[2]
			+ e0[1] * t2[2] * t3[0] + e0[2] * t1[0] * t2[1] - e0[2] * t1[0] * t3[1]
			- e0[2] * t1[1] * t2[0] + e0[2] * t1[1] * t3[0] + e0[2] * t2[0] * t3[1]
			- e0[2] * t2[1] * t3[0] - e1[0] * t1[1] * t2[2] + e1[0] * t1[1] * t3[2]
			+ e1[0] * t1[2] * t2[1] - e1[0] * t1[2] * t3[1] - e1[0] * t2[1] * t3[2]
			+ e1[0] * t2[2] * t3[1] + e1[1] * t1[0] * t2[2] - e1[1] * t1[0] * t3[2]
			- e1[1] * t1[2] * t2[0] + e1[1] * t1[2] * t3[0] + e1[1] * t2[0] * t3[2]
			- e1[1] * t2[2] * t3[0] - e1[2] * t1[0] * t2[1] + e1[2] * t1[0] * t3[1]
			+ e1[2] * t1[1] * t2[0] - e1[2] * t1[1] * t3[0] - e1[2] * t2[0] * t3[1]
			+ e1[2] * t2[1] * t3[0];

		if (d.get_sign() == 0) // coplanar
			return -1;
		t = (e0[0] * t1[1] * t2[2] - e0[0] * t1[1] * t3[2] - e0[0] * t1[2] * t2[1]
			+ e0[0] * t1[2] * t3[1] + e0[0] * t2[1] * t3[2] - e0[0] * t2[2] * t3[1]
			- e0[1] * t1[0] * t2[2] + e0[1] * t1[0] * t3[2] + e0[1] * t1[2] * t2[0]
			- e0[1] * t1[2] * t3[0] - e0[1] * t2[0] * t3[2] + e0[1] * t2[2] * t3[0]
			+ e0[2] * t1[0] * t2[1] - e0[2] * t1[0] * t3[1] - e0[2] * t1[1] * t2[0]
			+ e0[2] * t1[1] * t3[0] + e0[2] * t2[0] * t3[1] - e0[2] * t2[1] * t3[0]
			- t1[0] * t2[1] * t3[2] + t1[0] * t2[2] * t3[1] + t1[1] * t2[0] * t3[2]
			- t1[1] * t2[2] * t3[0] - t1[2] * t2[0] * t3[1]
			+ t1[2] * t2[1] * t3[0])
			/ d;

		const Rational u = (-e0[0] * e1[1] * t1[2] + e0[0] * e1[1] * t3[2]
			+ e0[0] * e1[2] * t1[1] - e0[0] * e1[2] * t3[1]
			- e0[0] * t1[1] * t3[2] + e0[0] * t1[2] * t3[1]
			+ e0[1] * e1[0] * t1[2] - e0[1] * e1[0] * t3[2]
			- e0[1] * e1[2] * t1[0] + e0[1] * e1[2] * t3[0]
			+ e0[1] * t1[0] * t3[2] - e0[1] * t1[2] * t3[0]
			- e0[2] * e1[0] * t1[1] + e0[2] * e1[0] * t3[1]
			+ e0[2] * e1[1] * t1[0] - e0[2] * e1[1] * t3[0]
			- e0[2] * t1[0] * t3[1] + e0[2] * t1[1] * t3[0]
			+ e1[0] * t1[1] * t3[2] - e1[0] * t1[2] * t3[1]
			- e1[1] * t1[0] * t3[2] + e1[1] * t1[2] * t3[0]
			+ e1[2] * t1[0] * t3[1] - e1[2] * t1[1] * t3[0])
			/ d;
		const Rational v = (e0[0] * e1[1] * t1[2] - e0[0] * e1[1] * t2[2]
			- e0[0] * e1[2] * t1[1] + e0[0] * e1[2] * t2[1]
			+ e0[0] * t1[1] * t2[2] - e0[0] * t1[2] * t2[1]
			- e0[1] * e1[0] * t1[2] + e0[1] * e1[0] * t2[2]
			+ e0[1] * e1[2] * t1[0] - e0[1] * e1[2] * t2[0]
			- e0[1] * t1[0] * t2[2] + e0[1] * t1[2] * t2[0]
			+ e0[2] * e1[0] * t1[1] - e0[2] * e1[0] * t2[1]
			- e0[2] * e1[1] * t1[0] + e0[2] * e1[1] * t2[0]
			+ e0[2] * t1[0] * t2[1] - e0[2] * t1[1] * t2[0]
			- e1[0] * t1[1] * t2[2] + e1[0] * t1[2] * t2[1]
			+ e1[1] * t1[0] * t2[2] - e1[1] * t1[2] * t2[0]
			- e1[2] * t1[0] * t2[1] + e1[2] * t1[1] * t2[0])
			/ d;

		// std::cout << t << std::endl;

		// std::cout << u << std::endl;

		// std::cout << v << std::endl;

		if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1) {
			if (u == 0 || u == 1 || v == 0 || v == 1 || u + v == 1)
				return 2; // on the border
			return 1;
		}

		return 0;

	}
	void get_tet_phi(bilinear& bl)
	{
		Vector3r p02 = (bl.v[0] + bl.v[2]) / 2;
		Rational phi02 = phi(p02, bl.v);
		if (phi02.get_sign() > 0) {
			bl.phi_f[0] = 1;
			bl.phi_f[1] = -1;
			return;
		}
		else {
			bl.phi_f[0] = -1;
			bl.phi_f[1] = 1;
			return;
		}
		std::cout << "!!can not happen, get tet phi" << std::endl;
	}
	bool segment_segment_inter(
		const Vector3r& s0,
		const Vector3r& e0,
		const Vector3r& s1,
		const Vector3r& e1,
		Vector3r& res,
		int axis)
	{
		const int i1 = (axis + 1) % 3;
		const int i2 = (axis + 2) % 3;

		const Rational dd = e0[i1] * e1[i2] - e0[i1] * s1[i2] - e0[i2] * e1[i1]
			+ e0[i2] * s1[i1] + e1[i1] * s0[i2] - e1[i2] * s0[i1] + s0[i1] * s1[i2]
			- s0[i2] * s1[i1];

		if (dd.get_sign() == 0) {
			return false;
		}

		const Rational t0 = (e1[i1] * s0[i2] - e1[i1] * s1[i2] - e1[i2] * s0[i1]
			+ e1[i2] * s1[i1] + s0[i1] * s1[i2] - s0[i2] * s1[i1])
			/ dd;
		const Rational t1 = (e0[i1] * s0[i2] - e0[i1] * s1[i2] - e0[i2] * s0[i1]
			+ e0[i2] * s1[i1] + s0[i1] * s1[i2] - s0[i2] * s1[i1])
			/ dd;

		// we exclude intersection on corners
		if (t0 <= 0 || t0 >= 1 || t1 <= 0 || t1 >= 1) {
			return false;
		}

		res = (1 - t0) * s0 + t0 * e0;
#ifndef NDEBUG
		const Vector3r p1 = (1 - t1) * s1 + t1 * e1;

		assert(res[0] == p1[0] && res[1] == p1[1] && res[2] == p1[2]);
#endif
		return true;
	}

	bool is_segment_degenerated(const Vector3r& s0, const Vector3r& s1)
	{
		if (s0[0] == s1[0] && s0[1] == s1[1] && s0[2] == s1[2])
			return true;
		return false;
	}

	// point and seg are colinear, now check if point is on the segment(can deal with segment degeneration)
	bool colinear_point_on_segment(
		const Vector3d& pt, const Vector3d& s0, const Vector3d& s1)
	{
		if (std::min(s0[0], s1[0]) <= pt[0] <= std::max(s0[0], s1[0]))
			if (std::min(s0[1], s1[1]) <= pt[1] <= std::max(s0[1], s1[1]))
				if (std::min(s0[2], s1[2]) <= pt[2] <= std::max(s0[2], s1[2]))
					return true;
		return false;
	}
	bool point_on_segment(const Vector3d& pt, const Vector3d& s0, const Vector3d& s1) {
		if (!is_triangle_degenerated(pt, s0, s1))
			return false;
		return colinear_point_on_segment(pt, s0, s1);
	}
	// check if two parallel segments (or any segment is degenerated as a point) have intersection
	bool parallel_segments_inter(
		const Vector3r& s0,
		const Vector3r& e0,
		const Vector3r& s1,
		const Vector3r& e1,
		const bool dege0,
		const bool dege1)
	{
		if (dege0)
			return colinear_point_on_segment(s0, s1, e1);
		if (dege1)
			return colinear_point_on_segment(s1, s0, e0);
		if (colinear_point_on_segment(s0, s1, e1))
			return true;
		if (colinear_point_on_segment(e0, s1, e1))
			return true;
		if (colinear_point_on_segment(s1, s0, e0))
			return true;
		if (colinear_point_on_segment(e1, s0, e0))
			return true;
		return false;
	}
	// segment segment intersection, can deal with degenerated segments
	bool segment_segment_intersection(
		const Vector3r& s0,
		const Vector3r& e0,
		const Vector3r& s1,
		const Vector3r& e1)
	{
		// if four points not coplanar, not intersected
		if (orient3d(s0, e0, s1, e1) != 0)
			return false;

		bool dege0 = is_segment_degenerated(s0, e0);
		bool dege1 = is_segment_degenerated(s1, e1);
		if (!dege0 && !dege1) {// two segments not degenerated
			if (same_point(
				cross(e0 - s0, e1 - s1),
				ORIGIN)) // if two segments not degenerated are parallel
				return parallel_segments_inter(s0, e0, s1, e1, dege0, dege1);

			Vector3r norm = tri_norm(s0, s1, e0);
			if (same_point(norm, ORIGIN))
				return colinear_point_on_segment(s1, s0, e0);

			Vector3r norm1 = tri_norm(s0, s1, e1);
			if (same_point(norm1, ORIGIN))
				return colinear_point_on_segment(s0, s1, e1);

			Vector3r norm2 = tri_norm(e1, s1, e0);
			if (same_point(norm2, ORIGIN))
				return colinear_point_on_segment(e0, e1, s1);
			Vector3r norm3 = tri_norm(e1, s0, e0);
			if (same_point(norm3, ORIGIN))
				return colinear_point_on_segment(e1, s0, e0);
			// if norm is same direction with norm1, norm2, norm3, then intersected
			if (norm.dot(norm1).get_sign() > 0 && norm.dot(norm2).get_sign() > 0 && norm.dot(norm3).get_sign() > 0)
				return true;
			return false;
		}
		if (dege0&&dege1) {
			if (same_point(s0, s1))
				return true;
			else
				return false;
		}
		if (dege0) {
			return colinear_point_on_segment(s0, s1, e1);
		}
		if (dege1) {
			return colinear_point_on_segment(s1, s0, e0);
		}
		std::cout << " impossible to go here" << std::endl;
		return false;
	}
	int orient_2d(const Vector2d&p, const Vector2d&q, const Vector2d&r) {
		return orient2d(p[0], p[1], q[0], q[1], r[0], r[1]);
	}
	int orient_3d(const Vector3d&p, const Vector3d&q, const Vector3d&r, const Vector3d& s) {
		return orient3d(p[0], p[1], p[2], q[0], q[1], q[2], r[0], r[1], r[2], s[0], s[1], s[2]);
	}
	bool point_on_segment_2d(const Vector2d&p, const Vector2d&s0, const Vector2d&s1, bool need_check_colinear) {
		if (need_check_colinear) {
			if (orient_2d(p, s0, s1) != 0)
				return false;
		}
		if (std::min(s0[0], s1[0]) <= p[0] <= std::max(s0[0], s1[0]))
			if (std::min(s0[1], s1[1]) <= p[1] <= std::max(s0[1], s1[1]))
				return true;
		return false;
	}
	bool segment_segment_intersection_2d(
		const Vector2d& s0,
		const Vector2d& e0,
		const Vector2d& s1,
		const Vector2d& e1) {
		int o0 = orient_2d(s0, s1, e1);
		int o1 = orient_2d(e0, s1, e1);
		if (o0 == o1 && o0 + o1 != 0) return false;
		int o2 = orient_2d(s1, s0, e0);
		int o3 = orient_2d(e1, s0, e0);
		if (o2 == o3 && o2 + o3 != 0) return false;
		if (o0 != o1 && o2 != o3) return true;
		// the rest of cases: one seg degenerate, or they are colinear
		if (point_on_segment_2d(s0, s1, e1, false) || point_on_segment_2d(e0, s1, e1, false)
			|| point_on_segment_2d(s1, s0, e0, false) || point_on_segment_2d(e1, s0, e0, false))
			return true;
		return false;
	}

	// 0 not intersected; 1 intersected; 2 pt on s0
	int point_on_ray(const Vector3r& s0,
		const Vector3r& dir0, const Vector3r& pt) {
		if (same_point(s0, pt))
			return 2;
		Vector3r dir1 = pt - s0;
		Vector3r norm = cross(dir0, dir1);
		if (!same_point(norm, ORIGIN))
			return 0;// if not coplanar, false
		if (dir0.dot(dir1) > 0)
			return 1;
		return 0;
	}
	// 0 not intersected, 1 intersected, 2 s0 on segment
	// can deal with degenerated cases
	int ray_segment_intersection(
		const Vector3r& s0,
		const Vector3r& dir0,
		const Vector3r& s1,
		const Vector3r& e1) {

		if (same_point(e1, s1))//degenerated case
			return point_on_ray(s0, dir0, s1);
		/////////////////////////////////////
		Vector3r norm = cross(s0 - s1, e1 - s0);
		if (same_point(norm, ORIGIN))
		{
			if (colinear_point_on_segment(s0, s1, e1))
				return 2;
			else return 0;
		}
		else {

			Vector3r norm1 = cross(s0 - s1, dir0);
			if (same_point(norm1, ORIGIN))
				return point_on_ray(s0, dir0, s1);

			Vector3r norm2 = cross(e1 - s0, dir0);
			if (same_point(norm2, ORIGIN)) {
				return point_on_ray(s0, dir0, e1);
			}

			if (norm.dot(norm1) > 0 && norm.dot(norm2) > 0) {
				return 1;
			}
			return 0;
		}


		/////////////////////////////////////
		//Vector3r dir1 = e1 - s1;
		//Vector3r norm = cross(dir1, dir0);
		//if (same_point(norm, ORIGIN))//parallel
		//{
		//	int inter1 = point_on_ray(s0, dir0, s1);
		//	int inter2 = point_on_ray(s0, dir0, e1);
		//	if (inter1 == 0 && inter2 == 0) return 0;
		//	if (inter1 == 2 && inter2 == 2) return 2;
		//	if (inter1 > 0 && inter2 == 0) return 2;
		//	if (inter2 > 0 && inter1 == 0) return 2;
		//	return 1;
		//}

	}
	int line_segment_intersection(
		const Vector3r& s0,
		const Vector3r& dir0,
		const Vector3r& s1,
		const Vector3r& e1) {

		if (same_point(e1, s1))//degenerated case
			return point_on_ray(s0, dir0, s1);
		/////////////////////////////////////
		Vector3r norm = cross(s0 - s1, e1 - s0);
		if (same_point(norm, ORIGIN))
		{
			if (colinear_point_on_segment(s0, s1, e1))
				return 2;
			else return 0;
		}
		else {

			Vector3r norm1 = cross(s0 - s1, dir0);
			if (same_point(norm1, ORIGIN))
				return point_on_ray(s0, dir0, s1);

			Vector3r norm2 = cross(e1 - s0, dir0);
			if (same_point(norm2, ORIGIN)) {
				return point_on_ray(s0, dir0, e1);
			}

			if (norm.dot(norm1) > 0 && norm.dot(norm2) > 0) {
				return 1;
			}
			return 0;
		}

	}


	void write(const Vector3d& v, std::ostream& out)
	{
		out.write(reinterpret_cast<const char*>(&v[0]), sizeof(v[0]));
		out.write(reinterpret_cast<const char*>(&v[1]), sizeof(v[1]));
		out.write(reinterpret_cast<const char*>(&v[2]), sizeof(v[2]));
	}

	Vector3d read(std::istream& in)
	{
		Vector3d res;
		double tmp;
		in.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
		res[0] = tmp;

		in.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
		res[1] = tmp;

		in.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));
		res[2] = tmp;

		return res;
	}
	// 2 on edge, 1 interior, 0 not intersect, 3 intersect OPEN edge t2-t3
	// norm follows right hand law
	int point_inter_triangle(
		const Vector3d&pt,
		const Vector3d& t1,
		const Vector3d& t2,
		const Vector3d& t3,
		const bool& dege, const bool halfopen) {
		if (dege) {// check 2 edges are enough
			if (point_on_segment(pt, t1, t2))
				return 2;
			if (point_on_segment(pt, t1, t3))
				return 2;
			return 0;
		}
		else {
			if (orient3d(pt, t1, t2, t3) != 0)
				return false;
			/*if (point_on_segment(pt, t1, t2))
				return 2;
			if (point_on_segment(pt, t1, t3))
				return 2;
			if (point_on_segment(pt, t2, t3))
				return 2;*///no need to do above
			Vector3d np = Vector3d::Random();
			while (orient_3d(np, t1, t2, t3) == 0) {// if coplanar, random
				np = Vector3d::Random();
			}
			int o1 = orient_3d(pt, np, t1, t2);
			int o2 = orient_3d(pt, np, t2, t3);// this edge 
			int o3 = orient_3d(pt, np, t3, t1);
			if (halfopen) {
				if (o2 == 0 && o1 == o3)
					return 3;// on open edge t2-t3
			}

			if (o1 == o2 && o1 == o3)
				return 1;
			if (o1 == 0 && o2 * o3 >= 0)
				return 2;
			if (o2 == 0 && o1 * o3 >= 0)
				return 2;
			if (o3 == 0 && o2* o1 >= 0)
				return 2;

			return 0;
		}
	}

	bool is_triangle_degenerated(const Vector3d& t1, const Vector3d& t2, const Vector3d&t3)
	{

		const auto to_2d = [](const Vector3d &p, int t)
		{
			return Vector2d(p[(t + 1) % 3], p[(t + 2) % 3]);
		};
		double r = ((t1 - t2).cross(t1 - t3)).norm();
		if (fabs(r) > 1e-8) return false;
		int ori;
		std::array<Vector2d, 3> p;
		for (int j = 0; j < 3; j++)
		{

			p[0] = to_2d(t1, j);
			p[1] = to_2d(t2, j);
			p[2] = to_2d(t3, j);

			ori = orient2d(p[0][0], p[0][1], p[1][0], p[1][1], p[2][0], p[2][1]);
			if (ori != 0)
			{
				return false;
			}
		}
		return true;
	}

	// we check if triangle intersect segment,
	// this function is used in cube edge--prism tri and cube edge--bilinear tri
	// if halfopen= true, can tell us if intersect the edge t2-t3
	// return 0, 1, 2, 3
	// this function is only used to check if seg intersect no degenerated bilinear
	//and if seg intersect opposite facets of tet. 
	int segment_triangle_intersection(
		const Vector3d& e0,
		const Vector3d& e1,
		const Vector3d& t1,
		const Vector3d& t2,
		const Vector3d& t3,
		const bool halfopen) {

		if (is_triangle_degenerated(t1, t2, t3))
			return 0;// since we already checked triangle edge - box segment

		int o1 = orient3d(e0, t1, t2, t3);
		int o2 = orient3d(e1, t1, t2, t3);
		if (o1 > 0 && o2 > 0)
			return 0;
		if (o1 < 0 && o2 < 0)
			return 0;

		if (o1 == 0 && o2 != 0)// e0 on the plane
			return point_inter_triangle(e0, t1, t2, t3, false, halfopen);
		if (o2 == 0 && o1 != 0)
			return point_inter_triangle(e1, t1, t2, t3, false, halfopen);
		if (o1 == 0 && o2 == 0)// two points are all on the plane. we already checked seg-two edges before
		{
			if (same_point(e0, e1))
				return point_inter_triangle(e0, t1, t2, t3, false, halfopen);
			else {
				int pinter0 = point_inter_triangle(e0, t1, t2, t3, false, halfopen);//2 is impossible
				int pinter1 = point_inter_triangle(e1, t1, t2, t3, false, halfopen);
				// two points all inside, inside; two outside, outside; others, intersect t2-t3
				if (pinter0 == 1 && pinter1 == 1) return 1; // interior
				if (pinter0 == 0 && pinter1 == 0) return 0;//  out
				if (halfopen)
					return 3;// intersect t2-t3
				else
					return 2;
			}
		}
		return is_line_cut_triangle(e0, e1, t1, t2, t3, halfopen);
	}
	// 0 no intersection, 1 intersect, 2 point on triangle(including two edges), 3 point on t2-t3 edge, -1 shoot on border
	int ray_triangle_intersection(
		const Vector3r& pt,
		const Vector3r& dir,
		const Vector3r& t1,
		const Vector3r& t2,
		const Vector3r& t3,
		const bool halfopen) {

		Vector3r norm = cross(t2 - t1, t3 - t2);
		if (same_point(norm, ORIGIN))// triangle degeneration
		{
			int inter1 = ray_segment_intersection(pt, dir, t1, t2);
			if (inter1 == 1) return -1;
			if (inter1 == 2) return 2;

			int inter2 = ray_segment_intersection(pt, dir, t1, t3);// check two segs is enough
			if (inter2 == 1) return -1;
			if (inter2 == 2) return 2;

			return 0;
		}

		int o1 = orient3d(pt, t1, t2, t3);
		if (o1 == 0) {//point is on the plane
			int inter = point_inter_triangle(pt, t1, t2, t3, false, norm, halfopen);
			if (inter == 1 || inter == 2) return 2;
			if (inter == 3) return 3;
			//if (inter == 0) 
			else {// pt on the plane but not intersect triangle.
				if (norm.dot(dir) == 0) {// if ray is on the plane
					int inter1 = ray_segment_intersection(pt, dir, t1, t2);
					if (inter1 == 1) return -1;
					if (inter1 == 2) return 2;// acutally, cannot be 2 because already checked by pt_inter_tri

					int inter2 = ray_segment_intersection(pt, dir, t1, t3);
					if (inter2 == 1) return -1;
					if (inter2 == 2) return 2;
					//actually since point do not intersect triangle, check two segs are enough
					//int inter3 = ray_segment_intersection(pt, dir, t2, t3);
					//// ray overlaps t2-t3, shoot another ray, ray intersect it, shoot another one 
					//if (inter3 == 1) return -1;
					//if (inter3 == 2) return 2;

					return 0;
				}


			}
			return 0;
		}
		Rational dt = norm.dot(dir);// >0, dir is same direction with norm, pointing to +1 orientation
		if (dt.get_sign() > 0 && o1 >= 0) return 0;
		if (dt.get_sign() < 0 && o1 <= 0) return 0;
		Vector3r np = sum(pt, dir);
		// if ray go across the plane, then get lpi and 3 orientations
		int inter = is_line_cut_triangle(pt, np, t1, t2, t3, halfopen);
		if (inter == 0)return 0;
		if (inter == 1)return 1;
		if (inter == 2)return -1;//shoot on edge
		if (inter == 3) return 3;

		return 0;
	}

	


} // namespace ccd