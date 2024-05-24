# include <Siv3D.hpp> // OpenSiv3D v0.6.
#include"Trapezoidal.h"
#include"Plotter.h"

void Main()
{
	double point1 = 1500;
	double point2 = 4000;
	double point3 = 9000;
	double point4 = -1000;
	double point5 = 5000;

	double point = 0;
	double startS = 0;
	double endS = 0;
	int next_point = 0;
	double mil = 0;
	double targetSpeed = 0;
	double target = 0;
	double targetpoint = 0;
	uint64 loop = 0;
	Plotter plotter1, plotter2, plotter3, plotter4;
	Trapezoidal auto_set(5000, 4500, -10000);
	bool oldClick = false;
	Window::Resize(1500, 1000);

	double oldSpeed = 0;
	bool count = true;
	double oldNowPos, oldTargetPos;
	bool oldStatus;
	double oldTime;

	while (System::Update())
	{
		ClearPrint();

		if (Time::GetMillisec() > 3000) {
			if (auto_set.status()) {
				next_point++;
			}
		}

		SimpleGUI::Slider(targetpoint, { 200,20 },400);

	/*	if (next_point > 4)
		{
			next_point = 0;
		}*/
		switch (next_point) {
		case 1:
			point = point1;
			endS = 1200;
			break;
		case 2:
			point = point2;
			endS = 5000;
			break;
		case 3:
			point = point3;
			endS = 0;
			break;
		case 4:
			point = point4;
			endS = 1000;
			break;
		case 5:
			point = point5;
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
		auto_set.update(targetpoint * 20000 - 10000);
		//auto_set.update(point, endS );

		if (count) {
			if (abs(oldSpeed - auto_set.getNowSpeed()) > 50) {
				count = false;
			}
			else {
				oldSpeed = auto_set.getNowSpeed();
				oldTargetPos = auto_set.getTargetPos();
				oldNowPos = auto_set.getNowPos();
				oldStatus = auto_set.status();
				oldTime = auto_set.getTime();
			}
		}
		//	Print << auto_set.status();
		//	Print << next_point;
		   // Print << U"Input    :" << targetpoint * 20000 - 10000;
			Print << U"NowPos   : " << auto_set.getNowPos();
			Print << U"TargetPos: " << auto_set.getTargetPos();
			Print << U"Speed    : " << auto_set.getNowSpeed();
			Print << U"Status   : " << auto_set.status();

			Print << U"Stops    : " << oldSpeed;
			Print << U"StopsT   : " << oldTargetPos;
			Print << U"StopsN   : " << oldNowPos;
			Print << U"StopsS   : " << oldStatus;
			Print << U"StopsTime: " << oldTime;

			//Print << U"xS : " << targetSpeed.x;
			//Print << U"yS : " << targetSpeed.y;

			plotter2
				.resize(Scene::Rect())
				.plot(/*next_point*/0)//y
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Yellow)
				.drawGrid();
			//plotter3
			//	.resize(Scene::Rect())
			//	.plot(0/*auto_set.getTargetPos().x*/)//y
			//	.maximum(10)
			//	.minimum(-10)
			//	.draw(Palette::Blue)
			//	.drawGrid();
			plotter4
				.resize(Scene::Rect())
				.plot(auto_set.getTargetPos())//y
				.maximum(10000)
				.minimum(-10000)
				.draw(Palette::Red)
				.drawGrid();
			plotter1
				.resize(Scene::Rect())
				.plot(auto_set.getNowSpeed())//x
				.maximum(5000)
				.minimum(-5000)
				.draw(Palette::Green)
				.drawGrid();
		
	}
}
