#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef ARDUINO
    #include <Arduino.h>
#endif

//TODO: new 12 bit audio now uses the lower pins
//       figure out new debugging scheme
enum error_type{ERROR_NONE=0,              // xxxx
                ERROR_MARK =  0x1,         // Oxxx
                ERROR_FATAL = 0x2,         // xOxx
                ERROR_CRITICAL = 0x4,      // xxOx 
                ERROR_OVERFLOW = 0x8,      // xxxO
                ERROR_RECEIVE = 0x6,       // xOOx 
                ERROR_ALL=0xf};            // OOOO


#ifdef ARDUINO
#define ERROR_SET(e) \ 
{ \
    switch(e) { \
        case ERROR_MARK: \
            PORTD ^= 0x2; \
            break; \
        default: \
            break; \
    } \
}
#else
#define ERROR_SET(e)
#endif



#endif

