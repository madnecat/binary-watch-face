#ifndef DRAWING_H
#define DRAWING_H

#include <pebble.h>

#include "pbl-math.h"

//initialisation des coordonnées des cercles
void init_pos_circles(GPoint *MN_center_1, GPoint *MN_center_2, GPoint *MN_center_4, GPoint *MN_center_8, GPoint *MN_center_16, GPoint *MN_center_32, GPoint *HR_center_1, GPoint *HR_center_2, GPoint *HR_center_4, GPoint *HR_center_8, int hour_circle_radius, int day_circle_radius, int circle_radius, GPoint GP_center);

//procédure d'initialisation du BG
void drawInitialBG(Layer *layer, GContext *ctx, GColor BG_color_H, int hour_circle_radius, GColor BG_color_C, int day_circle_radius);

//procédure de dessin des cercles extérieurs sans animation
void draw_full_circles_no_animate(GContext *ctx, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, GColor MN_color, GColor HR_color);

//procédure de dessin des cercles en état initial
void draw_full_circles(GContext *ctx, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, GColor MN_color, GColor HR_color, int inc);

//procédure de gestion d'un cercle interne précis
//unit : unité à tester (heure, minute), stud : palier binaire étudié (32 ,16 ,8 , ...)
void handle_internal(GContext *ctx, int *unit, int stud, GPoint center, GColor color, int inc);

//procédure de dessin des cercles internes pour affichage de l'heure
void draw_intern_circles(GContext *ctx, int minutes, int hour, GColor BG_color_M, GColor BG_color_H, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, int inc);

#endif