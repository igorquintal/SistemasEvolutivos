#include <stdio.h>
#include <libplayerc/playerc.h>

int main(int argc, const char **argv)
{
  int i, k;
  playerc_client_t *client;
  playerc_position2d_t *position2d;
  playerc_gripper_t *gripper;

  client = playerc_client_create(NULL, "localhost", 6665);
  if (playerc_client_connect(client) != 0)
    return -1;

  position2d = playerc_position2d_create(client, 0);
  if (playerc_position2d_subscribe(position2d, PLAYERC_OPEN_MODE) != 0)
  {
    fprintf(stderr, "error: %s\n", playerc_error_str());
    return -1;
  }

  gripper = playerc_gripper_create(client, 0);
  if (playerc_gripper_subscribe(gripper, PLAYER_OPEN_MODE))
    return -1;

  playerc_position2d_enable(position2d, 1);
  playerc_gripper_open_cmd(gripper);
  playerc_gripper_close_cmd(gripper) ;
//  playerc_gripper_store_cmd(gripper) ;
  sleep(2);

  playerc_position2d_set_cmd_vel(position2d, 0.2, 0, 0, 1);

  for (i = 0; i < 20; i++)
  {
    playerc_client_read(client);
  } 

  playerc_position2d_set_cmd_vel(position2d, 0, 0, 0, 1);
  playerc_gripper_open_cmd(gripper);
  sleep(2);

  playerc_gripper_unsubscribe(gripper);
  playerc_gripper_destroy(gripper);
  playerc_position2d_unsubscribe(position2d);
  playerc_position2d_destroy(position2d);
  playerc_client_disconnect(client);
  playerc_client_destroy(client);

  return 0;
}
