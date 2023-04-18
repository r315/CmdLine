#include "cmdlmcshd.h"
#include "board.h"


#define MATRIX_SIZE 1

void CmdLmcshd::init(void *params) 
{ 
    console = static_cast<Console*>(params);
    BOARD_LCD_Init();
    LCD_SetOrientation(1);
    m_matrix_w = LCD_GetWidth() / MATRIX_SIZE;
    m_matrix_h = LCD_GetHeight() / MATRIX_SIZE;
    m_serial = SERIAL_GetSerial(0);
}

void CmdLmcshd::help(void)
{
    console->putString("usage: lmcshd <option>\n");
    console->putString("LED Matrix Control Software HD");
    console->putString("Options:");
    console->putString("\tstart - start LMCSHD protocol");
}

char CmdLmcshd::execute(int argc, char **argv)
{
    //uint8_t update;
    uint8_t c;
    uint8_t ack = 0x06;
    uint16_t buf[m_matrix_w * 2];
    m_matrix_buffer = (uint8_t*)buf;

    if(xstrcmp("start", (const char*)argv[1]) == 0){
        LCD_Bkl(1);
        do{
            c = console->getChar();
            //update = false;
            switch(c){
                case 0x05:
                    console->print("%d\r\n%d\r\n", m_matrix_w, m_matrix_h);
                    break;

                 case 'A': //24bpp frame data
                    //while (backgroundLayer.isSwapPending());
                    //buffer = backgroundLayer.backBuffer();
                    //for (int i = 0; i < m_matrix_w * m_matrix_h; i++) {
                        //m_serial->readBytes(pix, 3);
                        //*buffer++ = rgb24{map(pix[0], 0, 255, BLK, 255), map(pix[1], 0, 255, BLK, 255), map(pix[2], 0, 255, BLK, 255)};
                    //}
                    //m_serial->write(&ack, 1); //acknowledge
                    //backgroundLayer.swapBuffers(false);
                    break;

                case 'B': //16bpp frame data
                    for(int i = 0; i < m_matrix_h; i++){
                        uint16_t *line = (uint16_t*)m_matrix_buffer + ((i&1) * m_matrix_w);
                        m_serial->readBytes((uint8_t*)line, m_matrix_w * 2);                        
                        LCD_WriteArea(0, i, m_matrix_w, 1, line);
                    }
                    m_serial->write(&ack, 1); //acknowledge
                    //update = true;
                break;

                case 'C': //8bpp frame data                    
                    m_serial->readBytes(m_matrix_buffer,  m_matrix_w * m_matrix_h);
                    //*buffer++ = rgb24{map(((pix[0] & B11100000) >> 5), 0, 7, BLK, 255), map(((pix[0] & B00011100) >> 2), 0, 7, BLK, 255), map(pix[0] & B00000011, 0, 3, BLK, 255)}; //,                    
                    m_serial->write(&ack, 1); //acknowledge
                    break;

                case 'D': //8bpp monochrome frame data
                    m_serial->readBytes(m_matrix_buffer,  m_matrix_w * m_matrix_h);                              
                    m_serial->write(&ack, 1); //acknowledge
                    break;

                case 'E': //1bpp monochrome frame data
                    /*
                    for (int i = 0; i < ((m_matrix_w * m_matrix_h) / 8) + ((m_matrix_w * m_matrix_h) % 8); i++) {
                        m_serial->readBytes(pix, 1);
                        byte b = ((pix[0] & B10000000) >> 7) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B01000000) >> 6) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B00100000) >> 5) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B00010000) >> 4) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B00001000) >> 3) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B00000100) >> 2) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = ((pix[0] & B00000010) >> 1) * 255;
                        *buffer++ = rgb24{b, b, b};
                        b = (pix[0] & B00000001) * 255;
                        *buffer++ = rgb24{b, b, b};
                    }
                    m_serial->write(&ack, 1); //acknowledge
                    */
                    break;
            }
/*
            if(update){
                uint16_t *rgb_buf = (uint16_t*)m_matrix_buffer;
                for(uint16_t i = 0; i < m_matrix_w * m_matrix_h; i++){
                    uint16_t color = (rgb_buf[i] << 8) | (rgb_buf[i] >> 8);
                    LCD_FillRect(
                        (i % m_matrix_w) * MATRIX_SIZE, 
                        (i / m_matrix_w) * MATRIX_SIZE,
                        MATRIX_SIZE, MATRIX_SIZE, color);
                }
                update = false; 
            }*/

        }while(c != 'q');
      
    }else{
        help();
    }

    return CMD_OK;
}