#pragma once
#include <type/Vector2.h>

enum class FlexDirection { Row, Column, RowReverse, ColumnReverse };
enum class JustifyContent { FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly };
enum class AlignItems { FlexStart, FlexEnd, Center, Stretch };

struct FlexItem
{
    Hagine::Vector2 preferredSize;
    Hagine::Vector2 minSize;
    Hagine::Vector2 maxSize;
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