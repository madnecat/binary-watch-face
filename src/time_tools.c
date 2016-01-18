#include "time_tools.h"

#include "dates.h"

//initialisation des dates
void init_dates() {

  init_suffixes();

}

//Procédure de maj de l'heure dans les variables locales
void update_local_time(int *hour, int *minutes, bool display_day, TextLayer *s_date_layer, TextLayer *s_date_suffixe_layer) {

  //initialisation de l'heure
  time_t temp = time(NULL); 
  
  //récupération de la structure contenant l'heure
  struct tm *tick_time = localtime(&temp);
  
  //buffer d'écriture
  static char buffer[] = "00";
  
  //écriture de l'heure actuelle dans le buffer
  strftime(buffer, sizeof(buffer), "%I", tick_time);
  
  //récupération de l'heure sous forme d'entier
  *hour = atoi(buffer);
  
  //écriture des minutes actuelles dans le buffer
  strftime(buffer, sizeof(buffer), "%M", tick_time);
  
  //récupération des minutes sous forme d'entier
  *minutes = atoi(buffer);
  
  //écrit la date dans le buffer, check du format
  strftime(buffer, sizeof(buffer), "%d", tick_time);
  
  if(display_day) {

    // affichage de la date dans le layer
    text_layer_set_text(s_date_layer, buffer);
    text_layer_set_text(s_date_suffixe_layer, date_suffixe[atoi(buffer) - 1]);

  }
}