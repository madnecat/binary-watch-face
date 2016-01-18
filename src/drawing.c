#include "drawing.h"

//configurations
#define _BG_MINUTES 1
#define _BG_HOURS 2
#define _BG_CENTER 3
#define _COL_MINUTES 4
#define _COL_HOURS 5
#define _COL_DAY 6
#define _DISPLAY_DAY 7

//rayon du cercle plein
#define _RADIUS_1 8

//rayon du cercle vide
#define _RADIUS_0 6

//delta d'animation
#define _DELTA_ANIM 100

//initialisation des coordonnées des cercles
void init_pos_circles(GPoint *MN_center_1, GPoint *MN_center_2, GPoint *MN_center_4, GPoint *MN_center_8, GPoint *MN_center_16, GPoint *MN_center_32, GPoint *HR_center_1, GPoint *HR_center_2, GPoint *HR_center_4, GPoint *HR_center_8, int hour_circle_radius, int day_circle_radius, int circle_radius, GPoint GP_center) {
  
  // distance entre le centre et les cercles des minutes
  int distance_minutes_from_center = hour_circle_radius + pbl_floor((circle_radius - hour_circle_radius) / 2);
  
  *MN_center_1 = GPoint(GP_center.x, GP_center.y - distance_minutes_from_center);
  *MN_center_2 = GPoint(GP_center.x + distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y - distance_minutes_from_center*pbl_sin(M_PI/6));
  *MN_center_4 = GPoint(GP_center.x + distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y + distance_minutes_from_center*pbl_sin(M_PI/6));
  *MN_center_8 = GPoint(GP_center.x, GP_center.y + distance_minutes_from_center);
  *MN_center_16 = GPoint(GP_center.x - distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y + distance_minutes_from_center*pbl_sin(M_PI/6));
  *MN_center_32 = GPoint(GP_center.x - distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y - distance_minutes_from_center*pbl_sin(M_PI/6));
  
  //coordonnées des points des heures
  int distance_hours_from_center = day_circle_radius + pbl_floor((hour_circle_radius - day_circle_radius) / 2);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "distance_hours_from_center : (%d)", distance_hours_from_center);
  
  *HR_center_1 = GPoint(GP_center.x, GP_center.y - distance_hours_from_center);
  *HR_center_2 = GPoint(GP_center.x + distance_hours_from_center, GP_center.y);
  *HR_center_4 = GPoint(GP_center.x, GP_center.y + distance_hours_from_center);
  *HR_center_8 = GPoint(GP_center.x - distance_hours_from_center, GP_center.y);
  
}

//procédure d'initialisation du BG
void drawInitialBG(Layer *layer, GContext *ctx, GColor BG_color_H, int hour_circle_radius, GColor BG_color_C, int day_circle_radius) {
  
  //récupération des bounds
  GRect bounds = layer_get_bounds(layer);
  
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "init BG : (%d, %d) : %d", grect_center_point(&bounds).x,grect_center_point(&bounds).y,bounds.size.w);
  
  //set de couleur BG Heures
  graphics_context_set_fill_color(ctx, BG_color_H);
  
  //dessin du cercle des heures
  //graphics_fill_circle(ctx, grect_center_point(&bounds), MN_center_2.x - grect_center_point(&bounds).x - _RADIUS_1 / 4);
  graphics_fill_circle(ctx, grect_center_point(&bounds), hour_circle_radius);
  
  
    //set de couleur BG centre
  graphics_context_set_fill_color(ctx, BG_color_C);
  
  //dessin du cercle central
  //graphics_fill_circle(ctx, grect_center_point(&bounds), HR_center_2.x - grect_center_point(&bounds).x - 2 * _RADIUS_0);
  graphics_fill_circle(ctx, grect_center_point(&bounds), day_circle_radius);
  
}

//procédure de dessin des cercles extérieurs sans animation
void draw_full_circles_no_animate(GContext *ctx, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, GColor MN_color, GColor HR_color) {
  
  //application de la couleur des minutes au dessin
  graphics_context_set_fill_color(ctx, MN_color);
  
  //dessin des points des minutes
  graphics_fill_circle(ctx, MN_center_1, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_2, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_4, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_8, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_16, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_32, _RADIUS_1);
  
  //application de la couleur des minutes au dessin
  graphics_context_set_fill_color(ctx, HR_color);
  
  //dessin des points des heures
  graphics_fill_circle(ctx, HR_center_1, _RADIUS_1);
  graphics_fill_circle(ctx, HR_center_2, _RADIUS_1);
  graphics_fill_circle(ctx, HR_center_4, _RADIUS_1);
  graphics_fill_circle(ctx, HR_center_8, _RADIUS_1);
  
}

//procédure de dessin des cercles en état initial
void draw_full_circles(GContext *ctx, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, GColor MN_color, GColor HR_color, int inc) {
  
  //application de la couleur des minutes au dessin
  graphics_context_set_fill_color(ctx, MN_color);
  
  //dessin des points des minutes
  graphics_fill_circle(ctx, MN_center_1, inc);
  graphics_fill_circle(ctx, MN_center_2, inc);
  graphics_fill_circle(ctx, MN_center_4, inc);
  graphics_fill_circle(ctx, MN_center_8, inc);
  graphics_fill_circle(ctx, MN_center_16, inc);
  graphics_fill_circle(ctx, MN_center_32, inc);
  
  //application de la couleur des minutes au dessin
  graphics_context_set_fill_color(ctx, HR_color);
  
  //dessin des points des heures
  graphics_fill_circle(ctx, HR_center_1, inc);
  graphics_fill_circle(ctx, HR_center_2, inc);
  graphics_fill_circle(ctx, HR_center_4, inc);
  graphics_fill_circle(ctx, HR_center_8, inc);
  
}

//procédure de gestion d'un cercle interne précis
//unit : unité à tester (heure, minute), stud : palier binaire étudié (32 ,16 ,8 , ...)
void handle_internal(GContext *ctx, int *unit, int stud, GPoint center, GColor color, int inc) {
  
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "unité = %d, stud : %d", *unit, stud);
  //étude du cas
  if(*(unit) / stud > 0) {
    
    //maj des minutes restantes à afficher
    *(unit) -= stud; 
    
  } else {
    
    //set la couleur du cercle
    graphics_context_set_fill_color(ctx, color);
    
    //dessin du cercle
    graphics_fill_circle(ctx, center, inc);
    
  }
  
}

//procédure de dessin des cercles internes pour affichage de l'heure
void draw_intern_circles(GContext *ctx, int minutes, int hour, GColor BG_color_M, GColor BG_color_H, GPoint MN_center_1, GPoint MN_center_2, GPoint MN_center_4, GPoint MN_center_8, GPoint MN_center_16, GPoint MN_center_32, GPoint HR_center_1, GPoint HR_center_2, GPoint HR_center_4, GPoint HR_center_8, int inc) {
  
  //application de la couleur de l'arrière plan
  //graphics_context_set_fill_color(ctx, BG_color_M);
  //étude des minutes
  handle_internal(ctx, &minutes, 32, MN_center_32, BG_color_M, inc);
  handle_internal(ctx, &minutes, 16, MN_center_16, BG_color_M, inc);
  handle_internal(ctx, &minutes, 8, MN_center_8, BG_color_M, inc);
  handle_internal(ctx, &minutes, 4, MN_center_4, BG_color_M, inc);
  handle_internal(ctx, &minutes, 2, MN_center_2, BG_color_M, inc);
  handle_internal(ctx, &minutes, 1, MN_center_1, BG_color_M, inc);
  
  //étude des heures
  handle_internal(ctx, &hour, 8, HR_center_8, BG_color_H, inc);
  handle_internal(ctx, &hour, 4, HR_center_4, BG_color_H, inc);
  handle_internal(ctx, &hour, 2, HR_center_2, BG_color_H, inc);
  handle_internal(ctx, &hour, 1, HR_center_1, BG_color_H, inc);
  
}