#include <pebble.h>

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
GColor BG_color;
GColor MN_color;
GColor HR_color;

//conteneurs de l'heure actuelle
int hour, minutes;

//incrément de dessin d'animation !! valeur de base 99 pour détection du premier dessin
int inc = 99;

//booléen de choix du type de dessin
bool deDrawing = false;

//centres des différents cercles
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

//initialisation des coordonnées des cercles
static void init_pos_circles() {
  
  //instanciation du layer
  Layer *root_layer = window_get_root_layer(s_main_window);
  
  //coordonnées de la fenêtre principale
  GRect bounds = layer_get_bounds(root_layer);
  
  //coordonnées des points des minutes
  MN_center_1 = GPoint(bounds.size.w / 2, bounds.size.h / 15);
  MN_center_2 = GPoint((bounds.size.w / 9) * 8, bounds.size.h / 3.25);
  MN_center_4 = GPoint((bounds.size.w / 9) * 8, (bounds.size.h / 3.25) * 2.25);
  MN_center_8 = GPoint(bounds.size.w / 2, bounds.size.h - bounds.size.h / 15);
  MN_center_16 = GPoint(bounds.size.w / 9, (bounds.size.h / 3.25) * 2.25);
  MN_center_32 = GPoint(bounds.size.w / 9, bounds.size.h / 3.25);
  
  //coordonnées des points des heures
  HR_center_1 = GPoint(bounds.size.w / 2, bounds.size.h / 4.5);
  HR_center_2 = GPoint(bounds.size.w - bounds.size.w / 4.5, bounds.size.h / 2);
  HR_center_4 = GPoint(bounds.size.w / 2, bounds.size.h - bounds.size.h / 4.5);
  HR_center_8 = GPoint(bounds.size.w / 4.5, bounds.size.h / 2);
  
}

// **************
// * Animations *
// **************

//Gestion du rebouclage du dessin pour grossissement des cercles
static void drawingHandler() {
  
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "Dessin, radius : %d", inc);
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
static void handle_internal(GContext *ctx, int *unit, int stud, GPoint center) {
  
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "unité = %d, stud : %d", *unit, stud);
  
  //étude du cas
  if(*(unit) / stud > 0) {
    
    //maj des minutes restantes à afficher
    *(unit) -= stud; 
    
  } else {
    
    //dessin du cercle
    graphics_fill_circle(ctx, center, _RADIUS_0);
    
  }
  
}

//procédure de dessin des cercles internes pour affichage de l'heure
static void draw_intern_circles(GContext *ctx) {
  
  //application de la couleur de l'arrière plan
  graphics_context_set_fill_color(ctx, BG_color);
  
  //étude des minutes
  handle_internal(ctx, &minutes, 32, MN_center_32);
  handle_internal(ctx, &minutes, 16, MN_center_16);
  handle_internal(ctx, &minutes, 8, MN_center_8);
  handle_internal(ctx, &minutes, 4, MN_center_4);
  handle_internal(ctx, &minutes, 2, MN_center_2);
  handle_internal(ctx, &minutes, 1, MN_center_1);
  
  //étude des heures
  handle_internal(ctx, &hour, 8, HR_center_8);
  handle_internal(ctx, &hour, 4, HR_center_4);
  handle_internal(ctx, &hour, 2, HR_center_2);
  handle_internal(ctx, &hour, 1, HR_center_1);
  
}

// ******************************
// * Mises à jour contextuelles *
// ******************************

//Procédure de maj de l'heure dans les variables locales
static void update_local_time() {

  //initialisation de l'heure
  time_t temp = time(NULL); 
  
  //récupération de la structure contenant l'heure
  struct tm *tick_time = localtime(&temp);
  
  //buffer d'écriture
  static char buffer[] = "00";
  
  //écriture de l'heure actuelle dans le buffer
  strftime(buffer, sizeof("00"), "%I", tick_time);
  
  //récupération de l'heure sous forme d'entier
  hour = atoi(buffer);
  
  //écriture des minutes actuelles dans le buffer
  strftime(buffer, sizeof("00"), "%M", tick_time);
  
  //récupération des minutes sous forme d'entier
  minutes = atoi(buffer);
  
}

//fonction de mise à jour du layer de dessin
void global_layer_update_proc(Layer *layer, GContext *ctx) {
  
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
  if (inc != 99) {
  
    //dédraw des cercles
    app_timer_register(_DELTA_ANIM, deDrawingHandler, NULL);  
  
  } else {
    
    //sinon, premier dessin donc juste draw
    inc = 1;
    
  }
      
  //Maj de l'heure dans les variables locales
  update_local_time();

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "passage");

  //dessin
  app_timer_register(_DELTA_ANIM, drawingHandler, NULL);
  
}

// *****************************************
// * Constructeur/destructeur de la fenêtre *
// *****************************************

//fonction de construction de la fenêtre
static void main_window_load(Window *window) {
  
  //applique la couleur d'arrière plan
  window_set_background_color(window, BG_color); 
  
  //dessin des cercles en état initial
  //layer_mark_dirty(s_global_layer); 
  
}

//fonction de destruction de la fenêtre
static void main_window_unload(Window *window) {

  //désallocation des layers
  layer_destroy(s_global_layer);
  
}

// *****************************************
// * Constructeur/destructeur du programme *
// *****************************************

//fonction d'initialisation du contexte
void init(void) {
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  //instanciation du layer
  Layer *root_layer = window_get_root_layer(s_main_window);
  
  //coordonnées de la fenêtre principale
  GRect bounds = layer_get_bounds(root_layer);
  
  //création du layer de dessin
  s_global_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_global_layer, global_layer_update_proc);
  layer_add_child(root_layer, s_global_layer);
  
  //initialisation des couleurs / TODO paramétrer avec la page de config
  BG_color = GColorBlack;
  HR_color = GColorCyan;
  MN_color = GColorRed;
  
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