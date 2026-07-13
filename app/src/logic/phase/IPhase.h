#pragma once

// フェーズの基底クラス
// フェーズはウェーブ中の状態を表す
// 例：Setupフェーズ、Battleフェーズ、Bossフェーズなど
// フェーズはフェーズディレクターによって管理される

class IPhase
{
public:
    virtual ~IPhase() = default;
    virtual void Enter() = 0;
    virtual void Exit() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
};