#include <pebble.h>
#include "pbl-math.h"

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

//layer de la date
static TextLayer *s_date_layer;

//booléen de choix du type de dessin
bool deDrawing = false;

//booléen de connexion bluetooth
bool bluetoothConnect = true;

//booléen d'affichage de la date
bool displayDay;

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

static void readPersistantData() {
  
  //tampon d'écriture des couleurs
  int tmp;
  
  //application des paramètres
  if(persist_read_int(_BG_MINUTES)) {
    
    tmp = persist_read_int(_BG_MINUTES);
    
    BG_color_M = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_BG_HOURS);
    
    BG_color_H = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_BG_CENTER);
    
    BG_color_C = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_MINUTES);
    
    MN_color = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_HOURS);
    
    HR_color = GColorFromHEX(tmp);
    
    tmp = persist_read_int(_COL_DAY);
    
    TXT_color = GColorFromHEX(tmp);

    displayDay = persist_read_bool(_DISPLAY_DAY);
    
  } else {
    
    //initialisation des couleurs
    BG_color_M = GColorBlack ;
    
    BG_color_H = GColorCobaltBlue;
    
    BG_color_C = GColorBlack;
    
    HR_color = GColorBlack;
    
    MN_color = GColorCobaltBlue;
    
    TXT_color = GColorCobaltBlue;
    
    displayDay = true;
    
  }
  
}

//initialisation des coordonnées des cercles
static void init_pos_circles() {
  
  //instanciation du layer
//  Layer *root_layer = window_get_root_layer(s_main_window);
  
  //coordonnées de la fenêtre principale
//  GRect bounds = layer_get_bounds(root_layer);
  
  // distance entre le centre et les cercles des minutes
  int distance_minutes_from_center = hour_circle_radius + pbl_floor((circle_radius - hour_circle_radius) / 2);
  
  MN_center_1 = GPoint(GP_center.x, GP_center.y - distance_minutes_from_center);
  MN_center_2 = GPoint(GP_center.x + distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y - distance_minutes_from_center*pbl_sin(M_PI/6));
  MN_center_4 = GPoint(GP_center.x + distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y + distance_minutes_from_center*pbl_sin(M_PI/6));
  MN_center_8 = GPoint(GP_center.x, GP_center.y + distance_minutes_from_center);
  MN_center_16 = GPoint(GP_center.x - distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y + distance_minutes_from_center*pbl_sin(M_PI/6));
  MN_center_32 = GPoint(GP_center.x - distance_minutes_from_center*pbl_cos(M_PI/6), GP_center.y - distance_minutes_from_center*pbl_sin(M_PI/6));
  
  //coordonnées des points des heures
  int distance_hours_from_center = day_circle_radius + pbl_floor((hour_circle_radius - day_circle_radius) / 2);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "distance_hours_from_center : (%d)", distance_hours_from_center);
  
  HR_center_1 = GPoint(GP_center.x, GP_center.y - distance_hours_from_center);
  HR_center_2 = GPoint(GP_center.x + distance_hours_from_center, GP_center.y);
  HR_center_4 = GPoint(GP_center.x, GP_center.y + distance_hours_from_center);
  HR_center_8 = GPoint(GP_center.x - distance_hours_from_center, GP_center.y);
  
  //coordonnées des points des minutes
  //MN_center_1 = GPoint(bounds.size.w / 2, bounds.size.h / 15);
  //MN_center_2 = GPoint((bounds.size.w / 9) * 8, bounds.size.h / 3.25);
  //MN_center_4 = GPoint((bounds.size.w / 9) * 8, (bounds.size.h / 3.25) * 2.25);
  //MN_center_8 = GPoint(bounds.size.w / 2, bounds.size.h - bounds.size.h / 15);
  //MN_center_16 = GPoint(bounds.size.w / 9, (bounds.size.h / 3.25) * 2.25);
  //MN_center_32 = GPoint(bounds.size.w / 9, bounds.size.h / 3.25);
  
  //coordonnées des points des heures
  //HR_center_1 = GPoint(bounds.size.w / 2, bounds.size.h / 4.5);
  //HR_center_2 = GPoint(bounds.size.w - bounds.size.w / 4.5, bounds.size.h / 2);
  //HR_center_4 = GPoint(bounds.size.w / 2, bounds.size.h - bounds.size.h / 4.5);
  //HR_center_8 = GPoint(bounds.size.w / 4.5, bounds.size.h / 2);
  
  //récupération de version de la montre
  //int version = watch_info_get_model();
    
  //si pebble time [steel]
  //if(version == 3 || version == 4 ) {
    //case WATCH_INFO_MODEL_PEBBLE_TIME || WATCH_INFO_MODEL_PEBBLE_TIME_STEEL :

    //dcallage des centres
   // APP_LOG(APP_LOG_LEVEL_DEBUG, "PT");
  //  HR_center_1.y += 5;
  //  HR_center_4.y -= 5;
  //  MN_center_1.y += 5;
  //  MN_center_8.y -= 5;

  //}
  
}

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



//Procédure de maj de l'heure dans les variables locales
static void update_local_time() {

  //initialisation de l'heure
  time_t temp = time(NULL); 
  
  //récupération de la structure contenant l'heure
  struct tm *tick_time = localtime(&temp);
  
  //buffer d'écriture
  static char buffer[] = "00";
  
  //écriture de l'heure actuelle dans le buffer
  strftime(buffer, sizeof(buffer), "%I", tick_time);
  
  //récupération de l'heure sous forme d'entier
  hour = atoi(buffer);
  
  //écriture des minutes actuelles dans le buffer
  strftime(buffer, sizeof(buffer), "%M", tick_time);
  
  //récupération des minutes sous forme d'entier
  minutes = atoi(buffer);
  
  //écrit la date dans le buffer, check du format
  strftime(buffer, sizeof(buffer), "%d", tick_time);
  
  if(displayDay)
    // affichage de la date dans le layer
    text_layer_set_text(s_date_layer, buffer);
  
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
    
  //condition de rebouclage
  } else if(inc < _RADIUS_1) {

    //incrément
    inc ++;

    //bouclage
    layer_mark_dirty(s_global_layer);

    //rebouclage
    app_timer_register(_DELTA_ANIM, drawingHandler, NULL);

  }
    
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

// ***********
// * Dessins *
// ***********

//procédure d'initialisation du BG
static void drawInitialBG(Layer *layer, GContext *ctx) {
  
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

//procédure de dessin des cercles en état initial
static void draw_full_circles(GContext *ctx) {
  
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
static void handle_internal(GContext *ctx, int *unit, int stud, GPoint center, GColor color) {
  
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "unité = %d, stud : %d", *unit, stud);
  
  //étude du cas
  if(*(unit) / stud > 0) {
    
    //maj des minutes restantes à afficher
    *(unit) -= stud; 
    
  } else {
    
    //set la couleur du cercle
    graphics_context_set_fill_color(ctx, color);
    
    //dessin du cercle
    graphics_fill_circle(ctx, center, _RADIUS_0);
    
  }
  
}

//procédure de dessin des cercles internes pour affichage de l'heure
static void draw_intern_circles(GContext *ctx) {
  
  //application de la couleur de l'arrière plan
  graphics_context_set_fill_color(ctx, BG_color_M);
  
  //étude des minutes
  handle_internal(ctx, &minutes, 32, MN_center_32, BG_color_M);
  handle_internal(ctx, &minutes, 16, MN_center_16, BG_color_M);
  handle_internal(ctx, &minutes, 8, MN_center_8, BG_color_M);
  handle_internal(ctx, &minutes, 4, MN_center_4, BG_color_M);
  handle_internal(ctx, &minutes, 2, MN_center_2, BG_color_M);
  handle_internal(ctx, &minutes, 1, MN_center_1, BG_color_M);
  
  //étude des heures
  handle_internal(ctx, &hour, 8, HR_center_8, BG_color_H);
  handle_internal(ctx, &hour, 4, HR_center_4, BG_color_H);
  handle_internal(ctx, &hour, 2, HR_center_2, BG_color_H);
  handle_internal(ctx, &hour, 1, HR_center_1, BG_color_H);
  
}

// ******************************
// * Mises à jour contextuelles *
// ******************************

//fonction de mise à jour du layer de dessin
void global_layer_update_proc(Layer *layer, GContext *ctx) {
    
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "test, H : %d, M : %d", hour, minutes);
  
  if(hour == 0 && minutes == 0) {
    
    update_local_time();
    
  }
  
  //dessin du BG
  drawInitialBG(layer, ctx);
    
  //en premier, dessin des cercles pleins
  draw_full_circles(ctx);
  
  //condition de dessin des cercles internes
  if(inc >= _RADIUS_1 && !deDrawing) {
    
    //dessin des cercles intérieurs pour affichage de l'heure
    draw_intern_circles(ctx);

  }  
  
}

//hook appelé lors du changement de temps
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  //si dessin normal
  if (inc != -10) {
  
    //dédraw des cercles
    app_timer_register(_DELTA_ANIM, deDrawingHandler, NULL);  
  
  } else {
    
    //sinon, premier dessin donc juste draw
    inc = 1;
    
  }
      
  //Maj de l'heure dans les variables locales
  update_local_time();
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
    
    update_local_time();
    
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
static void main_window_load(Window *window) {
  
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
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "circle_radius : (%d)\n hour_circle_radius : (%d)\nday_circle_radius : (%d)", circle_radius, hour_circle_radius, day_circle_radius);
  
  //création du layer de dessin
  s_global_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_global_layer, global_layer_update_proc);
  layer_add_child(root_layer, s_global_layer);
  
  //création du layer de date
  s_date_layer = text_layer_create(GRect(grect_center_point(&bounds).x - 20, grect_center_point(&bounds).y - 10, 40, 20));
  text_layer_set_background_color(s_date_layer, BG_color_C);
  text_layer_set_text_color(s_date_layer, TXT_color);
  
  // Create GFont
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_EIGHT_BIT_20 ));
  
  // Apply to TextLayer
  text_layer_set_font(s_date_layer, s_date_font);
  
  // Improve the layout to be more like a watchface
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(root_layer, text_layer_get_layer(s_date_layer));
  
}

//fonction de destruction de la fenêtre
static void main_window_unload(Window *window) {

  //désallocation des layers
  layer_destroy(s_global_layer);
  text_layer_destroy(s_date_layer);
  
}

// *****************************************
// * Constructeur/destructeur du programme *
// *****************************************

//fonction d'initialisation du contexte
void init(void) {
  
  //lecture des données persistantes
  readPersistantData();
  
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
  init_pos_circles();
  
  //initialisation de l'heure dans les variables locales
  update_local_time();
  
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
    
  //attente d'un événement système
  app_event_loop();
    
  //appel à la destruction des éléments dynamiques
  deinit();
}