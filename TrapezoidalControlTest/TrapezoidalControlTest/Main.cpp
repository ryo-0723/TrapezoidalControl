# include <Siv3D.hpp> // OpenSiv3D v0.6.
#include"Trapezoidal.h"
#include"Plotter.h"

void Main()
{
	Vec2 point1{ 1.5,8.0 };
	Vec2 point2{ 4.0,-9.5 };
	Vec2 point3{ 9.0,5.0 };
	Vec2 point4{ 10.0,0.0 };

	Vec2 point{ 0,0 };
	double startS = 0;
	double endS = 0;
	int next_point = 0;
	double mil = 0;
	Vec2 targetSpeed{ 0,0 };
	Vec2 target{ 0,0 };
	uint64 loop = 0;
	Plotter plotter1, plotter2, plotter3, plotter4;
	Trapezoidal auto_set(4.0, 2.5, { 0,0 });
	while (System::Update())
	{
		ClearPrint();
		auto_set.update(point, startS, endS);

		if (Time::GetMillisec() > 3000) {
			if (auto_set.next_status()) {
				next_point++;
				auto_set.set_next_status();
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
		}
		//理想ではこの形で自動を回すこと


		loop = Time::GetMillisec();
		if (loop % 10) {
			targetSpeed.y = (auto_set.getTargetPos().y - target.y) / (((double)loop * 0.001) - mil);//m/s
			targetSpeed.x = (auto_set.getTargetPos().x - target.x) / (((double)loop * 0.001) - mil);//m/s
			target = auto_set.getTargetPos();

			mil = (double)loop * 0.001;

		}
			Print << auto_set.next_status();
			Print << next_point;
			Print << U"x : " << target.x;
			Print << U"y : " << target.y;
			//Print << U"xS : " << targetSpeed.x;
			//Print << U"yS : " << targetSpeed.y;
			plotter1
				.resize(Scene::Rect())
				.plot(targetSpeed.x)//x
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
				.plot(auto_set.getTargetPos().x)//y
				.maximum(10)
				.minimum(-10)
				.draw(Palette::Red)
				.drawGrid();
		
	}
}
