#pragma once

//---------------------------------------------------------------------------//
//
// Draw.hpp
//  描画クラス
//   Copyright(C) 2014-2018 tapetums
//
//---------------------------------------------------------------------------//

#include <algorithm>

#if defined(max)
  #undef max
#endif
#if defined(min)
  #undef min
#endif

#include "Bitmap.hpp"

//---------------------------------------------------------------------------//

using COLORREF = unsigned long;

constexpr auto SIZE_OF_COLOR = sizeof(COLORREF);

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace Draw
    {
        inline void SetAlpha             (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t alpha);
        inline void DrawImage            (tapetums::Bitmap* dst, const tapetums::Bitmap& src, int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx = 0, int32_t sy = 0);
        inline void OverlayImage         (tapetums::Bitmap* dst, const tapetums::Bitmap& src, int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx = 0, int32_t sy = 0);
        inline void DrawRect             (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t t, COLORREF color);
        inline void DrawLineH            (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, COLORREF color);
        inline void DrawLineV            (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t h, COLORREF color);
        inline void OverlayLineH         (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, COLORREF color);
        inline void OverlayLineV         (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t h, COLORREF color);
        inline void FillRect             (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color);
        inline void OverlayRect          (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color);
        inline void GradFillRectH        (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color1, COLORREF color2);
        inline void GradFillRectV        (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color1, COLORREF color2);
        inline void GradOverlayRectH     (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color1, COLORREF color2);
        inline void GradOverlayRectV     (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color1, COLORREF color2);
        inline void DrawEllipse          (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color, int32_t tx = 0, int32_t ty = 0, int32_t tw = 0, int32_t th = 0);
        inline void FillEllipse          (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color, int32_t tx = 0, int32_t ty = 0, int32_t tw = 0, int32_t th = 0);
        inline void OverlayEllipse       (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color, int32_t tx = 0, int32_t ty = 0, int32_t tw = 0, int32_t th = 0);
        inline void DrawRoundRect        (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color);
        inline void FillRoundRect        (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color);
        inline void OverlayRoundRect     (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color);
        inline void GradFillRoundRectH   (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color1, COLORREF color2);
        inline void GradFillRoundRectV   (tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color1, COLORREF color2);
        inline void GradOverlayRoundRectH(tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color1, COLORREF color2);
        inline void GradOverlayRoundRectV(tapetums::Bitmap* dst, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color1, COLORREF color2);
    }
}

//---------------------------------------------------------------------------//
// Functions
//---------------------------------------------------------------------------//

inline void tapetums::Draw::SetAlpha
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, uint8_t alpha
)
{
    const auto ww = (w > 0) ? w : -w;
    const auto hh = (h > 0) ? h : -h;

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + ww, dst->width());
    const auto bottom = std::min(y + hh, dst->height());

    const auto dst_pbits  = dst->pbits();
    const auto dst_stride = dst->stride();

    const COLORREF alpha_mask = alpha << 24;
    uint8_t* p = nullptr;

    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = dst_pbits + SIZE_OF_COLOR * left + dst_stride * yy;

        for ( auto xx = left; xx < right; ++xx )
        {
            const auto color = *(COLORREF*)p;

            *(COLORREF*)p = alpha_mask | color;

            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawImage
(
    tapetums::Bitmap* dst, const tapetums::Bitmap& src,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy
)
{
    const auto bit = src.bit();
    if ( bit != 32 )
    {
        return;
    }

    const auto ww = (w > 0) ? w : -w;
    const auto hh = (h > 0) ? h : -h;

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + ww, dst->width());
    const auto bottom = std::min(y + hh, dst->height());

    const auto dst_pbits  = dst->pbits();
    const auto dst_stride = dst->stride();
    const auto src_pbits  = src.pbits();
    const auto src_stride = src.stride();

    int32_t v = (h > 0) ? sy : -h - sy - 1;
    uint8_t* p = nullptr;
    uint8_t* q = nullptr;

    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = dst_pbits + SIZE_OF_COLOR * left + dst_stride * yy;
        q = src_pbits + SIZE_OF_COLOR * sx + src_stride * v;

        for ( auto xx = left; xx < right; ++xx )
        {
            const auto a = *(q + 3);
            if ( a > 0 )
            {
                *(COLORREF*)p = *(COLORREF*)q;
            }

            p += SIZE_OF_COLOR;
            q += SIZE_OF_COLOR;
        }

        (h > 0) ?  ++v : --v;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayImage
(
    tapetums::Bitmap* dst, const tapetums::Bitmap& src,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy
)
{
    const auto bit = src.bit();
    if ( bit != 32 )
    {
        return;
    }

    const auto ww = (w > 0) ? w : -w;
    const auto hh = (h > 0) ? h : -h;

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + ww, dst->width());
    const auto bottom = std::min(y + hh, dst->height());

    const auto dst_pbits  = dst->pbits();
    const auto dst_stride = dst->stride();
    const auto src_pbits  = src.pbits();
    const auto src_stride = src.stride();

    int32_t v = (h > 0) ? 0 : -h - 1;
    uint8_t* p = nullptr;
    uint8_t* q = nullptr;

    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = dst_pbits + SIZE_OF_COLOR * left + dst_stride * yy;
        q = src_pbits + SIZE_OF_COLOR * sx + src_stride * (sy + v);

        for ( auto xx = left; xx < right; ++xx )
        {
            const auto a = *(q + 3);
            if ( a > 0 )
            {
                const auto b = *(q + 0);
                const auto g = *(q + 1);
                const auto r = *(q + 2);

                const auto sb = *(p + 0);
                const auto sg = *(p + 1);
                const auto sr = *(p + 2);
                const auto sa = *(p + 3);

                *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
                *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
                *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
                *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);
            }

            p += SIZE_OF_COLOR;
            q += SIZE_OF_COLOR;
        }

        (h > 0) ?  ++v : --v;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawLineH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, COLORREF color
)
{
    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto p = pbits + SIZE_OF_COLOR * left + stride * top;
    for ( auto xx = left; xx < right; ++xx )
    {
        *(COLORREF*)p = color;
        p += SIZE_OF_COLOR;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawLineV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t h, COLORREF color
)
{
    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto p = pbits + SIZE_OF_COLOR * left + stride * top;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        *(COLORREF*)p = color;
        p += stride;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayLineH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, COLORREF color
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }
    if ( a == 255 )
    {
        DrawLineH(dst, x, y, w, color);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto p = pbits + SIZE_OF_COLOR * left + stride * top;
    for ( auto xx = left; xx < right; ++xx )
    {
        const auto sb = *(p + 0);
        const auto sg = *(p + 1);
        const auto sr = *(p + 2);
        const auto sa = *(p + 3);

        *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
        *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
        *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
        *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);

        p += SIZE_OF_COLOR;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayLineV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t h, COLORREF color
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }
    if ( a == 255 )
    {
        DrawLineV(dst, x, y, h, color);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto p = pbits + SIZE_OF_COLOR * left + stride * top;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        const auto sb = *(p + 0);
        const auto sg = *(p + 1);
        const auto sr = *(p + 2);
        const auto sa = *(p + 3);

        *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
        *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
        *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
        *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);

        p += stride;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    int32_t t, COLORREF color
)
{
    if ( t > 0 )
    {
        DrawLineH(dst, x, y, w, color);
        DrawLineH(dst, x, y + h - 1, w, color);
        DrawLineV(dst, x, y + 1, h - 2, color);
        DrawLineV(dst, x + w - 1, y + 1, h - 2, color);

        DrawRect(dst, x + 1, y + 1, w - 2, h - 2, t - 1, color);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::FillRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color
)
{
    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        for ( auto xx = left; xx < right; ++xx )
        {
            *(COLORREF*)p = color;
            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }
    if ( a == 255 )
    {
        FillRect(dst, x, y, w, h, color);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        for ( auto xx = left; xx < right; ++xx )
        {
            const auto sb = *(p + 0);
            const auto sg = *(p + 1);
            const auto sr = *(p + 2);
            const auto sa = *(p + 3);

            *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
            *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
            *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
            *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);

            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradFillRectH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color1, COLORREF color2
)
{
    if ( w == 0 )
    {
        return;
    }

    const uint8_t a1 = 0xFF & (color1 >> 24);
    const uint8_t r1 = 0xFF & (color1 >> 16);
    const uint8_t g1 = 0xFF & (color1 >>  8);
    const uint8_t b1 = 0xFF & color1;

    const uint8_t a2 = 0xFF & (color2 >> 24);
    const uint8_t r2 = 0xFF & (color2 >> 16);
    const uint8_t g2 = 0xFF & (color2 >>  8);
    const uint8_t b2 = 0xFF & color2;

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto u = 0;
    const auto v = w;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        u = 0;
        for ( auto xx = left; xx < right; ++xx )
        {
            *(p + 0) = (uint8_t)((b1 * (v - u) + b2 * u) / v);
            *(p + 1) = (uint8_t)((g1 * (v - u) + g2 * u) / v);
            *(p + 2) = (uint8_t)((r1 * (v - u) + r2 * u) / v);
            *(p + 3) = (uint8_t)((a1 * (v - u) + a2 * u) / v);

            p += SIZE_OF_COLOR;
            ++u;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradFillRectV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color1, COLORREF color2
)
{
    if ( h == 0 )
    {
        return;
    }

    const uint8_t a1 = 0xFF & (color1 >> 24);
    const uint8_t r1 = 0xFF & (color1 >> 16);
    const uint8_t g1 = 0xFF & (color1 >>  8);
    const uint8_t b1 = 0xFF & color1;

    const uint8_t a2 = 0xFF & (color2 >> 24);
    const uint8_t r2 = 0xFF & (color2 >> 16);
    const uint8_t g2 = 0xFF & (color2 >>  8);
    const uint8_t b2 = 0xFF & color2;

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto u = 0;
    const auto v = h;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        for ( auto xx = left; xx < right; ++xx )
        {
            *(p + 0) = (uint8_t)((b1 * (v - u) + b2 * u) / v);
            *(p + 1) = (uint8_t)((g1 * (v - u) + g2 * u) / v);
            *(p + 2) = (uint8_t)((r1 * (v - u) + r2 * u) / v);
            *(p + 3) = (uint8_t)((a1 * (v - u) + a2 * u) / v);

            p += SIZE_OF_COLOR;
        }

        ++u;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradOverlayRectH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color1, COLORREF color2
)
{
    if ( w == 0 )
    {
        return;
    }
    if ( color1 == color2 )
    {
        OverlayRect(dst, x, y, w, h, color1);
        return;
    }

    const uint8_t a1 = 0xFF & (color1 >> 24);
    const uint8_t r1 = 0xFF & (color1 >> 16);
    const uint8_t g1 = 0xFF & (color1 >>  8);
    const uint8_t b1 = 0xFF & color1;

    const uint8_t a2 = 0xFF & (color2 >> 24);
    const uint8_t r2 = 0xFF & (color2 >> 16);
    const uint8_t g2 = 0xFF & (color2 >>  8);
    const uint8_t b2 = 0xFF & color2;

    if ( a1 == 0 && a2 == 0 )
    {
        return;
    }
    if ( a1 == 255 && a2 == 255 )
    {
        GradFillRectH(dst, x, y, w, h, color1, color2);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto u = 0;
    const auto v = w;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        u = 0;
        for ( auto xx = left; xx < right; ++xx )
        {
            const auto a = (uint8_t)((a1 * (v - u) + a2 * u) / v);
            const auto r = (uint8_t)((r1 * (v - u) + r2 * u) / v);
            const auto g = (uint8_t)((g1 * (v - u) + g2 * u) / v);
            const auto b = (uint8_t)((b1 * (v - u) + b2 * u) / v);

            const auto sb = *(p + 0);
            const auto sg = *(p + 1);
            const auto sr = *(p + 2);
            const auto sa = *(p + 3);

            *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
            *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
            *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
            *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);

            p += SIZE_OF_COLOR;
            ++u;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradOverlayRectV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h,
    COLORREF color1, COLORREF color2
)
{
    if ( h == 0 )
    {
        return;
    }
    if ( color1 == color2 )
    {
        OverlayRect(dst, x, y, w, h, color1);
        return;
    }

    const uint8_t a1 = 0xFF & (color1 >> 24);
    const uint8_t r1 = 0xFF & (color1 >> 16);
    const uint8_t g1 = 0xFF & (color1 >>  8);
    const uint8_t b1 = 0xFF & color1;

    const uint8_t a2 = 0xFF & (color2 >> 24);
    const uint8_t r2 = 0xFF & (color2 >> 16);
    const uint8_t g2 = 0xFF & (color2 >>  8);
    const uint8_t b2 = 0xFF & color2;

    if ( a1 == 0 && a2 == 0 )
    {
        return;
    }
    if ( a1 == 255 && a2 == 255 )
    {
        GradFillRectV(dst, x, y, w, h, color1, color2);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y, dst->height());
    const auto right  = std::min(x + w, dst->width());
    const auto bottom = std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    auto u = 0;
    const auto v = h;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        for ( auto xx = left; xx < right; ++xx )
        {
            const auto a = (uint8_t)((a1 * (v - u) + a2 * u) / v);
            const auto r = (uint8_t)((r1 * (v - u) + r2 * u) / v);
            const auto g = (uint8_t)((g1 * (v - u) + g2 * u) / v);
            const auto b = (uint8_t)((b1 * (v - u) + b2 * u) / v);

            const auto sb = *(p + 0);
            const auto sg = *(p + 1);
            const auto sr = *(p + 2);
            const auto sa = *(p + 3);

            *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
            *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
            *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
            *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);

            p += SIZE_OF_COLOR;
        }

        ++u;
    }
}

//---------------------------------------------------------------------------//
//
// DrawEllipse() / FillEllipse() / OverlayEllipse()
//
//  楕円の標準方程式 ((x - p)/a)^2 + (y - q)/b)^2 = 1 より、
//   (s, t) を左上の点として (w, h) の大きさを持つ長方形に内接する楕円の方程式
//
//   ((x - (s + w/2))/(w/2))^2 + ((y - (t + h/2))/(h/2))^2 = 1
//
//  を変形して
//
//   h^2 * (2*x - 2*s - w)^2 + w^2 * (2*y - 2*t - h)^2 = w^2 * h^2
//
//  を導き、範囲内に存在する点をプロットしている
//
//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawEllipse
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color,
    int32_t tx, int32_t ty, int32_t tw, int32_t th
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x + tx, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y + ty, dst->height());
    const auto right  = (tw > 0 ) ? std::min(x + tx + tw, dst->width())  : std::min(x + w, dst->width());
    const auto bottom = (th > 0 ) ? std::min(y + ty + th, dst->height()) : std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    const int64_t cx = 2 * x + (w - 1);
    const int64_t cy = 2 * y + (h - 1);
    const int64_t ww = (w - 1) * (w - 1);
    const int64_t hh = (h - 1) * (h - 1);
    const int64_t n  = ww * hh;
    const int64_t nn = w * w * h * h;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        const auto m = ww * (2*yy - cy) * (2*yy - cy);
        for ( auto xx = left; xx < right; ++xx )
        {
            const auto l = hh * (2*xx - cx) * (2*xx - cx);
            if ( n < l + m && l + m < nn )
            {
                const auto aa = (uint8_t)(a * (l + m - nn) / (n - nn));
                if ( aa > 0 )
                {
                    const auto sb = *(p + 0);
                    const auto sg = *(p + 1);
                    const auto sr = *(p + 2);
                    const auto sa = *(p + 3);

                    *(p + 0) = (b * aa + (sb * (0xFF ^ aa))) >> 8;
                    *(p + 1) = (g * aa + (sg * (0xFF ^ aa))) >> 8;
                    *(p + 2) = (r * aa + (sr * (0xFF ^ aa))) >> 8;
                    *(p + 3) = aa + ((sa * (0xFF ^ aa)) >> 8);
                }
            }

            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::FillEllipse
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color,
    int32_t tx, int32_t ty, int32_t tw, int32_t th
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x + tx, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y + ty, dst->height());
    const auto right  = (tw > 0 ) ? std::min(x + tx + tw, dst->width())  : std::min(x + w, dst->width());
    const auto bottom = (th > 0 ) ? std::min(y + ty + th, dst->height()) : std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    const int64_t cx = 2 * x + (w - 1);
    const int64_t cy = 2 * y + (h - 1);
    const int64_t ww = (w - 1) * (w - 1);
    const int64_t hh = (h - 1) * (h - 1);
    const auto n  = ww * hh;
    const auto nn = w * w * h * h;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        const auto m = ww * (2*yy - cy) * (2*yy - cy);
        for ( auto xx = left; xx < right; ++xx )
        {
            const auto l = hh * (2*xx - cx) * (2*xx - cx);
            if ( l + m < n )
            {
                *(COLORREF*)p = color;
            }
            else if ( l + m < nn )
            {
                const auto aa = (uint8_t)(a * (l + m - nn) / (n - nn));
                if ( aa > 0 )
                {
                    const auto sb = *(p + 0);
                    const auto sg = *(p + 1);
                    const auto sr = *(p + 2);
                    const auto sa = *(p + 3);

                    *(p + 0) = (b * aa + (sb * (0xFF ^ aa))) >> 8;
                    *(p + 1) = (g * aa + (sg * (0xFF ^ aa))) >> 8;
                    *(p + 2) = (r * aa + (sr * (0xFF ^ aa))) >> 8;
                    *(p + 3) = aa + ((sa * (0xFF ^ aa)) >> 8);
                }
            }

            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayEllipse
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, COLORREF color,
    int32_t tx, int32_t ty, int32_t tw, int32_t th
)
{
    const uint8_t a = 0xFF & (color >> 24);
    const uint8_t r = 0xFF & (color >> 16);
    const uint8_t g = 0xFF & (color >>  8);
    const uint8_t b = 0xFF & color;
    if ( a == 0 )
    {
        return;
    }
    if ( a == 255 )
    {
        FillEllipse(dst, x, y, w, h, color);
        return;
    }

    const auto left   = (x < 0) ? 0 : std::min(x + tx, dst->width());
    const auto top    = (y < 0) ? 0 : std::min(y + ty, dst->height());
    const auto right  = (tw > 0 ) ? std::min(x + tx + tw, dst->width())  : std::min(x + w, dst->width());
    const auto bottom = (th > 0 ) ? std::min(y + ty + th, dst->height()) : std::min(y + h, dst->height());
    const auto pbits  = dst->pbits();
    const auto stride = dst->stride();

    const int64_t cx = 2 * x + (w - 1);
    const int64_t cy = 2 * y + (h - 1);
    const int64_t ww = (w - 1) * (w - 1);
    const int64_t hh = (h - 1) * (h - 1);
    const auto n = ww * hh;
    const auto nn = w * w * h * h;

    uint8_t* p = nullptr;
    for ( auto yy = top; yy < bottom; ++yy )
    {
        p = pbits + SIZE_OF_COLOR * left + stride * yy;

        const auto m = ww * (2*yy - cy) * (2*yy - cy);
        for ( auto xx = left; xx < right; ++xx )
        {
            const auto l = hh * (2*xx - cx) * (2*xx - cx);
            if ( l + m < n )
            {
                const auto sb = *(p + 0);
                const auto sg = *(p + 1);
                const auto sr = *(p + 2);
                const auto sa = *(p + 3);

                *(p + 0) = (b * a + (sb * (0xFF ^ a))) >> 8;
                *(p + 1) = (g * a + (sg * (0xFF ^ a))) >> 8;
                *(p + 2) = (r * a + (sr * (0xFF ^ a))) >> 8;
                *(p + 3) = a + ((sa * (0xFF ^ a)) >> 8);
            }
            else if ( l + m < nn )
            {
                const auto aa = (uint8_t)(a * (l + m - nn) / (n - nn));
                if ( aa > 0 )
                {
                    const auto sb = *(p + 0);
                    const auto sg = *(p + 1);
                    const auto sr = *(p + 2);
                    const auto sa = *(p + 3);

                    *(p + 0) = (b * aa + (sb * (0xFF ^ aa))) >> 8;
                    *(p + 1) = (g * aa + (sg * (0xFF ^ aa))) >> 8;
                    *(p + 2) = (r * aa + (sr * (0xFF ^ aa))) >> 8;
                    *(p + 3) = aa + ((sa * (0xFF ^ aa)) >> 8);
                }
            }

            p += SIZE_OF_COLOR;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::DrawRoundRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        DrawRoundRect(dst, x, y, w, h, std::min(w/2, h/2), color);
    }
    else
    {
        DrawEllipse(dst, x, y, rr, rr, color, 0, 0, r, r);
        DrawEllipse(dst, x + w - rr, y, rr, rr, color, r, 0, r, r);
        DrawEllipse(dst, x, y + h - rr, rr, rr, color, 0, r, r, r);
        DrawEllipse(dst, x + w - rr, y + h - rr, rr, rr, color, r, r, r, r);

        DrawLineH(dst, x + r, y, w - rr, color);
        DrawLineV(dst, x, y + r, h - rr, color);
        DrawLineV(dst, x + w - 1, y + r, h - rr, color);
        DrawLineH(dst, x + r, y + h - 1, w - rr, color);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::FillRoundRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        FillRoundRect(dst, x, y, w, h, std::min(w/2, h/2), color);
    }
    else
    {
        FillEllipse(dst, x, y, rr, rr, color, 0, 0, r, r);
        FillEllipse(dst, x + w - rr, y, rr, rr, color, r, 0, r, r);
        FillEllipse(dst, x, y + h - rr, rr, rr, color, 0, r, r, r);
        FillEllipse(dst, x + w - rr, y + h - rr, rr, rr, color, r, r, r, r);

        FillRect(dst, x, y + r, r, h - rr, color);
        FillRect(dst, x + w - r, y + r, r, h - rr, color);
        FillRect(dst, x + r, y, w - rr, h, color);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::OverlayRoundRect
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, COLORREF color
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        OverlayRoundRect(dst, x, y, w, h, std::min(w/2, h/2), color);
    }
    else
    {
        OverlayEllipse(dst, x, y, rr, rr, color, 0, 0, r, r);
        OverlayEllipse(dst, x + w - rr, y, rr, rr, color, r, 0, r, r);
        OverlayEllipse(dst, x, y + h - rr, rr, rr, color, 0, r, r, r);
        OverlayEllipse(dst, x + w - rr, y + h - rr, rr, rr, color, r, r, r, r);

        OverlayRect(dst, x, y + r, r, h - rr, color);
        OverlayRect(dst, x + w - r, y + r, r, h - rr, color);
        OverlayRect(dst, x + r, y, w - rr, h, color);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradFillRoundRectH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,
    COLORREF color1, COLORREF color2
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        GradFillRoundRectH(dst, x, y, w, h, std::min(w/2, h/2), color1, color2);
    }
    else
    {
        FillEllipse(dst, x, y, rr, rr, color1, 0, 0, r, r);
        FillEllipse(dst, x + w - rr, y, rr, rr, color2, r, 0, r, r);
        FillEllipse(dst, x, y + h - rr, rr, rr, color1, 0, r, r, r);
        FillEllipse(dst, x + w - rr, y + h - rr, rr, rr, color2, r, r, r, r);

        FillRect(dst, x, y + r, r, h - rr, color1);
        FillRect(dst, x + w - r, y + r, r, h - rr, color2);
        GradFillRectH(dst, x + r, y, w - rr, h, color1, color2);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradFillRoundRectV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,
    COLORREF color1, COLORREF color2
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        GradFillRoundRectV(dst, x, y, w, h, std::min(w/2, h/2), color1, color2);
    }
    else
    {
        FillEllipse(dst, x, y, rr, rr, color1, 0, 0, r, r);
        FillEllipse(dst, x + w - rr, y, rr, rr, color1, r, 0, r, r);
        FillEllipse(dst, x, y + h - rr, rr, rr, color2, 0, r, r, r);
        FillEllipse(dst, x + w - rr, y + h - rr, rr, rr, color2, r, r, r, r);

        FillRect(dst, x + r, y, w - rr, r, color1);
        FillRect(dst, x + r, y + h - r, w - rr, r, color2);
        GradFillRectV(dst, x, y + r, w, h - rr, color1, color2);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradOverlayRoundRectH
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,
    COLORREF color1, COLORREF color2
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        GradOverlayRoundRectH(dst, x, y, w, h, std::min(w/2, h/2), color1, color2);
    }
    else
    {
        OverlayEllipse(dst, x, y, rr, rr, color1, 0, 0, r, r);
        OverlayEllipse(dst, x + w - rr, y, rr, rr, color2, r, 0, r, r);
        OverlayEllipse(dst, x, y + h - rr, rr, rr, color1, 0, r, r, r);
        OverlayEllipse(dst, x + w - rr, y + h - rr, rr, rr, color2, r, r, r, r);

        OverlayRect(dst, x, y + r, r, h - rr, color1);
        OverlayRect(dst, x + w - r, y + r, r, h - rr, color2);
        GradOverlayRectH(dst, x + r, y, w - rr, h, color1, color2);
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::Draw::GradOverlayRoundRectV
(
    tapetums::Bitmap* dst,
    int32_t x, int32_t y, int32_t w, int32_t h, int32_t r,
    COLORREF color1, COLORREF color2
)
{
    if ( r < 1 )
    {
        return;
    }

    const auto rr = 2 * r;
    if ( w < rr || h < rr )
    {
        GradOverlayRoundRectV(dst, x, y, w, h, std::min(w/2, h/2), color1, color2);
    }
    else
    {
        OverlayEllipse(dst, x, y, rr, rr, color1, 0, 0, r, r);
        OverlayEllipse(dst, x + w - rr, y, rr, rr, color1, r, 0, r, r);
        OverlayEllipse(dst, x, y + h - rr, rr, rr, color2, 0, r, r, r);
        OverlayEllipse(dst, x + w - rr, y + h - rr, rr, rr, color2, r, r, r, r);

        OverlayRect(dst, x + r, y, w - rr, r, color1);
        OverlayRect(dst, x + r, y + h - r, w - rr, r, color2);
        GradOverlayRectV(dst, x, y + r, w, h - rr, color1, color2);
    }
}

//---------------------------------------------------------------------------//

// Draw.hpp