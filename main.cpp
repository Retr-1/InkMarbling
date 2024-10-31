
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


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

	static Polygon createCircle(float r, float x, float y, int nPoints=10) {
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

		while (remPoints.size() > 3) {
			for (int i = 0; i < remPoints.size(); i++) {
				auto& p0 = remPoints[(i - 1 + remPoints.size()) % remPoints.size()];
				auto& p1 = remPoints[i];
				auto& p2 = remPoints[(i + 1) % remPoints.size()];

				olc::vi2d mid = (p0 + p2) / 2;

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
						float b = k0.y - k1.x * a;
						// y = ax + b
						// x = (y-b)/a
						int intersectX = (mid.y - b) / a;
						if (mid.x <= intersectX) {
							inside = !inside;
						}
					}
				}

				if (inside) {
					tri triangle(p0,p1,p2);
					triangles.push_back(triangle);
					remPoints.erase(remPoints.begin() + i);
					break;
				}
			}
		}

		//tri triangle(remPoints[0], remPoints[1], remPoints[2]);
		triangles.push_back(tri(remPoints[0], remPoints[1], remPoints[2]));
	}
};

void drop() {

}

void tineLine() {

}

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
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