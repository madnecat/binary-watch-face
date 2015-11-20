#include <pebble.h>

//rayon du cercle plein
#define _RADIUS_1 8

//rayon du cercle vide
#define _RADIUS_0 7

//screen size : 144 * 168

// ****************
// * Déclarations *
// ****************

//fenêtre principale
static Window *s_main_window;

//layer global
static Layer *s_global_layer;

//couleurs de la watchface
GColor BG_color;
GColor MN_color;
GColor HR_color;

//centres des différents cercles
GPoint MN_center_1, MN_center_2, MN_center_4, MN_center_8, MN_center_16, MN_center_32;
GPoint HR_center_1, HR_center_2, HR_center_4, HR_center_8, HR_center_16;

// **************
// * Procédures *
// **************

//initialisation des coordonnées des cercles
static void init_pos_circles() {
  
  //instanciation du layer
  Layer *root_layer = window_get_root_layer(s_main_window);
  
  //coordonnées de la fenêtre principale
  GRect bounds = layer_get_bounds(root_layer);
  
  //coordonnées des points des minutes
  MN_center_1 = GPoint(bounds.size.w / 2, bounds.size.h / 15);
  MN_center_2 = GPoint((bounds.size.w / 7) * 6, bounds.size.h / 3.25);
  MN_center_4 = GPoint((bounds.size.w / 7) * 6, (bounds.size.h / 3.25) * 2.25);
  MN_center_8 = GPoint(bounds.size.w / 2, bounds.size.h - bounds.size.h / 15);
  MN_center_16 = GPoint(bounds.size.w / 7, bounds.size.h / 3.25);
  MN_center_32 = GPoint(bounds.size.w / 7, (bounds.size.h / 3.25) * 2.25);
}

//procédure de dessin des cercles en état initial
static void draw_full_circles(Layer *layer, GContext *ctx) {
  
  //application de la couleur des minutes au dessin
  graphics_context_set_fill_color(ctx, MN_color);
  
  //dessin des points des minutes
  graphics_fill_circle(ctx, MN_center_1, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_2, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_4, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_8, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_16, _RADIUS_1);
  graphics_fill_circle(ctx, MN_center_32, _RADIUS_1);
  
}

//fonction de mise à jour du layer de dessin
void global_layer_update_proc(Layer *layer, GContext *ctx) {
  
  draw_full_circles(layer, ctx);
  
}

//fonction de construction de la fenêtre
static void main_window_load(Window *window) {
  
  //applique la couleur d'arrière plan
  window_set_background_color(window, BG_color); 
  
  //dessin des cercles en état initial
  layer_mark_dirty(s_global_layer); 
  
}

//fonction de destruction de la fenêtre
static void main_window_unload(Window *window) {
}

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
  
  //initialisation de la position des cercles
  init_pos_circles();

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
}

//fonction de destruction du contexte
void deinit(void) {
  
  // Destroy Window
  window_destroy(s_main_window);
  
}

//fonction principale
int main(void) {
  
  //appel de l'initialisation du programme
  init();
    
  //attente d'un événement système
  app_event_loop();
    
  //appel à la destruction des éléments dynamiques
  deinit();
}