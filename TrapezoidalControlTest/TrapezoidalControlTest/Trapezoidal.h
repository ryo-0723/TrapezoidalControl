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
	double maxSpeed, limitSpeed;
	double upTime, maxPowerTime, downTime;
	double dir, acc;
	bool calState, nextState;
	Vec2 nowPos;
	Vec2 targetPos;
	Vec2 targetSpeed;
	Vec2 oldTarget;
	unsigned long oldMillis;
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

	int select(double r) {
		if (r >= Math::Pi * 1.5 && r < Math::Pi * 2) {
			return -1;
		}
		else {
			return 1;
		}
	}

public:
	Trapezoidal(double acc, double maxSpeed, Vec2 nowPos)
		: acc(acc)
		, maxSpeed(maxSpeed)
		, nowPos(nowPos)
		, limitSpeed()
		, upTime()
		, maxPowerTime()
		, downTime()
		, dir()
		, calState(false)
		, nextState(true)
		, targetSpeed()
		, oldTarget()
		, oldMillis()
		, targetPos()
		, myTimer()
	{
	}

	Vec2 operator()(Vec2 target,double starSpeed=0,double enSpeed=0){
		return { 0,0 };
	}

	void update(Vec2 target, double startSpeed = 0, double endSpeed = 0) {
		//  X=Vot+(1/2)*at^2;
		if (!calState) {
			const double distance = target.x - nowPos.x;


			upTime = (maxSpeed - startSpeed) / acc; //最高速度までの加速にかかる時間 /s
			downTime = (maxSpeed - endSpeed) / acc; //減速にかかる時間 /s
			double L1 = startSpeed * upTime + acc * sq(upTime) * 0.5; //加速時における移動距離 /m
			double L3 = endSpeed * downTime + acc * sq(downTime) * 0.5; //減速時における移動距離/m
			if (abs(L1 + L3) > abs(distance)) {//台形ができなくなり、三角形になるときの制御
				limitSpeed = 2.00 * acc * abs(distance) * 0.5 + sq(startSpeed);
				limitSpeed = sqrt(limitSpeed);
				upTime = (limitSpeed - startSpeed) / acc;
				downTime = (limitSpeed - endSpeed) / acc;
				maxPowerTime = 0.0;
			}
			else {//台形が作れる場合の制御 
				maxPowerTime = (abs(distance) - L1 - L3) / maxSpeed; //最高速度での移動時間 /s
				limitSpeed = maxSpeed;
			}
			calState = true;
		}
		if (calState)
			myTimer.start();//タイマーを開始する←何度呼び出しても一度だけ実行される

		double t = myTimer.getTime() * 0.001;  //s
		double ut = constrain(t, 0.00, upTime);
		double dt = constrain(t - (upTime + maxPowerTime), 0.00, downTime);
		double target_ = acc * sq(ut) * 0.50 + startSpeed * ut
			+ maxSpeed * constrain(t - upTime, 0.00, maxPowerTime)
			+ (-acc * sq(dt) * 0.50) + limitSpeed * dt;

		targetPos.x = target_ + nowPos.x;
		if (upTime + downTime + maxPowerTime <= t) {
			nowPos.x = targetPos.x;//一つの経路を巡行し終えた時の座標の情報を保持
			myTimer.stop();
			myTimer.reset();
			calState = false;
			nextState = true;
		}

		// difdistance / diftime
		targetSpeed.x = (getTargetPos().x - oldTarget.x) / (((double)millis() * 0.001) - oldMillis);  //m/s
		oldMillis = millis() * 0.001;
		oldTarget = getTargetPos();
	}

	/// @brief 実行状態を返す
	/// @return 今回の経路を巡行し終えたらtrue
	bool next_status() {
		return nextState;
	}
	void set_next_status() {//次の経路をセットしたら必ず行う
		nextState = false;
	}
	Vec2 getNowPos() {
		return nowPos;
	}
	Vec2 getTargetPos() {
		return targetPos;
	}
};

