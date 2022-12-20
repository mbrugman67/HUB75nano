#include "Panel.h"

Panel::Panel()
{
    coloumns = PANEL_X;
    rows = PANEL_Y;
    pinMode(RA, OUTPUT);
    pinMode(RB, OUTPUT);
    pinMode(RC, OUTPUT);
    pinMode(RD, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(RF, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(GF, OUTPUT);
    pinMode(GS, OUTPUT);
    pinMode(BF, OUTPUT);
    pinMode(BS, OUTPUT);
    pinMode(LAT, OUTPUT);
    pinMode(OE, OUTPUT);

    // each LED struct contains 8 leds, rows * cols in total, so rows*cols/8 is needed
    bsize = rows * (coloumns / 8);
}

void convertColor(uint16_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{ // input color, get converted color by reference
    switch (color)
    {
    case Panel::RED:
        *red = 1;
        *green = 0;
        *blue = 0;
        break;
    case Panel::GREEN:
        *red = 0;
        *green = 1;
        *blue = 0;
        break;
    case Panel::BLUE:
        *red = 0;
        *green = 0;
        *blue = 1;
        break;
    case Panel::WHITE:
        *red = 1;
        *green = 1;
        *blue = 1;
        break;
    case Panel::BLACK:
        *red = 0;
        *green = 0;
        *blue = 0;
        break;
    case Panel::PURPLE:
        *red = 1;
        *green = 0;
        *blue = 1;
        break;
    case Panel::YELLOW:
        *red = 1;
        *green = 1;
        *blue = 0;
        break;
    case Panel::CYAN:
        *red = 0;
        *green = 1;
        *blue = 1;
        break;
#ifdef PANEL_BIG
    case Panel::DEEPRED:
        *red = 2;
        *green = 0;
        *blue = 0;
        break;
    case Panel::DEEPGREEN:
        *red = 0;
        *green = 2;
        *blue = 0;
        break;
    case Panel::DEEPBLUE:
        *red = 0;
        *green = 0;
        *blue = 2;
        break;
    case Panel::DEEPWHITE:
        *red = 2;
        *green = 2;
        *blue = 2;
    case Panel::DEEPERWHITE:
        *red = 3;
        *green = 3;
        *blue = 3;
        break;
    case Panel::DEEPCYAN:
        *red = 0;
        *green = 2;
        *blue = 2;
        break;
    case Panel::DARKYELLOW:
        *red = 1;
        *green = 2;
        *blue = 0;
        break;
    case Panel::DEEPPURPLE:
        *red = 2;
        *green = 0;
        *blue = 2;
        break;
    case Panel::DEEPYELLOW:
        *red = 2;
        *green = 2;
        *blue = 0;
        break;
    case Panel::TURQUOISE:
        *red = 0;
        *green = 1;
        *blue = 2;
        break;
    case Panel::PINK:
        *red = 1;
        *green = 0;
        *blue = 2;
        break;
    case Panel::DARKPURPLE:
        *red = 2;
        *green = 0;
        *blue = 1;
        break;
    case Panel::BRIGHTGREEN:
        *red = 2;
        *green = 1;
        *blue = 0;
        break;
    case Panel::BRIGHTCYAN:
        *red = 2;
        *green = 1;
        *blue = 1;
        break;
    case Panel::MEDIUMGREEN:
        *red = 2;
        *green = 1;
        *blue = 2;
        break;
    case Panel::DEEPERPURPLE:
        *red = 3;
        *green = 0;
        *blue = 3;
        break;
    case Panel::OCEANBLUE:
        *red = 0;
        *green = 2;
        *blue = 1;
        break;
    case Panel::FLESH:
        *red = 1;
        *green = 2;
        *blue = 2;
        break;
    case Panel::LIGHTPINK:
        *red = 1;
        *green = 2;
        *blue = 1;
        break;
    case Panel::DEEPERBLUE:
        *red = 0;
        *green = 0;
        *blue = 3;
        break;
#endif
    default:
        HIGH_TO_FULL_COLOR(color, red, green, blue);
        break;
    }
}

inline void Panel::selectLine(uint8_t lineIndex)
{ // selects one of the 16 lines, 0 based
    SET_ROW_PINS(lineIndex);
    LATCH_DATA;
}

void Panel::fillScreenShift(uint8_t s, uint8_t f, uint8_t o)
{ // creates interesting patterns (shift, factor, offset) | I honestly dont know what it does, @hexchen made this
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    for (uint8_t r = 0; r < rows / 2; r++)
    {
        // switch through all rows
        selectLine(r);

        for (uint8_t c = 0; c < coloumns; c++)
        {
            // c = coloumn r = row s = shift for moving fist number is offset for color, second for overall
            r = ((c + 0 + r + s * 1 + o) % f) == 0;
            b = ((c + 1 + r + s * 2 + o) % f) == 0;
            g = ((c + 2 + r + s * 3 + o) % f) == 0;
            r ^= ((c + 3 - r + s * 1 + o) % f) == 0;
            g ^= ((c + 4 - r + s * 2 + o) % f) == 0;
            b ^= ((c + 5 - r + s * 3 + o) % f) == 0;
            sendTwoPixels(r, g, b, r, g, b);
        }
        LATCH_DATA; // general latch to get rid of ghosting, or so i thought
    }
}

void Panel::fillScreenColor(uint16_t c)
{ // fills the screeen with the set color
    // switches all the colors and sets the values depending on colors
    HIGH_TO_FULL_COLOR(c, &r, &g, &b); // gets first couple colors

    for (uint8_t i = 0; i < 16; i++)
    {
        for (uint8_t row = 0; row < rows / 2; row++)
        {
            // switch through all rows
            sendWholeRow(r > i, g > i, b > i, r > i, g > i, b > i);
            selectLine(row);
        }
    }
}

inline void Panel::sendTwoPixels(uint8_t ru, uint8_t gu, uint8_t bu, uint8_t rl, uint8_t gl, uint8_t bl)
{ // sends two pixels, one in upper half, one in lower half to display | first upper half values, the lower half
    // set all pins at once
    SET_COLOR(ru | gu << 1 | bu << 2 | rl << 3 | gl << 4 | b << 5);
    CLOCK;
}

void Panel::sendWholeRow(uint8_t ru, uint8_t gu, uint8_t bu, uint8_t rl, uint8_t gl, uint8_t bl)
{ // sends two rows of pixels to display | first upper half values, the lower half
    SET_COLOR(ru | gu << 1 | bu << 2 | rl << 3 | gl << 4 | bl << 5);

    for (uint8_t i = 0; i < coloumns; i++)
    {
        CLOCK;
    }
    LATCH_DATA;
}

void Panel::fillBuffer(uint16_t color)
{
    // get colors
    convertColor(color, &r, &g, &b);

    // fills the buffer
    for (uint8_t x = 0; x < PANEL_X; x++)
    {
        for (uint8_t y = 0; y < PANEL_X; y++)
        {
            setBuffer(x, y, r, g, b);
        }
    }
}

void Panel::setBuffer(uint8_t x, uint8_t y, uint8_t red, uint8_t green, uint8_t blue)
{
#ifndef PANEL_BIG
    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint8_t index = (y * coloumns + x) / 4;
        switch (x % 4)
        {
        case 0: /*first pixel*/
            buffer[index].redUpper1 = red;
            buffer[index].greenUpper1 = green;
            buffer[index].blueUpper1 = blue;
            break;
        case 1: /*second pixel*/
            buffer[index].redUpper2 = red;
            buffer[index].greenUpper2 = green;
            buffer[index].blueUpper2 = blue;
            break;
        case 2: /*third pixel*/
            buffer[index].redUpper3 = red;
            buffer[index].greenUpper3 = green;
            buffer[index].blueUpper3 = blue;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redUpper4 = red;
            buffer[index].greenUpper4 = green;
            buffer[index].blueUpper4 = blue;
            break;

        default:
            break;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint8_t index = (y * coloumns + x) / 4;
        switch (x % 4)
        {
        case 0: /*first pixel*/
            buffer[index].redLower1 = red;
            buffer[index].greenLower1 = green;
            buffer[index].blueLower1 = blue;
            break;
        case 1: /*second pixel*/
            buffer[index].redLower2 = red;
            buffer[index].greenLower2 = green;
            buffer[index].blueLower2 = blue;
            break;
        case 2: /*third pixel*/
            buffer[index].redLower3 = red;
            buffer[index].greenLower3 = green;
            buffer[index].blueLower3 = blue;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redLower4 = red;
            buffer[index].greenLower4 = green;
            buffer[index].blueLower4 = blue;
            break;

        default:
            break;
        }
    }

#else

    if (y < (PANEL_Y / 2))
    {
        // we are in upper half of pixels
        uint8_t index = (y * coloumns + x) / 4;
        switch (x % 4)
        {
        case 0: /*first pixel*/
            buffer[index].redUpperBit1Led1 = red;
            buffer[index].greenUpperBit1Led1 = green;
            buffer[index].blueUpperBit1Led1 = blue;
            buffer[index].redUpperBit2Led1 = red >> 1;
            buffer[index].greenUpperBit2Led1 = green >> 1;
            buffer[index].blueUpperBit2Led1 = blue >> 1;
            break;
        case 1: /*second pixel*/
            buffer[index].redUpperBit1Led2 = red;
            buffer[index].greenUpperBit1Led2 = green;
            buffer[index].blueUpperBit1Led2 = blue;
            buffer[index].redUpperBit2Led2 = red >> 1;
            buffer[index].greenUpperBit2Led2 = green >> 1;
            buffer[index].blueUpperBit2Led2 = blue >> 1;
            break;
        case 2: /*third pixel*/
            buffer[index].redUpperBit1Led3 = red;
            buffer[index].greenUpperBit1Led3 = green;
            buffer[index].blueUpperBit1Led3 = blue;
            buffer[index].redUpperBit2Led3 = red >> 1;
            buffer[index].greenUpperBit2Led3 = green >> 1;
            buffer[index].blueUpperBit2Led3 = blue >> 1;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redUpperBit1Led4 = red;
            buffer[index].greenUpperBit1Led4 = green;
            buffer[index].blueUpperBit1Led4 = blue;
            buffer[index].redUpperBit2Led4 = red >> 1;
            buffer[index].greenUpperBit2Led4 = green >> 1;
            buffer[index].blueUpperBit2Led4 = blue >> 1;
            break;

        default:
            break;
        }
    }
    else
    {
        y -= (PANEL_Y / 2);
        // we are in lower half of pixels
        uint8_t index = (y * coloumns + x) / 4;
        switch (x % 4)
        {
        case 0:
            buffer[index].redLowerBit1Led1 = red;
            buffer[index].greenLowerBit1Led1 = green;
            buffer[index].blueLowerBit1Led1 = blue;
            buffer[index].redLowerBit2Led1 = red >> 1;
            buffer[index].greenLowerBit2Led1 = green >> 1;
            buffer[index].blueLowerBit2Led1 = blue >> 1;
            break;
        case 1: /*second pixel*/
            buffer[index].redLowerBit1Led2 = red;
            buffer[index].greenLowerBit1Led2 = green;
            buffer[index].blueLowerBit1Led2 = blue;
            buffer[index].redLowerBit2Led2 = red >> 1;
            buffer[index].greenLowerBit2Led2 = green >> 1;
            buffer[index].blueLowerBit2Led2 = blue >> 1;
            break;
        case 2: /*third pixel*/
            buffer[index].redLowerBit1Led3 = red;
            buffer[index].greenLowerBit1Led3 = green;
            buffer[index].blueLowerBit1Led3 = blue;
            buffer[index].redLowerBit2Led3 = red >> 1;
            buffer[index].greenLowerBit2Led3 = green >> 1;
            buffer[index].blueLowerBit2Led3 = blue >> 1;
            break;
        case 3: /*fourth pixel*/
            buffer[index].redLowerBit1Led4 = red;
            buffer[index].greenLowerBit1Led4 = green;
            buffer[index].blueLowerBit1Led4 = blue;
            buffer[index].redLowerBit2Led4 = red >> 1;
            buffer[index].greenLowerBit2Led4 = green >> 1;
            buffer[index].blueLowerBit2Led4 = blue >> 1;
            break;

        default:
            break;
        }
    }
#endif
}

void Panel::displayBuffer()
{ // puts the  buffer contents onto the panel
#ifndef PANEL_BIG
    for (uint16_t index = 0; index < bsize; index++)
    {
        // one led struct contains bits in 3 bytes:
        // |23  22  21  20  19  18  17  16 |15  14  13  12  11  10  9   8  |7   6   5   4   3   2   1   0  |
        // |bl4:gl4:rl4:bu4:gu4:ru4:bl3:gl3|rl3:bu3:gu3:ru3:bl2:gl2:rl2:bu2|gu2:ru2:bl1:gl1:rl1:bu1:gu1:ru1|

        // first pixels
        SET_COLOR(*(uint8_t *)(&buffer[index]));
        CLOCK;

        // second pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(&buffer[index])) >> 6)));
        CLOCK;

        // 3rd pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(((uint8_t *)(&buffer[index]) + sizeof(uint8_t))))) >> 4));
        CLOCK;

        // 4th pixels
        SET_COLOR(((*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 2))) >> 2));
        CLOCK;

        if ((index + 1) % PANEL_CHUNKSIZE == 0) /*16 = 64 / 4 for 4 pixels per loop iteration*/
        {
            SET_ROW_PINS(index / PANEL_CHUNKSIZE);
            LATCH_DATA;
        }
    }
#else

    // one led struct contains bits in 6 bytes:

    // first 3
    // |23    22    21    20    19    18    17    16   |15    14    13    12    11    10    9     8    |7     6     5     4     3     2     1     0    |
    // |bl2b2:gl2b2:rl2b2:bu2b2:gu2b2:ru2b2:bl2b1:gl2b1|rl2b1:bu2b1:gu2b1:ru2b1:bl1b2:gl1b2:rl1b2:bu1b2|gu1b2:ru1b2:bl1b1:gl1b1:rl1b1:bu1b1:gu1b1:ru1b1|

    // second 3
    // |23    22    21    20    19    18    17    16   |15    14    13    12    11    10    9     8    |7     6     5     4     3     2     1     0    |
    // |bl4b2:gl4b2:rl4b2:bu4b2:gu4b2:ru4b2:bl4b1:gl4b1|rl4b1:bu4b1:gu4b1:ru4b1:bl3b2:gl3b2:rl3b2:bu3b2|gu3b2:ru3b2:bl3b1:gl3b1:rl3b1:bu3b1:gu3b1:ru3b1|

    // msb pass 1
    for (uint16_t index = 0; index < bsize; index++)
    {
        // first pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(&buffer[index])) >> 6)));
        CLOCK;

        // second pixels
        SET_COLOR(((*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 2))) >> 2));
        CLOCK;

        // 3rd pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 3)))) >> 6));
        CLOCK;

        // 4th pixels
        SET_COLOR(((*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 5))) >> 2));
        CLOCK;

        if ((index + 1) % PANEL_CHUNKSIZE == 0)
        {
            SET_ROW_PINS(index / PANEL_CHUNKSIZE);
            LATCH_DATA;
        }
    }

    // lsb
    for (uint16_t index = 0; index < bsize; index++)
    {
        // first pixels
        SET_COLOR(*((uint8_t *)(&buffer[index])));
        CLOCK;

        // second pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(((uint8_t *)(&buffer[index])) + sizeof(uint8_t)))) >> 4));
        CLOCK;

        // 3rd pixels
        SET_COLOR(*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 3)));
        CLOCK;

        // 4th pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 4)))) >> 4));
        CLOCK;

        if ((index + 1) % PANEL_CHUNKSIZE == 0)
        {
            SET_ROW_PINS(index / PANEL_CHUNKSIZE);
            LATCH_DATA;
        }
    }

    // msb pass 2
    for (uint16_t index = 0; index < bsize; index++)
    {
        // first pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(&buffer[index])) >> 6)));
        CLOCK;

        // second pixels
        SET_COLOR(((*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 2))) >> 2));
        CLOCK;

        // 3rd pixels
        SET_COLOR((uint8_t)((*((uint16_t *)(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 3)))) >> 6));
        CLOCK;

        // 4th pixels
        SET_COLOR(((*(((uint8_t *)(&buffer[index])) + (sizeof(uint8_t) * 5))) >> 2));
        CLOCK;

        if ((index + 1) % PANEL_CHUNKSIZE == 0)
        {
            SET_ROW_PINS(index / PANEL_CHUNKSIZE);
            LATCH_DATA;
        }
    }
#endif
}

void Panel::drawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color, bool fill)
{ // draws a rect filled ro not filled with the given color at coords (landscape, origin in upper left corner)
    // get colors
    convertColor(color, &r, &g, &b);

    if (fill)
    {
        for (uint8_t i = x1; i <= x2; i++)
        {
            for (uint8_t j = y1; j <= y2; j++)
            {
                setBuffer(i, j, r, g, b);
            }
        }
    }
    else
    {
        // left line
        for (uint8_t j = y1; j <= y2; j++)
        {
            setBuffer(x1, j, r, g, b);
        }

        // right line
        for (uint8_t j = y1; j <= y2; j++)
        {
            setBuffer(x2, j, r, g, b);
        }

        // top line
        for (uint8_t i = x1; i <= x2; i++)
        {
            setBuffer(i, y1, r, g, b);
        }

        // bottom line
        for (uint8_t i = x1; i <= x2; i++)
        {
            setBuffer(i, y2, r, g, b);
        }
    }
}

void Panel::drawSquare(uint8_t x, uint8_t y, uint8_t size, uint8_t color, bool fill)
{
    drawRect(x, y, x + size, y + size, color, fill);
}

void Panel::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{ // draws a line with color at coords given, must be left to right
    // get colors

    convertColor(color, &r, &g, &b);

    //  f(x) = m*x+t
    //  t = f(0) = y1
    //  m =(y2-y1)/(x2-x1)
    //  ->iterate for each x from x1 to x2
    //

    float dy = y2 + 1 - y1; // delta y
    float m = dy / (x2 + 1 - x1);
    uint8_t temp = 0;
    for (uint8_t x = 0; x <= (x2 - x1); x++)
    {
        uint8_t y = (uint8_t)(m * x + y1) + 0.5f;
        setBuffer(x + x1, y, r, g, b);
    }
}

void Panel::drawCircle(uint8_t x, uint8_t y, uint8_t radius, uint16_t color, bool fill)
{
    // draws a circle at the coords with radius and color
    // get colors
    convertColor(color, &r, &g, &b);
    int xC;
    int yC;

    if (fill)
    {
        // draw outline
        for (uint8_t i = 1; i < 180; i++)
        {
            xC = x + (radius - 0.1) * cos(i * 0.035) + 0.5;
            yC = y + (radius - 0.1) * sin(i * 0.035) + 0.5;
            setBuffer(xC, yC, r, g, b);
        }
        // check if point in circle, then fill
        for (uint8_t i = x - radius; i < x + radius; i++)
        {
            for (uint8_t j = y - radius; j < y + radius; j++)
            {
                if ((x - i) * (x - i) + (y - j) * (y - j) < ((radius - 0.5) * (radius - 0.5)))
                {
                    setBuffer(i, j, r, g, b);
                }
            }
        }
    }
    else
    {
        // draw circle outline
        for (uint8_t i = 1; i < 180; i++)
        {
            xC = x + (radius - 0.1) * cos(i * 0.035) + 0.5;
            yC = y + (radius - 0.1) * sin(i * 0.035) + 0.5;
            setBuffer(xC, yC, r, g, b);
        }
    }
}

void Panel::drawChar(uint8_t x, uint8_t y, char letter, uint16_t color)
{ // deprecated, but probably faster
    // color for the char

    convertColor(color, &r, &g, &b);
    // iterate through the character line by line
    char out;
    for (uint8_t i = 0; i < 5; i++)
    {
        out = getFontLine(letter, i);
        // iterate through the character bit by bit
        for (uint8_t j = 4; j > 0; --j)
        {
            // shift by j and check for bit set
            if (out & (1 << j))
            {
                // set pixel at i and j
                setBuffer(x + 4 - j, y + i, r, g, b);
            }
        }
    }
}

void Panel::drawBigChar(uint8_t x, uint8_t y, char letter, uint16_t color, uint8_t size_modifier)
{ // new with scaling, but may be slower
    // color for the char
    convertColor(color, &r, &g, &b);
    // x = (x>64)*(x-64)+(x<64)*(x);

    // iterate through the character line by line
    char out;
    for (uint8_t i = 0; i < 5 * size_modifier; i++)
    {
        out = getFontLine(letter, i / size_modifier);
        // iterate through the character bit by bit
        for (uint8_t j = 4 * size_modifier; j > 0; --j)
        {
            // shift by j and check for bit set
            if (out & (1 << j / size_modifier))
            {
                // set pixel at i and j
                setBuffer(x + 4 * size_modifier - j, y + i, r, g, b);
            }
        }
    }
}

// todo add drawing methods for
// - triangle with arbitrary points
// - triangle with right angle, rotation and side lengths
// - line with given width