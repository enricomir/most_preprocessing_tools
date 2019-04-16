#include "simplifier.hpp"

using namespace Simplifier;

//Helping functions for tetrahedral removal:
SimplePoint get_point(SimplePoint p, Polygon pol) {
	//TODO: change header to accept polygons. Get cost as the max of the other
	//polygon that has this as closest.
	SimplePoint ret = pol.points[0];

	for (auto pol_point: pol.points) {
		if (SimplePoint::norm(p, ret) > SimplePoint::norm(p, pol_point)) {
			ret = pol_point;
		}
	}

	return ret;
}

double get_vol(size_t i_t, Polygon pol, Polygon next_pol) {
	SimplePoint pt(0, 0);  //Current point
	SimplePoint ptp(0, 0); //Previous point on current polygon
	SimplePoint ptn(0, 0); //Next point on current polygon
	SimplePoint ptt(0, 0); //Point on other polygon.

	pt = pol.points[i_t];
	if (i_t == 0) {
		ptp = pol.points[pol.points.size() - 1];
	} else {
		ptp = pol.points[i_t-1];
	}

	if (i_t == (pol.points.size()-1)) {
		ptn = pol.points[0];
	} else {
		ptn = pol.points[i_t+1];
	}

	ptt = get_point(pol.points[i_t], next_pol);

	//Volume reference: 
	//https://en.wikipedia.org/wiki/Tetrahedron#Heron-type_formula_for_the_volume_of_a_tetrahedron
	double u, v, w, U, V, W;
	U = SimplePoint::norm(pt, ptp);
	V = SimplePoint::norm(pt, ptn);
	W = SimplePoint::norm(pt, ptt, 10);

	u = SimplePoint::norm(ptn, ptt, 10);
	v = SimplePoint::norm(ptp, ptt, 10);
	w = SimplePoint::norm(ptp, ptn);

	double x, y, z, X, Y, Z;
	X = (w - U + v)*(U + v + w);
	x = (U - v + w)*(v - w + U);
	Y = (u - V + w)*(V + w + u);
	y = (V - w + u)*(w - u + V);
	Z = (v - W + u)*(W + u + v);
	z = (W - u + v)*(u - v + W);

	if (X*x*Y*y*Z*z == 0) return 0;

	double a, b, c, d;
	a = std::sqrt(x*Y*Z);
	b = std::sqrt(y*Z*X);
	c = std::sqrt(z*X*Y);
	d = std::sqrt(x*y*z);

	//The 4 parts of numerator on volume
	double p1, p2, p3, p4;
	p1 = (-a+b+c+d);
	p2 = (+a-b+c+d);
	p3 = (+a+b-c+d);
	p4 = (+a+b+c-d);

	return std::sqrt(p1*p2*p3*p4)/(192*u*v*w);
}

void Simplifier::tetrahedral_until_n_points (std::vector<Polygon> polys, float red_percentage) {
	//Not enough polygons
	if (polys.size() < 2) return;
	std::vector<size_t> red_number;

	//Decides how many points to remove from each polygon
	for (size_t i = 0; i < polys.size(); ++i) {
		red_number.push_back(polys[i].points.size()*red_percentage);
	}

	bool cont=true;
	while (cont) {
		//Removes 1 point from any needed polygon
		for (size_t i_pol = 0; i_pol < polys.size(); ++i_pol) {
			if (red_number[i_pol] == 0) continue; //If removed all needed points, move on

			//Minimum cost of points, and point associated with this cost
			double min_cost=std::numeric_limits<double>::max();
			size_t to_remove=0;

			//Calculates the cost for all points
			for (size_t i_pt = 0; i_pt < polys[i_pol].points.size(); ++i_pt) {
				//Cost to remove from previous shape
				double vol_t_previous = 0.0;
				if (i_pol > 0) {
					vol_t_previous = get_vol(i_pt, polys[i_pol], polys[i_pol-1]);
				}

				//Cost to remove from next shape
				double vol_t_next = 0.0;
				if (i_pol < (polys.size()-1)) {
					vol_t_next = get_vol(i_pt, polys[i_pol], polys[i_pol+1]);
				}

				//Checks if current point is the cheapest to remove
				double cost = (vol_t_previous > vol_t_next? vol_t_previous : vol_t_next);
				if (cost < min_cost) {
					to_remove = i_pt;
					min_cost = cost;
				}
			}
			//Removes the cheapest point
			polys[i_pol].points.erase(polys[i_pol].points.begin() + to_remove);
			red_number[i_pol]--;
		}

		//Loop condition - if there is still any points to remove
		cont = false;
		for (size_t i = 0; i < polys.size(); ++i) {
			if (red_number[i] != 0) {
				cont = true;
				break;
			}
		}
	}
}

//Haussforf-like metric
double get_cost(size_t i, Polygon pol, Polygon next_pol) {
	double max_cost = 0.0;
	for (SimplePoint p: next_pol.points) {
		double cost = SimplePoint::norm(pol.points[i], p);
		double found_closer = false; //Checks if i is closest point
		for (SimplePoint p1: pol.points) {
			if (cost > SimplePoint::norm(p, p1)) {
				found_closer = true; 
				break;
			}
		}
		if (!found_closer) { //If i is the closest point, we check max_cost
			if (cost > max_cost) {
				max_cost = cost;
			}
		}
	}

	return max_cost;
}

//Get area of triangle centered on i
double get_triangle(size_t i, Polygon pol) {
	SimplePoint pt(0, 0);  //Current point
	SimplePoint ptp(0, 0); //Previous point
	SimplePoint ptn(0, 0); //Next point

	pt = pol.points[i];

	if (i == 0) {
		ptp = pol.points[pol.points.size() - 1];
	} else {
		ptp = pol.points[i-1];
	}

	if (i == (pol.points.size()-1)) {
		ptn = pol.points[0];
	} else {
		ptn = pol.points[i+1];
	}

	double a = SimplePoint::norm(pt, ptp);
	double b = SimplePoint::norm(pt, ptn);
	double c = SimplePoint::norm(ptp, ptn);

	double p = (a+b+c)/2;

	return std::sqrt(p*(p-a)*(p-b)*(p-c));
}

void Simplifier::visvalingam_with_time(std::vector<Polygon> polys, float red_percentage, float time_value) {
	//Not enough polygons
	if (polys.size() < 2) return;
	std::vector<size_t> red_number;

	//Decides how many points to remove from each polygon
	for (size_t i = 0; i < polys.size(); ++i) {
		red_number.push_back(polys[i].points.size()*red_percentage);
	}

	bool cont=true;
	while (cont) {
		//Removes 1 point from any needed polygon
		for (size_t i_pol = 0; i_pol < polys.size(); ++i_pol) {
			if (red_number[i_pol] == 0) continue; //If removed all needed points, move on

			//Minimum cost of points, and point associated with this cost
			double min_cost=std::numeric_limits<double>::max();
			size_t to_remove=0;

			//Calculates the cost for all points
			for (size_t i_pt = 0; i_pt < polys[i_pol].points.size(); ++i_pt) {
				//Cost to remove from previous shape
				double cost_previous = 0.0;
				if (i_pol > 0) {
					cost_previous = get_cost(i_pt, polys[i_pol], polys[i_pol-1]);
				}

				//Cost to remove from next shape
				double cost_next = 0.0;
				if (i_pol < (polys.size()-1)) {
					cost_next = get_cost(i_pt, polys[i_pol], polys[i_pol+1]);
				}

				//Checks if current point is the cheapest to remove
				double cost_time = (cost_previous > cost_next? cost_previous : cost_next);
				double cost_triangle = get_triangle(i_pt, polys[i_pol]);

				double cost = (cost_time*time_value > cost_triangle? cost_time*time_value : cost_triangle);

				if (cost < min_cost) {
					to_remove = i_pt;
					min_cost = cost;
				}
			}
			//Removes the cheapest point
			polys[i_pol].points.erase(polys[i_pol].points.begin() + to_remove);
			red_number[i_pol]--;
		}

		//Loop condition - if there is still any points to remove
		cont = false;
		for (size_t i = 0; i < polys.size(); ++i) {
			if (red_number[i] != 0) {
				cont = true;
				break;
			}
		}
	}
}


