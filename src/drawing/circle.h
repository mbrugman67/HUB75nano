#ifndef HUB75NANO_CIRCLE_H
#define HUB75NANO_CIRCLE_H

#include "drawing_common.h"

#ifdef PANEL_MAX_SPEED
__attribute__((always_inline))
#endif
inline void
drawCircle(uint8_t xMiddle, uint8_t yMiddle, uint8_t radius, Color color, bool fill)
{
    if (fill)
    {
        fillCircle(xMiddle, yMiddle, radius, color);
    }
    else
    {
        drawCircleHollow(xMiddle, yMiddle, radius, color);
    }
}

void drawCircleHollow(uint8_t xMiddle, uint8_t yMiddle, uint8_t radius, Color color)
{
    switch (radius)
    {
    case 0:
        setBuffer(xMiddle, yMiddle, color);
        break;
    case 1:
        setBuffer(xMiddle, yMiddle, color);
        setBuffer(xMiddle + 1, yMiddle, color);
        setBuffer(xMiddle, yMiddle + 1, color);
        setBuffer(xMiddle - 1, yMiddle, color);
        setBuffer(xMiddle, yMiddle - 1, color);
        break;

    default:
        // draws a circle at the coords with radius and color
        int8_t x = -radius;
        int8_t y = 0;
        int8_t new_radius = radius;
        int16_t err = 2 - 2 * radius; // bottom left to top right
        do
        {
            setBuffer(xMiddle - x, yMiddle + y, color); //   I. Quadrant +x +y
            setBuffer(xMiddle - y, yMiddle - x, color); //  II. Quadrant -x +y
            setBuffer(xMiddle + x, yMiddle - y, color); // III. Quadrant -x -y
            setBuffer(xMiddle + y, yMiddle + x, color); //  IV. Quadrant +x -y
            new_radius = err;
            if (new_radius <= y)
                err += ++y * 2 + 1;        // e_xy+e_y < 0
            if (new_radius > x || err > y) // e_xy+e_x > 0 or no 2nd y-step
                err += ++x * 2 + 1;        // -> x-step now
        } while (x < 0);
        break;
    }
}

void fillCircle(uint8_t xMiddle, uint8_t yMiddle, uint8_t radius, Color color)
{
    switch (radius)
    {
    case 0:
        setBuffer(xMiddle, yMiddle, color);
        break;
    case 1:
        setBuffer(xMiddle, yMiddle, color);
        setBuffer(xMiddle + 1, yMiddle, color);
        setBuffer(xMiddle, yMiddle + 1, color);
        setBuffer(xMiddle - 1, yMiddle, color);
        setBuffer(xMiddle, yMiddle - 1, color);
        break;

    default:
        // todo do we need the bresenham if we have to fill it anyways?
        // draws a circle at the coords with radius and color
        int8_t x = -radius;
        int8_t y = 0;
        int8_t new_radius = radius;
        int16_t err = 2 - 2 * radius; // bottom left to top right
        do
        {
            setBuffer(xMiddle - x, yMiddle + y, color); //   I. Quadrant +x +y
            setBuffer(xMiddle - y, yMiddle - x, color); //  II. Quadrant -x +y
            setBuffer(xMiddle + x, yMiddle - y, color); // III. Quadrant -x -y
            setBuffer(xMiddle + y, yMiddle + x, color); //  IV. Quadrant +x -y
            new_radius = err;
            if (new_radius <= y)
                err += ++y * 2 + 1;        // e_xy+e_y < 0
            if (new_radius > x || err > y) // e_xy+e_x > 0 or no 2nd y-step
                err += ++x * 2 + 1;        // -> x-step now
        } while (x < 0);

        // check if point in circle, then fill
        for (int8_t i = -radius; i < radius; i++)
        {
            for (int8_t j = -radius; j < radius; j++)
            {
                if (((int16_t)i * i + (int16_t)j * j) < ((radius - 0.5) * (radius - 0.5)))
                {
                    setBuffer(xMiddle + i, yMiddle + j, color);
                }
            }
        }
        break;
    }
}

#endif // HUB75NANO_CIRCLE_H