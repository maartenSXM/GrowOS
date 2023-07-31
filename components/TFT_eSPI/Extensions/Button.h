/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this TFT_eSPI library allows the button
// legends to be in any font, allow longer labels and to adjust text positioning
// within button
***************************************************************************************/
#include "global_Defines.h"

class TFT_eSPI_Button : public TFT_eSPI
{

public:
  TFT_eSPI_Button(void);
  // "Classic" initButton() uses centre & size
  void initButton(TFT_eSPI *gfx, int16_t x, int16_t y,
                  uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
                  uint16_t textcolor, char *label, uint8_t textsize);

  // New/alt initButton() uses upper-left corner & size
  void initButtonUL(TFT_eSPI *gfx, int16_t x1, int16_t y1,
                    uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
                    uint16_t textcolor, char *label, uint8_t textsize);

  void initButtonUL(int16_t x1,
                    int16_t y1, uint16_t w, uint16_t h,
                    uint16_t outline, uint16_t fill,
                    uint16_t textcolor, const char *label,
                    uint8_t textsize,
                    const uint8_t bMap[], uint16_t bMap_w, uint16_t bMap_h);
  void initButtonUL(TFT_eSPI *gfx, int16_t x1,
                    int16_t y1, uint16_t w, uint16_t h,
                    uint16_t outline, uint16_t fill,
                    uint16_t textcolor, const char *label,
                    uint8_t textsize,
                    const uint8_t bMap[], uint16_t bMap_w, uint16_t bMap_h);

  // Adjust text datum and x, y deltas
  void setLabelDatum(int16_t x_delta, int16_t y_delta, uint8_t datum = MC_DATUM);
  void setLabel(const char *label);
  void setTextColor(uint16_t c),
      setOutline(uint16_t c),
      setFill(uint16_t c);

  void drawButton(bool inverted = false, String long_name = "");
  bool contains(int16_t x, int16_t y);

  void press(bool p);
  bool isPressed();
  bool justPressed();
  bool justReleased();

private:
  TFT_eSPI *_gfx;
  int16_t _x1, _y1;              // Coordinates of top-left corner of button
  int16_t _xd, _yd;              // Button text datum offsets (wrt centre of button)
  uint16_t _w, _h;               // Width and height of button
  uint8_t _textsize, _textdatum; // Text size multiplier and text datum for button
  uint16_t _outlinecolor, _fillcolor, _textcolor;
  uint16_t _outlinewidth;
  char _label[maxLabelLength]; // Button text is 9 chars maximum unless long_name used
  const uint8_t *_bMap;        // means pointer cannot write to it, "const uint8_t * _bMap;" - would not work since the pointer is constant and immutable but the pointed data is not.
  uint16_t _bMap_w;
  uint16_t _bMap_h;

  bool currstate, laststate; // Button states
};
