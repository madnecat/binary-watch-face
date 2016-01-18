#include "pbl-math.h"
#include "loading.h"
#include "drawing.h"
#include "time_tools.h"

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

//screen size : 144 * 168

// ****************
// *              *
// * Déclarations *
// *              *
// ****************

//fenêtre principale
static Window *s_main_window;

//layer global
static Layer *s_global_layer;

//couleurs de la watchface
GColor BG_color_M; //minutes
GColor BG_color_H; //heures
GColor BG_color_C; //Centre
GColor MN_color;
GColor HR_color;
GColor TXT_color;

//conteneurs de l'heure actuelle
int hour, minutes;

//incrément de dessin d'animation !! valeur de base -10 pour détection du premier dessin
int inc = -10;

/*
 * rayon de la watchface (en fonction de la taille de l'écran) (outer circle)
 * défini dans main_window_load()
 * si hauteur < largeur, alors hauteur, sinon largeur
 */
int circle_radius = -1;

// rayon du cercle des heures (outer circle)
int hour_circle_radius = -1;

// rayon du cercle de la date (inner circle)
int day_circle_radius = -1;

//variable de police du jour
static GFont s_date_font;
static GFont s_date_suffixe_font;

//layer de la date
static TextLayer *s_date_layer;
static TextLayer *s_date_suffixe_layer;

//booléen de choix du type de dessin
bool deDrawing = false;

//booléen de connexion bluetooth
bool bluetoothConnect = true;

//booléen d'affichage de la date
bool displayDay;

//booléen pour détection de l'initialisation
bool binit = true;
bool externInitialised = false;

//centres des différents cercles
GPoint GP_center;
GPoint MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32;
GPoint HR_center_1, HR_center_2, HR_center_4, HR_center_8, HR_center_16;

// **************
// *            *
// * Procédures *
// *            *
// **************

// *******************
// * Initialisations *
// *******************

// ***********
// * Updates *
// ***********

//fonction lancée lorsque perte du signal bluetooth
static void bluetooth_callback(bool connected) {
  
  //si bluetooth perdu
  if(!connected) {
    
    // double vibration de la montre
    vibes_double_pulse();
   
  //si bluetooth retrouvé
  } else if (connected) {
    
    //courte vibration de la montre
    vibes_short_pulse();
    
  }
}

// **************
// * Animations *
// **************

//Gestion du rebouclage du dessin pour grossissement des cercles
static void drawingHandler() {

  //si le dedrawing est encore en cours
  if(deDrawing) {
    
    //on reboucle en attendant
    app_timer_register(_DELTA_ANIM, drawingHandler, NULL);
    
  //cas particulier, initialisation
  } else if(binit && (inc < _RADIUS_0 || !externInitialised)) {
    //incrément
    inc ++;

    //bouclage
    layer_mark_dirty(s_global_layer);

    //rebouclage
    app_timer_register(_DELTA_ANIM, drawingHandler, NULL);

  //condition de rebouclage
  } else if(inc < _RADIUS_0) {

    if(inc < 0)
      //Maj de l'heure dans les variables locales
      update_local_time(&hour, &minutes, displayDay, s_date_layer, s_date_suffixe_layer);
    
    //incrément
    inc ++;

    //bouclage
    layer_mark_dirty(s_global_layer);

    //rebouclage
    app_timer_register(_DELTA_ANIM, drawingHandler, NULL);

    //si on a fini la séquence d'initialisation
  } else if(inc >= _RADIUS_0 && binit)
    //désactivation de l'init
    binit = false;
    
}

//gestion du rétrécissement des cercles
static void deDrawingHandler() {

  //passage du booléen de dedraw à true
  deDrawing = true;
  
  //condition de bouclage
  if (inc >= 0) {
    
     //incrément
    inc --;
    //bouclage
    layer_mark_dirty(s_global_layer);

    //rebouclage
    app_timer_register(_DELTA_ANIM, deDrawingHandler, NULL);
    
  } else {
    
    //si inc = 0 on replace à false le booléen de dedraw
    deDrawing = false;
    
  }
  
}

// ******************************
// * Mises à jour contextuelles *
// ******************************

//fonction de mise à jour du layer de dessin
void global_layer_update_proc(Layer *layer, GContext *ctx) {
    
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "test, H : %d, M : %d, inc : %d", hour, minutes, inc);
  
  //if(hour == 0 && minutes == 0) {
    
  //  update_local_time(&hour, &minutes, displayDay, s_date_layer, s_date_suffixe_layer);
    
  //}
  
  //dessin du BG
  drawInitialBG(layer, ctx, BG_color_H, hour_circle_radius, BG_color_C, day_circle_radius);
  
  //si initialisation
  if (binit) {
    
    //si les cercles extérieurs ne sont pas encore dessinés
    if(!externInitialised && inc < _RADIUS_1) {

      //en premier, dessin des cercles pleins
      draw_full_circles(ctx, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, MN_color, HR_color, inc);

    //sinon cercles extérieurs dessinés
    } else if (!externInitialised && inc >= _RADIUS_1 && binit) {
      //passage du booléen à true et raz inc
      externInitialised = true;

      inc = 0;

      //dessin non animé des extérieurs
      draw_full_circles_no_animate(ctx, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, MN_color, HR_color);

      //dessin animé des cercles intérieurs
      draw_intern_circles(ctx, minutes, hour, BG_color_M, BG_color_H, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, inc);

    //sinon animation dessin animé des cercles intérieurs  
    } else {
      //dessin non animé des extérieurs
      draw_full_circles_no_animate(ctx, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, MN_color, HR_color);

      //dessin animé des cercles intérieurs
      draw_intern_circles(ctx, minutes, hour, BG_color_M, BG_color_H, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, inc);

    }

  //si pas initialisation
  } else {

    //dessin non animé des extérieurs
      draw_full_circles_no_animate(ctx, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, MN_color, HR_color);

      //dessin animé des cercles intérieurs
      draw_intern_circles(ctx, minutes, hour, BG_color_M, BG_color_H, MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32, HR_center_1, HR_center_2, HR_center_4, HR_center_8, inc);

  }

}

//hook appelé lors du changement de temps
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  //si dessin normal
  if (!binit) {
    
    //dédraw des cercles
    app_timer_register(_DELTA_ANIM, deDrawingHandler, NULL);  
  
  } else {
    
    //Maj de l'heure dans les variables locales
    update_local_time(&hour, &minutes, displayDay, s_date_layer, s_date_suffixe_layer);
    //sinon, premier dessin donc juste draw
    inc = 1;
    
  }
      
  //Maj de l'heure dans les variables locales
  //update_local_time();
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "H : %d, M : %d", hour, minutes);

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "passage");

  //dessin
  app_timer_register(_DELTA_ANIM, drawingHandler, NULL);
  
}

// ********************************
// * Récupération des messages JS *
// ********************************

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  //récupération des valeurs dans le dictionnaire
  Tuple *bg_minutes = dict_find(iter, _BG_MINUTES);  
  Tuple *bg_hours = dict_find(iter, _BG_HOURS);  
  Tuple *bg_center = dict_find(iter, _BG_CENTER);  
  Tuple *col_minutes = dict_find(iter, _COL_MINUTES);  
  Tuple *col_hours = dict_find(iter, _COL_HOURS);  
  Tuple *col_day = dict_find(iter, _COL_DAY);  
  Tuple *display_day = dict_find(iter, _DISPLAY_DAY);  
  
  //si valeurs existantes
  if(bg_minutes) {
    
    //variable de recueil des infos
    int tmp;
    
    //gestion du bg des minutes
    tmp = bg_minutes->value->int32;
    
    persist_write_int(_BG_MINUTES, tmp);
    
    BG_color_M = GColorFromHEX(tmp);
    
    //gestion du bg des heures
    tmp = bg_hours->value->int32;
    
    persist_write_int(_BG_HOURS, tmp);
    
    BG_color_H = GColorFromHEX(tmp);
    
    //gestion du bg du centre
    tmp = bg_center->value->int32;
    
    persist_write_int(_BG_CENTER, tmp);
    
    BG_color_C = GColorFromHEX(tmp);
    
    //gestion de la couelur des minutes
    tmp = col_minutes->value->int32;
    
    persist_write_int(_COL_MINUTES, tmp);
    
    MN_color = GColorFromHEX(tmp);
    
    //gestion de la couleur des heures
    tmp = col_hours->value->int32;
    
    persist_write_int(_COL_HOURS, tmp);
    
    HR_color = GColorFromHEX(tmp);
    
    //gestion de la couleur du jour
    tmp = col_day->value->int32;
    
    persist_write_int(_COL_DAY, tmp);
    
    TXT_color = GColorFromHEX(tmp);
    
    //gestion de l'affichage du jour
    displayDay = (display_day->value->int8 != 0);
    
    persist_write_bool(_DISPLAY_DAY, displayDay);
    
    //application des paramètres
    window_set_background_color(s_main_window, BG_color_M); 
    
    text_layer_set_background_color(s_date_layer, BG_color_C);
    
    text_layer_set_text_color(s_date_layer, TXT_color);
    
    update_local_time(&hour, &minutes, displayDay, s_date_layer, s_date_suffixe_layer);
    
    layer_mark_dirty(s_global_layer);
    
    if(!displayDay) {
      
      text_layer_set_text(s_date_layer, "");
      
    }
    
  }
  
}

// ******************************************
// * Constructeur/destructeur de la fenêtre *
// ******************************************

//fonction de construction de la fenêtre
void main_window_load(Window *window) {
  
  //applique la couleur d'arrière plan
  window_set_background_color(window, BG_color_M); 
  
  //instanciation du layer
  Layer *root_layer = window_get_root_layer(s_main_window);
  
  //coordonnées de la fenêtre principale
  GRect bounds = layer_get_bounds(root_layer);
  
  //calcul du centre
  GP_center = grect_center_point(&bounds);
  
  // initilisation du rayon de la watchface
  // normalement h < w mais dans le cas d'une pebble rect, ça permet d'optimiser l'espace (si bug : inverser > par <)
  if (bounds.size.h > bounds.size.w){
    circle_radius = bounds.size.h / 2;
  } else {
    circle_radius = bounds.size.w / 2;
  }
    
  // initialisation du cercle intérieur des heures
  hour_circle_radius = 2 * pbl_floor(circle_radius/3);
  
  // initialisation du cercle intérieur de la date
  day_circle_radius = pbl_floor(circle_radius/3);
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "circle_radius : (%d)\n hour_circle_radius : (%d)\nday_circle_radius : (%d)", circle_radius, hour_circle_radius, day_circle_radius);
  
  //création du layer de dessin
  s_global_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_global_layer, global_layer_update_proc);
  layer_add_child(root_layer, s_global_layer);
  
  //création du layer de date
  s_date_layer = text_layer_create(GRect(grect_center_point(&bounds).x - 25, grect_center_point(&bounds).y - 10, 34, 17));
  text_layer_set_background_color(s_date_layer, BG_color_C);
  text_layer_set_text_color(s_date_layer, TXT_color);

  //création du layer du suffixe de la date
  s_date_suffixe_layer = text_layer_create(GRect(grect_center_point(&bounds).x + 11, grect_center_point(&bounds).y - 10, 14, 7));
  text_layer_set_background_color(s_date_suffixe_layer, BG_color_C);
  text_layer_set_text_color(s_date_suffixe_layer, TXT_color);
  
  // Create GFont
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_EIGHT_BIT_17 ));
  s_date_suffixe_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_EIGHT_BIT_7 ));

  // Apply to TextLayer
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_date_suffixe_layer, s_date_suffixe_font);

  // Improve the layout to be more like a watchface
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_date_suffixe_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(root_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(root_layer, text_layer_get_layer(s_date_suffixe_layer));

}

//fonction de destruction de la fenêtre
void main_window_unload(Window *window) {

  //désallocation des layers
  layer_destroy(s_global_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_date_suffixe_layer);
  
}

// *****************************************
// * Constructeur/destructeur du programme *
// *****************************************

//fonction d'initialisation du contexte
void init(void) {
  
  //lecture des données persistantes
  readPersistantData(&BG_color_M, &BG_color_H, &BG_color_C, &MN_color, &HR_color, &TXT_color, &displayDay);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //premier dessin
  //drawindHandler();
  
  //initialisation de la position des cercles
  init_pos_circles(&MN_center_1, &MN_center_2, &MN_center_4, &MN_center_8, &MN_center_16, &MN_center_32, &HR_center_1, &HR_center_2, &HR_center_4, &HR_center_8, hour_circle_radius, day_circle_radius, circle_radius, GP_center);
  
  //initialisation de l'heure dans les variables locales
  update_local_time(&hour, &minutes, displayDay, s_date_layer, s_date_suffixe_layer);
  
  // Inscription du callback au service bluetooth
  bluetooth_connection_service_subscribe(bluetooth_callback);
  
  //enregistrement aux messages de configuration
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
}

//fonction de destruction du contexte
void deinit(void) {
  
  // Destroy Window
  window_destroy(s_main_window);
  
}

// ************************
// * Procédure Principale *
// ************************

//fonction principale
int main(void) {
  
  //appel de l'initialisation du programme
  init();
  
  //initialisation des dates
  init_dates();

  //attente d'un événement système
  app_event_loop();
    
  //appel à la destruction des éléments dynamiques
  deinit();
}