#pragma once
#include "type/Vector3.h"
#include <algorithm>
#include <cmath>

/// プレイヤーと敵が動けるフィールドの範囲（原点中心・一辺200の正方形）
/// 地面の plane_1 が原点中心・スケール100（＝一辺200）なので、それに合わせている
namespace FieldBounds {

// フィールドの中心（地面の中心が原点のため）
inline constexpr float kCenterX = 0.0f;
inline constexpr float kCenterZ = 0.0f;

// 一辺200の正方形なので、中心から端までの距離（ハーフサイズ）は100
inline constexpr float kHalfSize = 100.0f;

/// XZ平面で見たときの外接半径を求める（体の角までの距離）
/// halfSize は中心から端までの大きさ（当たり判定の持ち方と同じ）
/// 向きが変わっても角がはみ出さないよう、対角線の長さを半径として扱う
inline float HorizontalRadius(const Hagine::Vector3 &halfSize) {
    return std::sqrt(halfSize.x * halfSize.x + halfSize.z * halfSize.z);
}

/// XZ座標をフィールドの内側に収める（高さYはそのまま）
/// margin を渡すと、その分だけ内側で止まる（体の半径ぶんはみ出させたくないときに使う）
/// 位置は体の中心なので、marginを渡さないと半身がはみ出したところで止まる
inline void Clamp(Hagine::Vector3 &pos, float margin = 0.0f) {
    // 体がフィールドより大きいときに範囲が反転しないようにしておく
    const float limit = (std::max)(kHalfSize - margin, 0.0f);
    pos.x = std::clamp(pos.x, kCenterX - limit, kCenterX + limit);
    pos.z = std::clamp(pos.z, kCenterZ - limit, kCenterZ + limit);
}

} // namespace FieldBounds
