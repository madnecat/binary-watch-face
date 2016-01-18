#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <pebble.h>

//initialisation des dates
void init_dates();

//Procédure de maj de l'heure dans les variables locales
void update_local_time(int *hour, int *minutes, bool display_day, TextLayer *s_date_layer, TextLayer *s_date_suffixe_layer);

#endif