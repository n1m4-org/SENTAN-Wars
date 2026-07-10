#pragma once
#include "Character/Player/Sentan/Sentan.h"
#include "Component/AttributeComponent.h"
#include "Component/Component.h"
#include "type/Vector3.h"
#include <memory>
#include <vector>

namespace Hagine {
class BaseObject;
class ViewProjection;
} // namespace Hagine

/// プレイヤーのSENTAN(武器)を装備・所有・更新・描画する汎用コンポーネント
/// SENTANの所有と装備ロジックを本体（Player）から切り離すためのもの
///
/// 更新は「装備先(owner)の子」としてワールド変換の親子関係で行われるため、
/// このコンポーネント自身は Init(装備) と Draw(描画) だけを担当する
class SentanComponent : public Component {
  public:
    /// owner はSENTANの装備先（親）。ここに追従させる
    explicit SentanComponent(Hagine::BaseObject *owner) : owner_(owner) {}

    void Init() override;

    void Draw(const Hagine::ViewProjection &viewProjection) override;

  private:
    /// SENTAN(武器)を装備する（仕様により最大2つまで）
    /// localOffset は装備先からの相対位置
    Sentan *EquipSentan(AttributeType type, const Hagine::Vector3 &localOffset);

  private:
    // 装備先（親）。所有はしない・参照するだけ
    Hagine::BaseObject *owner_ = nullptr;

    // SENTANは最大2つまで
    static constexpr size_t kMaxSentanCount = 2;
    // SENTANの装備位置：装備先中心から左右への横オフセット
    static constexpr float kSentanEquipOffsetX = 1.5f;

    // 装備中のSENTAN(このコンポーネントが所有)
    std::vector<std::unique_ptr<Sentan>> sentans_;
};
