/* Sergey Khashin

Department of Mathematics
Ivanovo State University
Ermaka 39, Ivanovo, Russia
+7(4932)30-02-42
http://math.ivanovo.ac.ru/dalgebra/Khashin/index.html
*/

#ifndef READ_BMP_H__
#define READ_BMP_H__

typedef unsigned short WORD;

typedef struct {
    WORD   bfType;         // 0x4d42 | 0x4349 | 0x5450
    int    bfSize;         // размер файла
    int    bfReserved;     // 0
    int    bfOffBits;      // смещение до поля данных,
                           // обычно 54 = 16 + biSize
    int    biSize;         // размер струкуры в байтах:
                           // 40(BITMAPINFOHEADER) или 108(BITMAPV4HEADER)
                           // или 124(BITMAPV5HEADER)
    int    biWidth;        // ширина в точках
    int    biHeight;       // высота в точках
    WORD   biPlanes;       // всегда должно быть 1
    WORD   biBitCount;     // 0 | 1 | 4 | 8 | 16 | 24 | 32
    int    biCompression;  // BI_RGB | BI_RLE8 | BI_RLE4 |
                           // BI_BITFIELDS | BI_JPEG | BI_PNG
                           // реально используется лишь BI_RGB
    int    biSizeImage;    // Количество байт в поле данных
                           // Обычно устанавливается в 0
    int    biXPelsPerMeter;// горизонтальное разрешение, точек на дюйм
    int    biYPelsPerMeter;// вертикальное разрешение, точек на дюйм
    int    biClrUsed;      // Количество используемых цветов
                           // (если есть таблица цветов)
    int    biClrImportant; // Количество существенных цветов.
                           // Можно считать, просто 0
} BMPheader;

int *loadBMP(const char *fname, int &mx, int &my);

#endif  // readBMP.h
