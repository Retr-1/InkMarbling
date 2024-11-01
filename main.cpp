
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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
	private:
		struct tri {
			olc::vi2d p1;
			olc::vi2d p2;
			olc::vi2d p3;
			float color;

			tri(const olc::vi2d& p1, const olc::vi2d& p2, const olc::vi2d& p3, float c=0) : p1(p1), p2(p2), p3(p3),color(c) {}
			tri() {}
		};
		std::vector<tri> triangles;

	public:
		std::vector<olc::vi2d> points;
		olc::Pixel color;

		static Polygon createCircle(float r, float x, float y, int nPoints = 10) {
			float da = 3.1415 * 2 / 10;
			Polygon pol;
			for (int i = 0; i < nPoints; i++) {
				auto p = olc::vf2d(cosf(da * i) * r + x, sinf(da * i) * r + y);
				pol.points.push_back(p);
			}
			return pol;
		}

		bool isInside(olc::vi2d point, olc::PixelGameEngine& canvas) {
			Line2d line1(point, point + olc::vi2d(10, 0));

			bool oddIntersects = false;
			for (int j = 0; j < points.size(); j++) {
				auto& k0 = points[j];
				auto& k1 = points[(j + 1) % points.size()];

				Line2d line2(k0, k1);

				olc::vi2d intersection = line1.intersection(line2);
				canvas.DrawCircle(intersection, 3, olc::CYAN);
				if (intersection.x > std::min(k0.x, k1.x) && intersection.x < std::max(k0.x, k1.x) && intersection.x > point.x) {
					oddIntersects = !oddIntersects;
				}
			}

			return oddIntersects;
		}

		void draw(olc::PixelGameEngine& canvas) {
			for (const auto& t : triangles) {
				canvas.FillTriangle(t.p1, t.p2, t.p3, olc::Pixel(t.color, t.color, t.color));
			}
		}

		void calculateTriangles() {
			std::vector<olc::vi2d> remPoints = points;
			triangles.clear();

			if (remPoints.size() < 3) {
				return;
			}

			while (remPoints.size() > 3) {
				for (int i = 0; i < remPoints.size(); i++) {
					auto& p0 = remPoints[(i - 1 + remPoints.size()) % remPoints.size()];
					auto& p1 = remPoints[i];
					auto& p2 = remPoints[(i + 1) % remPoints.size()];

					Line2d line1(p0, p2);
					olc::vi2d mid = p0 + (p2 - p0) / 2;
					Line2d midline(mid, mid + olc::vi2d(10, 0));

					bool oddIntersects = false;
					for (int j = 0; j < remPoints.size(); j++) {
						auto& k0 = remPoints[j];
						auto& k1 = remPoints[(j + 1) % remPoints.size()];
						
						Line2d line2(k0, k1);
						olc::vi2d intersection = line1.intersection(line2);
						if (intersection.x > std::min(p0.x, p2.x) && intersection.x < std::max(p0.x, p2.x) && intersection.x > std::min(k0.x, k1.x) && intersection.x < std::max(k0.x,k1.x)) {
							goto ELSE_END_J;
						}

						intersection = midline.intersection(line2);
						if (intersection.x > std::min(k0.x, k1.x) && intersection.x < std::max(k0.x, k1.x) && intersection.x > mid.x) {
							oddIntersects = !oddIntersects;
						}
					}
					
					if (oddIntersects) {
						triangles.push_back(tri(p0, p1, p2, remPoints.size() / (float)points.size() * 255));
						remPoints.erase(remPoints.begin() + i);
						goto ELSE_END_I;
					}

					ELSE_END_J:;

				}
				std::cout << "DIDNT FIND\n";
				break;

				ELSE_END_I:;
			}
	
			//tri triangle(remPoints[0], remPoints[1], remPoints[2]);
			triangles.push_back(tri(remPoints[0], remPoints[1], remPoints[2],50));
		}
	};
}

void drop() {

}

void tineLine() {

}

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
	retr::Polygon pol;

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
		pol.color = olc::WHITE;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		if (GetMouse(0).bPressed) {
			pol.points.push_back(GetMousePos());
			pol.calculateTriangles();
		}
		if (GetKey(olc::R).bPressed) {
			pol.points.clear();
		}

		Clear(olc::BLACK);
		pol.draw(*this);
		for (auto& p : pol.points) {
			DrawCircle(p, 5, olc::RED);
		}
		for (int i = 0; i < pol.points.size(); i++) {
			DrawLine(pol.points[i], pol.points[(i + 1) % pol.points.size()], olc::GREEN);
		}

		if (pol.isInside(GetMousePos(), *this)) {
			FillCircle(GetMousePos(), 5, olc::BLUE);
		}

		return true;

	}
};

int main()
{
	Window win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();
	return 0;
}