#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i;
  playerc_client_t *client;
  playerc_sonar_t *sonar;

  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
    return -1;

  sonar = playerc_sonar_create(client, 0);
  if (playerc_sonar_subscribe(sonar, PLAYER_OPEN_MODE))
    return -1;

  for (i = 0; i < 1000; i++)
  {
    playerc_client_read(client);    
    //Mostra a leitura do sonar, sensores 0 (esquerda), 4 (frente) e 7 (direita)
  printf("Sonar: esquerda:%.3fm\n   frente: %.3fm\n  direita: %.3fm\n\n", 
    sonar->scan[0], sonar->scan[4], sonar->scan[7]);
  } 

  playerc_sonar_unsubscribe(sonar);
  playerc_sonar_destroy(sonar);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
