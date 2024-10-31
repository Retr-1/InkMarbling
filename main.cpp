
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

namespace retr {
	struct Line2d {
		float a;
		float b;
		float c;

		Line2d(const olc::vi2d& p0, const olc::vi2d& p1) {
			auto u = p1 - p0;
			a = u.y;
			b = -u.x;
			c = -(a * p0.x + b * p0.y);
		}

		olc::vi2d intersection(const Line2d& other) {
			// a1*x + b1*y + c1 = 0
			// a2*x + b2*y + c2 = 0
			// x = -(b1*y + c1)/a1
			// -a2*b1*y/a1 -a2*c1/a1 + b2*y + c2 = 0
			// y = (a2*c1/a1 - c2)/(-a2*b1/a1 + b2)
			float y = (other.a * c / a - other.c) / (-other.a * b / a + other.b);
			float x = -(b * y + c) / a;
			return olc::vi2d(x, y);
		}

	};
	class Polygon {
	private:
		struct tri {
			olc::vi2d p1;
			olc::vi2d p2;
			olc::vi2d p3;

			tri(const olc::vi2d& p1, const olc::vi2d& p2, const olc::vi2d& p3) : p1(p1), p2(p2), p3(p3) {}
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

		void draw(olc::PixelGameEngine& canvas) {
			for (const auto& t : triangles) {
				canvas.FillTriangle(t.p1, t.p2, t.p3, color);
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

					

					bool inside = false;
					for (int j = 0; j < remPoints.size(); j++) {
						auto& k0 = remPoints[j];
						auto& k1 = remPoints[(j + 1) % remPoints.size()];

						if (std::max(k0.y, k1.y) < mid.y || std::min(k0.y, k1.y) > mid.y)
							continue;

						if (k0.x == k1.x) {
							if (mid.x <= k0.x) {
								inside = !inside;
							}
						}
						else {
							float a = (k1.y - k0.y) / (float)(k1.x - k0.x);
							float b = k0.y - k0.x * a;
							// y = ax + b
							// x = (y-b)/a
							int intersectX = (mid.y - b) / a;
							if (mid.x <= intersectX) {
								inside = !inside;
							}
						}
					}

					if (inside) {
						tri triangle(p0, p1, p2);
						triangles.push_back(triangle);
						remPoints.erase(remPoints.begin() + i);
						goto ELSE_END;
					}
				}

				std::cout << "DID not find\n";
				break;
				ELSE_END:;
			}

			//tri triangle(remPoints[0], remPoints[1], remPoints[2]);
			triangles.push_back(tri(remPoints[0], remPoints[1], remPoints[2]));
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

		Clear(olc::BLACK);
		pol.draw(*this);
		for (auto& p : pol.points) {
			DrawCircle(p, 5, olc::RED);
		}
		for (int i = 0; i < pol.points.size(); i++) {
			DrawLine(pol.points[i], pol.points[(i + 1) % pol.points.size()], olc::GREEN);
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