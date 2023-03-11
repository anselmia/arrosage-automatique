// #include "screen.h"
//
// SCREEN::SCREEN()
//{
//     U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
//     selectedScreen = 0;
//     subScreenSelected = 0;
//     selectedEv = 0;
// }
//
// void SCREEN::init()
//{
// }
//
// void SCREEN::selection()
//{
//     u8g.firstPage(); // Select the first memory page of the scrren
//     do
//     {
//         u8g.setFont(u8g_font_tpss); // Use standard character
//         screen_selection();
//     } while (u8g.nextPage()); // Select the next page
// }