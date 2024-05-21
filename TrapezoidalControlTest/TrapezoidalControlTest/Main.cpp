# include <Siv3D.hpp> // OpenSiv3D v0.6.
#include"Trapezoidal.h"
#include"Plotter.h"

void Main()
{
	double point1 = 1.5;
	double point2 = 4.0;
	double point3 = 9.0;
	double point4 = -10;
	double point5 = 5;

	double point = 0;
	double startS = 0;
	double endS = 0;
	int next_point = 0;
	double mil = 0;
	double targetSpeed = 0;
	double target = 0;
	uint64 loop = 0;
	Plotter plotter1, plotter2, plotter3, plotter4;
	Trapezoidal auto_set(5.0, 1.5, 0);
	bool oldClick = false;
	while (System::Update())
	{
	//	ClearPrint();

		if (Time::GetMillisec() > 3000) {
			if (auto_set.status()) {
				next_point++;
			}
		}

		switch (next_point) {
		case 1:
			point = point1;
			startS = 0;
			endS = 1.2;
			break;
		case 2:
			point = point2;
			startS = 1.2;
			endS = 0.5;
			break;
		case 3:
			point = point3;
			startS = 0.5;
			endS = 0;
			break;
		case 4:
			point = point4;
			startS = 0;
			endS = 0;
			break;
		case 5:
			point = point5;
			startS = 0;
			endS = 0;
			break;
		}
		//理想ではこの形で自動を回すこと

		if (KeySpace.pressed()) {
			if (oldClick == false) {
			next_point++;
			}
			oldClick = true;
		}
		else {
			oldClick = false;
		}

		auto_set.update(point, startS, endS);

		//	Print << auto_set.status();
		//	Print << next_point;
			//Print << U"x : " << target.x;
			//Print << U"y : " << target.y;
			//Print << U"xS : " << targetSpeed.x;
			//Print << U"yS : " << targetSpeed.y;
			plotter1
				.resize(Scene::Rect())
				.plot(auto_set.getNowSpeed())//x
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Green)
				.drawGrid();
			plotter2
				.resize(Scene::Rect())
				.plot(next_point)//y
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Yellow)
				.drawGrid();
			plotter3
				.resize(Scene::Rect())
				.plot(0/*auto_set.getTargetPos().x*/)//y
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Blue)
				.drawGrid();
			plotter4
				.resize(Scene::Rect())
				.plot(auto_set.getTargetPos())//y
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Red)
				.drawGrid();
		
	}
}
