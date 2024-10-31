
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class Polygon {
private:
	struct tri {
		int p0;
		int p1;
		int p2;
	};
	std::vector<tri> triangles;
public:
	std::vector<olc::vf2d> points;
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
	
	}

	void calculateTriangles() {
		
		for (int i = 0; i < points.size(); i++) {
			auto& p0 = points[(i - 1 + points.size()) % points.size()];
			auto& p1 = points[i];
			auto& p2 = points[(i + 1) % points.size()];

			olc::vf2d mid = (p0 + p2) / 2;

			bool inside = false;
			for (int j = 0; j < points.size(); j++) {
				auto& k0 = points[j];
				auto& k1 = points[(j + 1) % points.size()];

			}
		}
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