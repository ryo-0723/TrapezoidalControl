#pragma once

#include<Siv3D.hpp>

//ベジェ曲線の作図用クラス
//点を30個描画して疑似的な曲線を再現する

/*
＜流れ＞
点を4つ描画（直線を二つ描画する）
二つ直線を描画したらすぐに曲線が表示されるようにする
それをもとにリアルタイムで描画を更新し続ける

オブジェクトの生成と共に選択された点を基準に曲線描画用のセットを作成する
*/
/*
制御点の格納された配列を受け取り、この中で一つずつのベジェ曲線に分割する
クラスの継承、
表示関数と、何個目の曲線か、各曲線の長さ、各曲線でのある長さでの座標取得関数の実装
制御点の変更関数の実装も
*/
namespace Udon {
	class Bezier3 {
		std::vector<Vec2>::const_iterator beginIt;
		int n = 1000;
	public:
		/// @brief 配列の任意のイテレータを引数にそこから4つの座標を取得し、描画する
		/// @param beginIt 
		Bezier3(std::vector<Vec2>::const_iterator beginIt)
		:beginIt(beginIt)
		{}

		Bezier3& getLength() {




			return *this;
		}

		Vec2 getPos(const double tar) {
			//0 <= tar <= 1.0
			const auto px = (1 - tar) * (1 - tar) * (1 - tar) * (*beginIt).x + tar * (3 * (1 - tar) * (1 - tar) * (*(beginIt + 1)).x + tar * (3 * (1 - tar) * (*(beginIt + 2)).x + (*(beginIt + 3)).x));
			const auto py = (1 - tar) * (1 - tar) * (1 - tar) * (*beginIt).y + tar * (3 * (1 - tar) * (1 - tar) * (*(beginIt + 1)).y + tar * (3 * (1 - tar) * (*(beginIt + 2)).y + (*(beginIt + 3)).y));
			return Vec2{px,py};
		}

		Bezier3& draw(Color& color) {

			return *this;
		}
	};

	class Beziers3 {
	private:

	public:
		Beziers3(std::vector<Vec2>& point) {}



		Beziers3& getLength() {
			return *this;
		}

		Beziers3& draw(Color& color) {

			return *this;
		}
	};
}
