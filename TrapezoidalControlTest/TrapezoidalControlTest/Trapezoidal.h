#pragma once
/*
* 現状の課題点は次の順路に移行したときに、がたつきが見られるところ
* 長さと旋回角の台形制御をして、ベジェ曲線の経路クラスと合わせて使用することを目的とした。
*/
# include <Siv3D.hpp> // OpenSiv3D v0.6.


class MyTimer {
	unsigned long times;
	bool stopFlag;
	bool resetFlag;
	bool timerState;
	unsigned long millis() const {
		return (unsigned long)Time::GetMillisec();
	}
public:
	MyTimer()
		: times()
		, stopFlag(false)
		, resetFlag(false)
		, timerState(false)
	{}
	/// @brief タイマー時間を返す
	unsigned long getTime()const {
		if (resetFlag)return 0;
		if (stopFlag)return times;
		return millis() - times;// ms
	}
	/// @brief タイマーの始動（何度呼び出してもかまわない）
	void start() {
		if (not timerState)
			times = millis();
		timerState = true;
		stopFlag = false;
		resetFlag = false;
	}
	/// @brief タイマー停止
	void stop() {
		timerState = false;
		if (not stopFlag)
			times = millis() - times;
		stopFlag = true;
	}
	/// @brief タイマーのリセット
	void reset() {
		resetFlag = true;
	}
};

class Trapezoidal {
private:
	double upTime, maxPowerTime, downTime;
	double acc , maxSpeed, limitSpeed;
	bool calState, nextState,dir ;
	double nowPos, nowSpeed;
	double targetPos,oldTarget;
	double startSpeed, endSpeed;
	MyTimer myTimer;


	unsigned long millis() const {
		return (unsigned long)Time::GetMillisec();
	}
	double constrain(const double x, const double a, const double b) {
		if (x < a)
			return a;
		else if (b < x)
			return b;
		else
			return x;
	}
	double sq(double r) {
		return r * r;
	}

public:
	/// @brief 一次元台形制御のコンストラクタ
	/// @param acc 加速度
	/// @param maxSpeed　最大速度 
	/// @param nowPos スタート座標
	Trapezoidal(double acc, double maxSpeed, double nowPos)
		: acc(acc)
		, maxSpeed(maxSpeed)
		, nowPos(nowPos)
		, limitSpeed()
		, upTime()
		, maxPowerTime()
		, downTime()
		, calState(false)
		, nextState(true)
		, dir()
		, nowSpeed()
		, targetPos()
		, oldTarget()
		, startSpeed()
		,endSpeed()
		, myTimer()
	{
	}

	double operator()(double target, double starSpeed = 0, double enSpeed = 0) {
		return 0;
	}

	void update(double target, double startS = 0, double endS = 0) {
		// X=Vot+(1/2)*at^2;
		// V=a*t +Vo;
		// V^2-Vo^2 =2as;
		//引数のstartSは必要ないかも
		if (oldTarget != target) {//目標座標が変わったとき
			nowPos = targetPos;//途中までの経路情報の保持
			if (nowSpeed != 0)
				startSpeed = nowSpeed;
			else
				startSpeed = startS * (target - oldTarget > 0 ? 1 : -1);//これにも＋ーの判定いる

			myTimer.stop();
			myTimer.reset();
			calState = false;
			nextState = false;
			oldTarget = target;
		}

		if (!calState) {
			//速度が進行方向と逆かどうかで減速の動作も必要
			const double distance = target - nowPos;//距離を求める
			dir = (distance > 0.0 ? 1 : 0);//増減の方向を決める
			endSpeed = endS;
			if (!dir) {
				//startSpeed = abs(startSpeed)* - 1;
				endSpeed = abs(endSpeed) * -1;
				maxSpeed = abs(maxSpeed) * -1;
			}

			upTime = (maxSpeed - startSpeed) / acc; //最高速度までの加速にかかる時間 /s
			downTime = (maxSpeed - endSpeed) / acc; //減速にかかる時間 /s
			const double L1 = startSpeed * upTime + acc * sq(upTime) * 0.5; //加速時における移動距離 /m
			const double L3 = endSpeed * downTime + acc * sq(downTime) * 0.5; //減速時における移動距離/m
			if (abs(L1 + L3) > abs(distance)) {//台形ができなくなり、三角形になるときの制御
				limitSpeed = 2.00 * acc * distance * 0.5 + sq(startSpeed);
				limitSpeed = sqrt(limitSpeed);
				upTime = (limitSpeed - startSpeed) / acc;
				downTime = (limitSpeed - endSpeed) / acc;
				maxPowerTime = 0.0;
			}
			else {//台形が作れる場合の制御 
				maxPowerTime = (abs(distance) - L1 - L3) / maxSpeed; //最高速度での移動時間 /s
				limitSpeed = maxSpeed;
				Print << U"aa";
			}
			calState = true;
		}
		if (calState)
			myTimer.start();//タイマーを開始する←何度呼び出しても一度だけ実行される

		upTime = abs(upTime); //絶対値をとる（時間のマイナスを消すため）
		downTime = abs(downTime);
		double t = myTimer.getTime() * 0.001;  //s
		double ut = constrain(t, 0.00, upTime);
		double mt = constrain(t - upTime, 0.00, maxPowerTime);
		double dt = constrain(t - (upTime + maxPowerTime), 0.00, downTime);
		double target_ = acc * sq(ut) * 0.50 + startSpeed * ut
			+ maxSpeed * mt
			+ (-acc * sq(dt) * 0.50) + limitSpeed * dt;

		nowSpeed = (startSpeed + acc * ut + -acc * dt) * (dir ? 1 : -1);//+-込み
		targetPos = target_ * (dir ? 1 : -1) + nowPos;

		if (upTime + downTime + maxPowerTime <= t) {
			nowPos = targetPos;//一つの経路を巡行し終えた時の座標の情報を保持
			myTimer.stop();
			myTimer.reset();
			calState = false;
			nextState = true;
		}
		Print << calState;
	}

	/// @brief 実行状態を取得する
	/// @return セットされている経路を巡行し終えたらtrue
	bool status() const{
		return nextState;
	}
	/// @brief 最終の開始座標を取得する
	/// @return 開始座標
	double getNowPos() const{
		return nowPos;
	}
	/// @brief 台形制御の現在座標を取得する
	/// @return 計算座標
	double getTargetPos() const{
		return targetPos;
	}
	/// @brief 現在速度を取得する
	/// @return 現在速度
	double getNowSpeed() const{
		return nowSpeed;
	}
};

