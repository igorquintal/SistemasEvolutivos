#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i;
  playerc_client_t *client;
  playerc_laser_t *laser;

  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
    return -1;

  laser = playerc_laser_create(client, 0);
  if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
    return -1;

  for (i = 0; i < 10000; i++)
  {
    playerc_client_read(client);
    
    //Mostra a leitura do laser, sensores 0 (esquerda), 
    //180 (frente) e 360 (direita)
    printf("Laser: direita:%.3fm\n  esquerda: %.3fm\n  frente: %.3fm\n\n", 
    laser->scan[0][0], laser->scan[360][0], laser->scan[180][0]);
  } 

  playerc_laser_unsubscribe(laser);
  playerc_laser_destroy(laser);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
