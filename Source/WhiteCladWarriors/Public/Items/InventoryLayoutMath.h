#pragma once
#include <cstdint>

// Engine-independent arithmetic shared by the implementation and standalone tests.
namespace InventoryLayout
{
    using Int = std::int32_t;

    constexpr Int RequiredSlots(Int Total, Int Max)
    {
        return Total < 0 || Max <= 0 ? -1 : Total / Max + (Total % Max != 0);
    }

    constexpr Int LastAmount(Int Total, Int Max)
    {
        return Total < 0 || Max <= 0 ? -1 : (Total == 0 ? 0 : 1 + (Total - 1) % Max);
    }

    // Positions must be unique, ascending, and within [0, Capacity).
    template <class PositionAt>
    Int FirstGap(Int Capacity, Int Count, PositionAt At)
    {
        Int Expected = 0;
        for (Int Index = 0; Index < Count; ++Index)
        {
            if (At(Index) > Expected) return Expected;
            ++Expected;
        }
        return Expected < Capacity ? Expected : -1;
    }

    template <class PositionAt>
    Int LowerBound(Int Count, Int Position, PositionAt At)
    {
        Int First = 0;
        while (Count > 0)
        {
            const Int Step = Count / 2;
            const Int Mid = First + Step;
            if (At(Mid) < Position)
            {
                First = Mid + 1;
                Count -= Step + 1;
            }
            else Count = Step;
        }
        return First;
    }
}
