/* Sergey Khashin
  
   Department of Mathematics 
   Ivanovo State University 
   Ermaka 39, Ivanovo, Russia
   +7(4932)30-02-42
   http://math.ivanovo.ac.ru/dalgebra/Khashin/index.html
   */

#include <cstdio>

#include "readBMP.h"

int *loadBMP(const char *fname, int &mx, int &my)
{
    mx = my = -1;
    FILE *f = fopen(fname, "rb");
    if (!f) return NULL;
    BMPheader bh;    // File header sizeof(BMPheader) = 56

    // читаем заголовок
    fread(&bh.bfType, 1, 2, f);
    fread(&bh.bfSize, 1, 4, f);
    fread(&bh.bfReserved, 1, 4, f);
    fread(&bh.bfOffBits, 1, 4, f);
    fread(&bh.biSize, 1, 4, f);
    fread(&bh.biWidth, 1, 4, f);
    fread(&bh.biHeight, 1, 4, f);
    fread(&bh.biPlanes, 1, 2, f);
    fread(&bh.biBitCount, 1, 2, f);
    fread(&bh.biCompression, 1, 4, f);
    fread(&bh.biSizeImage, 1, 4, f);
    fread(&bh.biXPelsPerMeter, 1, 4, f);
    fread(&bh.biYPelsPerMeter, 1, 4, f);
    fread(&bh.biClrUsed, 1, 4, f);
    fread(&bh.biClrImportant, 1, 4, f);
    if (!f) { fclose(f); return NULL; }

    // проверяем сигнатуру
    if (bh.bfType != 0x4d42 && bh.bfType != 0x4349 && bh.bfType != 0x5450) { fclose(f); return NULL; }

    // проверка размера файла
    fseek(f, 0, SEEK_END);
    int filesize = ftell(f);
    // восстановим указатель в файле:
    fseek(f, 54, SEEK_SET);
    // проверим условия
    if (bh.bfSize != filesize ||
        bh.bfReserved != 0 ||
        bh.biPlanes != 1 ||
        (bh.biSize != 40 && bh.biSize != 108 && bh.biSize != 124) ||
        bh.bfOffBits != 14 + bh.biSize ||

        bh.biWidth <1 || bh.biWidth >10000 ||
        bh.biHeight<1 || bh.biHeight>10000 ||
        bh.biBitCount != 24 ||             // пока рассматриваем только полноцветные изображения
        bh.biCompression != 0                // пока рассматриваем только несжатие изображения
        )
    {
        fclose(f);
        return NULL;
    }
    // Заголовок прочитан и проверен, тип - верный (BGR-24), размеры (mx,my) найдены
    mx = bh.biWidth;
    my = bh.biHeight;
    int mx3 = (3 * mx + 3) & (-4);    // Compute row width in file, including padding to 4-byte boundary
    unsigned char *tmp_buf = new unsigned  char[mx3*my];    // читаем данные
    size_t res = fread(tmp_buf, 1, mx3*my, f);
    if ((int)res != mx3*my) { delete[]tmp_buf; fclose(f); return NULL; }
    // данные прочитаны
    fclose(f);

    // выделим память для результата
    int *v = new int[mx*my];

    // Перенос данных (не забудем про BGR->RGB)
    unsigned char *ptr = (unsigned char *)v;
    for (int y = 0; y < my; y++) {
        unsigned char *pRow = tmp_buf + mx3*y;
        for (int x = 0; x< mx; x++) {
            *ptr++ = *(pRow + 2);
            *ptr++ = *(pRow + 1);
            *ptr++ = *pRow;
            pRow += 3;
            ptr++;
        }
    }
    delete[]tmp_buf;
    return v;    // OK
}
