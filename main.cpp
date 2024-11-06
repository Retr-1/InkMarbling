
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
using namespace std;

namespace retr {
	struct Line2d {
		float a;
		float b;
		bool vertical = false;

		Line2d(const olc::vi2d& p0, const olc::vi2d& p1) {
			if (p0.x == p1.x) {
				vertical = true;
				a = p0.y;
				b = p0.x;
			}
			else {
				a = (p1.y - p0.y) / (float)(p1.x - p0.x);
				b = p0.y - p0.x * a;
			}
		}

		olc::vi2d intersection(const Line2d& other) {
			// y = a1x + b1
			// y = a2x + b2
			// a1x + b1 = a2x + b2
			// x = (b2-b1) / (a1-a2)
			float x = 0, y = 0;
			if (vertical || other.vertical) {
				if (!vertical) {
					x = other.b;
					y = a * x + b;
				}
				else if (!other.vertical) {
					x = b;
					y = other.a * x + other.b;
				}
			}
			else {
				if (a != other.a) {
					x = (other.b - b) / (a - other.a);
					y = a * x + b;
				}
			}
			return olc::vi2d(x, y);
		}

	};
	class Polygon {
	public:
		std::vector<olc::vf2d> points;
		olc::Pixel color;

		static Polygon createCircle(float x, float y, float r, int nPoints = 10, olc::Pixel color=olc::WHITE) {
			float da = 3.1415 * 2 / nPoints;
			Polygon pol;
			pol.color = color;
			for (int i = 0; i < nPoints; i++) {
				auto p = olc::vf2d(cosf(da * i) * r + x, sinf(da * i) * r + y);
				pol.points.push_back(p);
			}
			return pol;
		}

		bool isInside(const olc::vi2d& point, olc::PixelGameEngine& canvas) {
			bool inside = false;
			for (int j = 0; j < points.size(); j++) {
				auto& k0 = points[j];
				auto& k1 = points[(j + 1) % points.size()];

				if (point.y > std::min(k0.y, k1.y) && point.y <= std::max(k0.y, k1.y) && point.x <= std::max(k0.x, k1.x)) {
					double x_intersection = (point.y - k0.y) * (k1.x - k0.x) / (double)(k1.y - k0.y) + k0.x;
					canvas.DrawCircle((int32_t)x_intersection, point.y, 5, olc::GREEN);
					if (k0.x == k1.x || point.x <= x_intersection) {
						inside = !inside;
					}
				}
			}

			return inside;
		}

		void draw(olc::PixelGameEngine& canvas) {
			if (points.size() == 0) return;

			std::vector<std::tuple<int, int>> events;
			int ymin = points[0].y;
			int ymax = points[0].y;

			for (int i = 0; i < points.size(); i++) {
				/*int j = i;
				int k = (i + 1) % points.size();
				if (points[j].y < points[k].y) {
					std::swap(j, k);
				}*/
				int j = (i + 1) % points.size();
				events.push_back(std::make_tuple(points[i].y, i));
				events.push_back(std::make_tuple(points[j].y, i));
				ymin = std::min(points[i].y, (float)ymin);
				ymax = std::max(points[i].y, (float)ymax);
			}

			std::sort(events.begin(), events.end());

			struct SweepObject {
				double x;
				double dx;
				int id;
			};

			std::vector<SweepObject> sweepline;

			int e = 0;

			std::vector<bool> used(points.size(), false);

			for (int y = ymin; y <= ymax; y++) {
				while (e < events.size() && std::get<0>(events[e]) <= y) {
					int id = get<1>(events[e]);
					
					int i = id;
					int j = (i + 1) % points.size();
					if (points[i].y > points[j].y) {
						swap(i, j);
					}

					if (!used[id]) {
						// adding
						used[id] = true;
						SweepObject swee;
						swee.x = points[i].x;
						swee.dx = (points[j].x - points[i].x) / (double)(points[j].y - points[i].y);
						swee.id = id;
						sweepline.push_back(swee);
					}
					else {
						// removing
						for (int k = 0; k < sweepline.size(); k++) {
							if (sweepline[k].id == id) {
								sweepline.erase(sweepline.begin() + k);
								break;
							}
						}
					}
					e++;
				}

				sort(sweepline.begin(), sweepline.end(), [](const SweepObject& o1, const SweepObject& o2)->bool {
					return o1.x < o2.x;
				});

				for (int i = 0; i+1 < sweepline.size(); i += 2) {
					canvas.DrawLine(sweepline[i].x, y, sweepline[i + 1].x, y, color);
				}

				for (int i = 0; i < sweepline.size(); i++) {
					sweepline[i].x += sweepline[i].dx;
				}
			}

		}
	};
}

void drop(vector<retr::Polygon>& polys, float x, float y, float r, olc::Pixel color) {
	retr::Polygon pol = retr::Polygon::createCircle(x, y, r, 50, color);
	olc::vf2d C(x, y);
	for (int i = 0; i < polys.size(); i++) {
		for (int j = 0; j < polys[i].points.size(); j++) {
			auto& P = polys[i].points[j];
			olc::vf2d u = P - C;
			double m = u.x * u.x + u.y * u.y;
			olc::vf2d P2 = C + u * sqrt(1 + (r * r) / m);
			P.x = P2.x;
			P.y = P2.y;
		}
	}
	polys.push_back(pol);
}

void tineLine(vector<retr::Polygon>& polys, olc::vf2d B, olc::vf2d M, float z, float c) {
	M = M.norm();
	olc::vf2d N(-M.y, M.x);
	float u = 1 / pow(2, 1 / c);
	for (int i = 0; i < polys.size(); i++) {
		for (int j = 0; j < polys[i].points.size(); j++) {
			auto& P = polys[i].points[j];
			float d = abs((P - B).dot(N));
			P = P + z * pow(u, d) * M;
		}
	}
}

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
	vector<retr::Polygon> polys;
	bool refresh = false;

public:
	Window()
	{
		// Name your application
		sAppName = "Window";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		if (GetMouse(0).bPressed) {
			refresh = true;
			int a = rand() / (float)RAND_MAX * 255;
			drop(polys, GetMouseX(), GetMouseY(), 40, olc::Pixel(a, a, a));
		}
		if (GetMouse(1).bPressed) {
			refresh = true;
			tineLine(polys, GetMousePos(), { 0, 1.0f }, 80, 16);
		}
		if (GetKey(olc::R).bPressed) {
			polys.clear();
		}
		

		if (refresh) {
			refresh = false;
			Clear(olc::BLACK);
			for (auto& p : polys) {
				p.draw(*this);
			}
		}

		return true;

	}
};

int main()
{
	srand(time(0));
	Window win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();
	
	return 0;
}