/*--------------------------------------------------------------------------------------
 Simple FM6363 4-bit color demo

 Cycles solid colors across the full panel and inserts a black frame between each
 color so every step visibly blinks.
 ------------------------------------------------------------------------------------- */
#if (defined(__STM32F1__) || defined(__STM32F4__))
#include "DMD_SPWM_Driver.h"
#elif (defined(ARDUINO_ARCH_RP2040))
#include "DMD_SPWM_Driver_RP.h"
#endif

// Number of panels in x and y axis
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

// Since the panel itself has a SRAM memory,
// using a library dual buffering is not necessary.
#define ENABLE_DUAL_BUFFER false

/* =================== *
 *     STM32F4 pins    *
 * =================== */
#if (defined(__STM32F1__) || defined(__STM32F4__))
// ==== DMD_RGB pins ====
// mux pins - A, B, C... all mux pins must be selected from same port!
#define DMD_PIN_A PB6
#define DMD_PIN_B PB5
#define DMD_PIN_C PB4
#define DMD_PIN_D PB3
#define DMD_PIN_E PB8
uint8_t mux_list[] = {DMD_PIN_A, DMD_PIN_B, DMD_PIN_C, DMD_PIN_D, DMD_PIN_E};

// pin OE must be one of PB0 PB1 PA6 PA7
#define DMD_PIN_nOE PB0
#define DMD_PIN_SCLK PB7

// CLK, R0, G0, B0, R1, G1, B1
uint8_t custom_rgbpins[] = {PA6, PA0, PA1, PA2, PA3, PA4, PA5};

#elif (defined(ARDUINO_ARCH_RP2040))

// ==== DMD_RGB pins ====
// mux pins - A, B, C... all mux pins must be selected from same port!
#define DMD_PIN_A 6
#define DMD_PIN_B 7
#define DMD_PIN_C 8
#define DMD_PIN_D 9
#define DMD_PIN_E 10
uint8_t mux_list[] = {DMD_PIN_A, DMD_PIN_B, DMD_PIN_C, DMD_PIN_D, DMD_PIN_E};

// pin OE and shift clock
#define DMD_PIN_nOE 15
#define DMD_PIN_SCLK 12

// CLK, R0, G0, B0, R1, G1, B1
uint8_t custom_rgbpins[] = {11, 0, 1, 2, 3, 4, 5};

#endif

DMD_RGB_FM6363<RGB128x64plainS32, COLOR_4BITS> dmd(
    mux_list,
    DMD_PIN_nOE,
    DMD_PIN_SCLK,
    custom_rgbpins,
    DISPLAYS_ACROSS,
    DISPLAYS_DOWN,
    ENABLE_DUAL_BUFFER);

namespace
{

    constexpr uint32_t kColorOnTimeMs = 500;
    constexpr uint32_t kBlackOffTimeMs = 180;

    bool showBlackFrame = false;
    uint8_t colorIndex = 0;
    uint32_t lastFrameChangeMs = 0;

    const uint16_t kDemoColors[] = {
        0,
        dmd.Color888(255, 0, 0),
        dmd.Color888(0, 255, 0),
        dmd.Color888(0, 0, 255),
        dmd.Color888(255, 255, 0),
        dmd.Color888(255, 0, 255),
        dmd.Color888(0, 255, 255),
        dmd.Color888(255, 255, 255),
        dmd.Color888(255, 128, 0),
        dmd.Color888(128, 0, 255),
        dmd.Color888(0, 128, 255),
        dmd.Color888(128, 255, 0),
        dmd.Color888(255, 64, 64),
        dmd.Color888(64, 255, 64),
        dmd.Color888(64, 64, 255),
        dmd.Color888(160, 160, 160),
    };

    constexpr size_t kDemoColorCount = sizeof(kDemoColors) / sizeof(kDemoColors[0]);

    void renderFrame(uint16_t color)
    {
        dmd.fillScreen(color);
        dmd.swapBuffers(true);
    }

} // namespace

void setup(void)
{
    dmd.init();
    renderFrame(kDemoColors[colorIndex]);
    lastFrameChangeMs = millis();
}

void loop(void)
{
    const uint32_t now = millis();
    const uint32_t frameInterval = showBlackFrame ? kBlackOffTimeMs : kColorOnTimeMs;

    if ((now - lastFrameChangeMs) < frameInterval)
    {
        return;
    }

    lastFrameChangeMs = now;

    colorIndex++;
    if (colorIndex >= kDemoColorCount)
    {
        colorIndex = 0;
    }

    renderFrame(kDemoColors[colorIndex]);
}