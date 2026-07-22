#pragma once
#include <type/Vector2.h>

enum class FlexDirection { Row, Column, RowReverse, ColumnReverse, Count };
enum class JustifyContent { FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly, Count };
enum class AlignItems { FlexStart, FlexEnd, Center, Stretch, Count };

struct FlexItem
{
    Hagine::Vector2 preferredSize;
};

struct FlexBox
{
    Hagine::Vector2 position;
    Hagine::Vector2 size;
};

struct FlexResult
{
    Hagine::Vector2 position;
    Hagine::Vector2 size;
};

struct CrossAlignResult
{
    float position;
    float size;
};