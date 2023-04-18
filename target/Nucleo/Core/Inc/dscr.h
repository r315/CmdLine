#ifndef _dscr_h_
#define _dscr_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void DSCR_Init(void);
void DSCR_StartCapture(uint16_t *dst, uint32_t size, uint32_t duration, void(*cb)(uint32_t ncap));
void DSCR_StopCapture(void);
uint32_t DSCR_WaitCapture(void);

void DSCR_Replay(uint16_t *dst, uint32_t len);
void DSCR_WaitReplay(void);

void DSCR_TimeoutHandler(void);
void DSCR_CaptureHandler(void);
void DSCR_ReplayHandler(void);

#ifdef __cplusplus
}
#endif

#endif //_dscr_h_
