#ifndef SIGNAL_H
#define SIGNAL_H
#include <stdint.h>
typedef struct {
     
  uint8_t                 IDE:1;       
  union {
    struct {
      uint32_t              StdId:11;         
    };
    struct {
      uint32_t              ExtId:29;         
    };
  };
  union {
    uint8_t                 data8[8];     
    uint16_t                data16[4];     
    uint32_t                data32[2];    
  };
}CANRxFrame;


void canread_init(void);
void can_send_DJI2006(void);
void can_send_VESC(void);
#endif

