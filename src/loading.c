#include "loading.h"

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

void readPersistantData(GColor *bg_m, GColor *bg_h, GColor *bg_c, GColor *color_m, GColor *color_h, GColor *color_c, bool *display_day) {
  
  //tampon d'écriture des couleurs
  int tmp;
  
  //application des paramètres
  if(persist_read_int(_BG_MINUTES)) {
    
    tmp = persist_read_int(_BG_MINUTES);
    
    *bg_m = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_BG_HOURS);
    
    *bg_h = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_BG_CENTER);
    
    *bg_c = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_MINUTES);
    
    *color_m = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_HOURS);
    
    *color_h = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_DAY);
    
    *color_c = GColorFromHEX(tmp);

    *display_day = persist_read_bool(_DISPLAY_DAY);
    
  } else {
    
    //initialisation des couleurs
    *bg_m = GColorVividCerulean ;
    
    *bg_h = GColorYellow;
    
    *bg_c = GColorPurple;
    
    *color_h = GColorPurple;
    
    *color_m = GColorPurple;
    
    *color_c = GColorYellow;
    
    *display_day = true;
    
  }
  
}