
//lien entre la page de configuration et le bouton de paramètre
Pebble.addEventListener('showConfiguration', function(e) {
  
  // Show config page
  Pebble.openURL('http://necatdev.byethost12.com/config.html');
  
});

//Ecoute de l'événement de fermeture de la page de config
Pebble.addEventListener('webviewclosed', function(e) {
 
  //récupération des infos de configuration en JSON
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  console.log('valeur retournée : ' + JSON.stringify(configData));
  
  //si il y a des infos
  if(configData.obgMinutes) {
    
    Pebble.sendAppMessage({

      BG_MINUTES : parseInt(configData.obgMinutes, 16),
      BG_HOURS : parseInt(configData.obgHours, 16),
      BG_CENTER : parseInt(configData.obgCenter, 16),
      COL_MINUTES : parseInt(configData.ocolMinutes, 16),
      COL_HOURS : parseInt(configData.ocolHours, 16),
      COL_DAY : parseInt(configData.ocolDay, 16),
      DISPLAY_DAY : configData.odisplayDay ? 1 : 0
      
    }, function() {
      console.log('send successful');
    }, function() {
      console.log('send failed');                      
    });
    
  }
  
});