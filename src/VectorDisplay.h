#ifndef _VECTOR_DISPLAY_H
#define _VECTOR_DISPLAY_H

#include <Arduino.h>

#define VECTOR_DISPLAY_MAX_STRING 256

#define ALIGN_LEFT 'l'
#define ALIGN_RIGHT 'r'
#define ALIGN_CENTER 'c'
#define ALIGN_TOP 't'
#define ALIGN_BOTTOM 'b'
#define ALIGN_BASELINE 'l'

template <class MyWriter>
class VectorDisplay {
private:
    union {
        uint32_t color;
        uint16_t twoByte[4];
        struct {
            uint16_t x;
            uint16_t y;
            char text[VECTOR_DISPLAY_MAX_STRING];
        } __attribute__((packed)) xyText;
        struct {
            uint16_t width;
            uint16_t height;
        } __attribute__((packed)) coords;
        struct {
            char attr;
            uint8_t value;
        } __attribute__((packed)) attribute8;
        struct {
            char attr;
            uint16_t value;
        } __attribute__((packed)) attribute16;
        char text[VECTOR_DISPLAY_MAX_STRING];
    } args;
    
    MyWriter writer;
public:    

    VectorDisplay(MyWriter _writer) {
        writer = _writer;
    }

    void sendCommand(char c, const void* arguments, int argumentsLength) {
        writer.write(c);
        writer.write(c^0xFF);
        if (argumentsLength > 0) 
            writer.write((uint8_t*)arguments, argumentsLength);
        uint8_t sum = 0;
        for (int i = 0; i<argumentsLength; i++)
        sum += ((uint8_t*)arguments)[i];
        writer.write(sum^0xFF);
    }

    void line(int x1, int y1, int x2, int y2) {
        args.twoByte[0] = x1;
        args.twoByte[1] = y1;
        args.twoByte[2] = x2;
        args.twoByte[3] = y2;
        sendCommand('L', &args, 8);
    }
    
    void rectangle(int x1, int y1, int x2, int y2) {
        args.twoByte[0] = x1;
        args.twoByte[1] = y1;
        args.twoByte[2] = x2;
        args.twoByte[3] = y2;
        sendCommand('R', &args, 8);
    }
    
    void point(int x, int y) {
        args.twoByte[0] = x;
        args.twoByte[1] = y;
        sendCommand('P', &args, 2);
    }
    
    void text(int x, int y, const char* str) {
        args.xyText.x = x;
        args.xyText.y = y;
        strncpy(args.xyText.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.xyText.text[VECTOR_DISPLAY_MAX_STRING-1] = 0;
        sendCommand('T', &args, 4+strlen(args.xyText.text)+1);
    }
    
    void text(int x, int y, String str) {
        text(x,y,str.c_str());
    }
    
    void message(const char* str) {
        strncpy(args.text, str, VECTOR_DISPLAY_MAX_STRING);
        args.text[VECTOR_DISPLAY_MAX_STRING-1] = 0;
        sendCommand('T', &args, 4+strlen(args.text)+1);
    }
    
    void message(String text) {
        message(text.c_str());
    }
    
    void foreColor(uint32_t color) {
        args.color = color;
        sendCommand('F', &args, 4);
    }

    void backColor(uint32_t color) {
        args.color = color;
        sendCommand('B', &args, 4);
    }

    void clear() {
        sendCommand('C', NULL, 0);
    }

    void reset() {
        sendCommand('E', NULL, 0);
    }

    void coordinates(int width, int height) {
        args.coords.width = width;
        args.coords.height = height;
        sendCommand('Z', &args, 4);
    }
    
    void textHorizontalAlign(char hAlign) {
        args.attribute8.attr = 'h';
        args.attribute8.value = hAlign;
        sendCommand('Y', &args, 2);
    }

    void textVerticalAlign(char hAlign) {
        args.attribute8.attr = 'v';
        args.attribute8.value = hAlign;
        sendCommand('Y', &args, 2);
    }

    void textOpaqueBackground(bool opaque) {
        args.attribute8.attr = 'o';
        args.attribute8.value = opaque ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
    
    void textBold(bool bold) {
        args.attribute8.attr = 'b';
        args.attribute8.value = bold ? 1 : 0;
        sendCommand('Y', &args, 2);
    }
};

//extern VectorDisplay<decltype(Serial)> SerialDisplay;

#endif
