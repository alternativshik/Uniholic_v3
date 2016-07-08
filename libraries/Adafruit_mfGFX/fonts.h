//
//  fonts.h
//  

#ifndef _fonts_h
#define _fonts_h

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define PROGMEM
#endif

// Font selection descriptors - Add an entry for each new font and number sequentially
#define OPENSANS_10	0
#define OPENSANS_12	1
#define OPENSANS_18	2
#define ROBOTO_48	3

#define FONT_START 0
#define FONT_END 1

struct FontDescriptor
{
	uint8_t	width;		// width in bits
	uint8_t	height; 	// char height in bits
	uint16_t offset;	// offset of char into char array
};

// Font references - add pair of references for each new font
extern const uint8_t openSans_10ptBitmaps[];
extern const FontDescriptor openSans_10ptDescriptors[];

extern const uint8_t openSans_12ptBitmaps[];
extern const FontDescriptor openSans_12ptDescriptors[];

extern const uint8_t openSans_18ptBitmaps[];
extern const FontDescriptor openSans_18ptDescriptors[];

extern const uint8_t robotoCondensed_48ptBitmaps[];
extern const FontDescriptor robotoCondensed_48ptDescriptors[];

#endif
