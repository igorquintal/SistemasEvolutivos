#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i;

  //Cliente para conexão com o servidor
  playerc_client_t *client;
  
  //Objeto para conexão com o odômetro
  playerc_position2d_t *position2d;

  //Conecta ao servidor no endereço “localhost”, na porta 6665 (default)  
  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  //Conecta ao odômetro
  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  //Ativa os motores do robô
  playerc_position2d_enable(position2d, 1);

  //Comando para o robô se mover para frente a 0,2m/s e rotacionar a 0.4rad/s
  playerc_position2d_set_cmd_vel(position2d, 0.2, 0, 0.4, 1);

  for (i = 0; i < 400; i++)
  {
    //Recebe dados do servidor
    playerc_client_read(client);

    //Mostra a posição do robô
    printf("position : %f %f %f\n", position2d->px, position2d->py, \
    position2d->pa);
  } 

  //Desconecta do odômetro e do servidor
  playerc_position2d_unsubscribe(position2d);
  playerc_position2d_destroy(position2d);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
