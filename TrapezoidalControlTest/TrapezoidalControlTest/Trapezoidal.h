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
	bool nextState,dir ;
	double nowPos, nowSpeed;
	double targetPos,oldTarget;
	double startSpeed, endSpeed;
	bool calState;
	double all;
	double dis;
	double nextNowPos, nextNowSpeed;
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
	/// @param acc mm/s^2 加速度
	/// @param maxSpeed mm/s 最大速度 
	/// @param nowPos mm スタート座標
	Trapezoidal(double acc, double maxSpeed, double nowPos)
		: acc(acc*0.001*0.001)//mm / ms^2
		, maxSpeed(maxSpeed*0.001)//mm/ms
		, nowPos(nowPos)
		, limitSpeed()
		, upTime()
		, maxPowerTime()
		, downTime()
		, nextState(true)
		, dir()
		, nowSpeed()
		, targetPos(nowPos)
		, oldTarget(nowPos)
		, startSpeed()
		, endSpeed()
		, calState(false)
		,all()
		,dis()
		,nextNowPos()
		,nextNowSpeed()
		, myTimer()
	{
	}

	//入力はs単位で計算時はms単位にする

	//double operator()(double target, double starSpeed = 0, double enSpeed = 0) {
	//	return 0;
	//}

	/// @brief 目標値を計算する
	/// @param target mm 目標値
	/// @param endS mm/s 巡行後の終了スピード(デフォルトで0mm/s)
	void update(const double target, const double endS = 0.0) {
		// X=Vot+(1/2)*at^2;
		// V=a*t +Vo;
		// V^2-Vo^2 =2as;
		if (oldTarget != target) {//目標座標が変わったときは計算しなおす
			myTimer.stop();
			myTimer.reset();
			nextState = false;
			oldTarget = target;

			nowPos = nextNowPos;//途中までの経路情報の保持
			startSpeed = nextNowSpeed;//最初のスピードは前の経路情報をほじ
			endSpeed = endS * 0.001;
			const double distance = target - nowPos;//距離を求める
			dir = (distance > 0.0 ? 1 : 0);//増減の方向を決める

			if (!dir) {//マイナス方向への進行の場合
				endSpeed = abs(endSpeed) * -1;
				maxSpeed = abs(maxSpeed) * -1;
				acc = abs(acc) * -1;
			} else{
				endSpeed = abs(endSpeed);
				maxSpeed = abs(maxSpeed);
				acc = abs(acc);
			}

			upTime = (maxSpeed - startSpeed) / acc; //最高速度までの加速にかかる時間 ms
			downTime = (maxSpeed - endSpeed) / acc; //減速にかかる時間 ms
			if (upTime < 0) {
				downTime -= upTime;
			}
			//
			const double L1 = startSpeed * upTime + acc * sq(upTime) * 0.5; //加速時における移動距離 mm
			const double L3 = endSpeed * downTime + acc * sq(downTime) * 0.5; //減速時における移動距離 mm
			if (abs(L1) + abs(L3) > abs(distance)) { //台形ができなくなり、三角形になるときの制御
				limitSpeed = 2.00 * acc * distance * 0.5 + sq(startSpeed);//+限定
				limitSpeed = sqrt(limitSpeed) * (dir ? 1 : -1);//必ず＋になるため、向きによって＋ーを変える
				if (abs(limitSpeed) < abs(startSpeed)) {
					limitSpeed = startSpeed;
				}
				upTime = (limitSpeed - startSpeed) / acc;//必ず0以上になる保証なし
				downTime = (limitSpeed - endSpeed) / acc;
				maxPowerTime = 0.0;
				//limitSpeed,upTimeがマイナスになるかもしれない？
			}
			else {//台形が作れる場合の制御
				maxPowerTime = (distance - L1 - L3) / maxSpeed; //最高速度での移動時間 ms
				limitSpeed = maxSpeed;
			}
			myTimer.start();//タイマーを開始する←何度呼び出しても一度だけ実行される

			all = acc * sq(upTime) * 0.50 + startSpeed * upTime
				+ maxSpeed * maxPowerTime
				+ (-acc * sq(downTime) * 0.50) + limitSpeed * downTime + nowPos;

			dis = distance +nowPos;
			//distanceと計算した距離が同じじゃない
			//何度計算しなおしても同じっぽい
			//startSpeedが原因？
		}

		double t = myTimer.getTime();  //ms
		double ut = constrain(t, 0.00, upTime);
		double mt = constrain(t - upTime, 0.00, maxPowerTime);
		double dt = constrain(t - (upTime + maxPowerTime), 0.00, downTime);
		double targets = acc * sq(ut) * 0.50 + startSpeed * ut
			+ maxSpeed * mt
			+ (-acc * sq(dt) * 0.50) + limitSpeed * dt;

		if (!nextState) {
			nowSpeed = startSpeed + acc * ut + -acc * dt;
			targetPos = targets + nowPos;
		}
		nextNowPos = targetPos;
		nextNowSpeed = nowSpeed;

		if (upTime + downTime + maxPowerTime <= t) {
			nowPos = targetPos;//一つの経路を巡行し終えた時の座標の情報を保持
			myTimer.stop();
			myTimer.reset();
			nextState = true;
			//oldTarget = nowPos;
		}

		Print << oldTarget;
		Print << all;
		Print << dis;
	}

	double getTime() {
		return myTimer.getTime();
	}

	/// @brief 実行状態を取得する
	/// @return セットされている経路を巡行し終えたらtrue
	bool status() const{
		return nextState;
	}
	/// @brief 最終の開始座標を取得する
	/// @return 開始座標 mm
	double getNowPos() const{
		return nowPos;
	}
	/// @brief 台形制御の現在座標を取得する
	/// @return 計算座標 mm
	double getTargetPos() const{
		return targetPos;
	}
	/// @brief 現在速度を取得する
	/// @return 現在速度 m/s
	double getNowSpeed() const{
		return nowSpeed * 1000;
	}
};

