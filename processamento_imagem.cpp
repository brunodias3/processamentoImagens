//This program loads 3-channel BMP
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "load_bmp.h"
using namespace std;

int janela[9][9];
unsigned char *bf; // billateral filtered image


void init(void) { 
    glClearColor(0.0, 0.0, 0.0, 0.0); 
}

void display(void) {
    long width = header.bmpinfo.width;
    long height = header.bmpinfo.height;

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
        exit(0);
    }
}

void zera_janela(){
    for(int a=0; a<9; a++){
        for(int b=0; b<9; b++){
            janela[a][b] = 0;
        }
    }
}
long double dist_euclidiana(int px,int py,int qx,int qy) {
    return sqrt(pow(abs(px-qx),2) + pow(abs(py-qy),2));
}

long double gauss(int sigma, int x) {
    return 1/(2*M_PI*pow(sigma,2))*exp(-(pow(x,2)/2*pow(sigma,2)));
}

long double calcula_wp(bool iq) {
    int p = janela[4][4];
    long double soma = 0;
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(i == 4 && j == 4){
                continue;
            }
            int q = janela[i][j];
            if(iq == true){
                soma += gauss(3, dist_euclidiana(4, 4, i, j))*gauss(8, p - q)*q;
            }
            else{
                soma += gauss(3, dist_euclidiana(4, 4, i, j))*gauss(8, p - q);
            }
        }
    }
    return soma;
}

int main(int argc, char **argv) {

    loadBMP(argv[1]);
    long width = header.bmpinfo.width;
    long height = header.bmpinfo.height;
    int matriz_pixels[height][width];
    int matriz_bilateral[height][width];
    int linha,coluna,cont;
    linha = 0;
    coluna = 0;
    cont = 0;
    for(int i = 0; i < 3*width*height; i += 3) {
        matriz_pixels[linha][coluna] = (int)data[i];
        cont += 1;
        if (cont == width ) {
            linha += 1;
            coluna = 0;
            cont = 0;
        }
        coluna += 1;
    }


    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            // inicializa janela com 0
            zera_janela(); 
            int ijanela = 0, jjanela = 0;
            // percorre a matriz 4 linhas para trás e 4 coluna para trás do pixel a ser processado
            for(int imatriz = i-4; imatriz <= i+4; imatriz++){
                for(int jmatriz = j-4; jmatriz <= j+4; jmatriz++){
                    // se a posiçao estiver fora dos limites da matriz, não modifica a célula da janela (segue 0)
                    if(imatriz < 0 || jmatriz < 0 || imatriz >= height || jmatriz >= width){
                        jjanela++;                        
                        continue;
                    }
                    janela[ijanela][jjanela] = matriz_pixels[imatriz][jmatriz];
                    jjanela++;
                }
                ijanela++;
                jjanela = 0;
            }
            int BIp = (int)round(1/calcula_wp(false) * calcula_wp(true));
            if(BIp < 0){
                BIp = 0;
            }
            else if(BIp > 255){ 
                BIp = 255;
            }
            matriz_bilateral[i][j] = BIp;
        }
    }


    // bf = (unsigned char *)malloc(header.bmpinfo.imagesize);
    // bf = data;
    cont = 0;
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            data[cont] = (unsigned char)matriz_bilateral[i][j];
            data[cont+1] = (unsigned char)matriz_bilateral[i][j];
            data[cont+2] = (unsigned char)matriz_bilateral[i][j];
            cont +=3;
        }
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
