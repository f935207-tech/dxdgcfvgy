#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    short bfType;
    int   bfSize;
    short bfReserved1;
    short bfReserved2;
    int   bfOffBits;
} BMPHeader;

typedef struct {
    int biSize;
    int biWidth;
    int biHeight;
    short biPlanes;
    short biBitCount;
    int biCompression;
    int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    int   biClrUsed;
    int   biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

void drawLine(unsigned char* pixelData, int width, int height, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
            int bmpY = height - 1 - y1;
            int idx = (bmpY * width + x1) * 3;
            pixelData[idx] = b;
            pixelData[idx + 1] = g;
            pixelData[idx + 2] = r;
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy)  { err += dx; y1 += sy; }
    }
}

int getRowSize(int width) {
    return ((width * 3 + 3) / 4) * 4;
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "rb+");
    BMPHeader header; 
    BMPInfoHeader iheader;
    fread(&header, sizeof(header), 1, file);
    fread(&iheader, sizeof(iheader), 1, file);

    int width = iheader.biWidth;
    int heigt = iheader.biHeight;
    int rowSize = getRowSize(width);
    unsigned char* data = malloc((size_t)rowSize * heigt);
    fseek(file, header.bfOffBits, SEEK_SET);
    fread(data, 1, (size_t)rowSize * heigt, file);

    drawLine(data, width, heigt, 0, 0, width - 1, heigt - 1, 255, 0, 0);
    drawLine(data, width, heigt, 0, heigt - 1, width - 1, 0, 255, 0, 0);

    fseek(file, header.bfOffBits, SEEK_SET);
    fwrite(data, 1, (size_t)rowSize * heigt, file);

    free(data);
    fclose(file);
    return 0;
}