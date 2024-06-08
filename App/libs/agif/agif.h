#ifndef __AGIF__
#define __AGIF__

#ifdef __cplusplus
extern "C" {
#endif

int GIF_Init(GIFIMAGE *pGIF);
int GIF_OpenRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
int GIF_GetInfo(GIFIMAGE *pPage, GIFINFO *pInfo);
int GIF_ParseInfo(GIFIMAGE *pPage, int bInfoOnly);
int GIF_PlayFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser);
int GIF_GetLastError(GIFIMAGE *pGIF);
int GIF_GetComment(GIFIMAGE *pGIF, char *pDest);
int GIF_GetCanvasHeight(GIFIMAGE *pGIF);
int GIF_GetCanvasWidth(GIFIMAGE *pGIF);
int DecodeLZW(GIFIMAGE *pImage, int iOptions);

#if defined( __LINUX__ )
int GIF_OpenFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw);
void GIF_Close(GIFIMAGE *pGIF);
#endif

#ifdef __cplusplus
}
#endif

#endif