#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <libplayerc/playerc.h>

#define WIDTH 640
#define HEIGHT 480
#define SCALE 25
#define DESTINO_POSSIVEL 12
#define DESTINO 15




IplImage *image = 0;

int mapa[WIDTH][HEIGHT];

/********************************************************************/
/********************************************************************/
/* Função para calcular o tempo de execução de determinadas rotinas */
double getUnixTime(void)
{
    struct timespec tv;

    if(clock_gettime(CLOCK_REALTIME, &tv) != 0)
	{
		return 0;
	}

    return (((double) tv.tv_sec) + (double) (tv.tv_nsec / 1000000000.0));
}
/********************************************************************/
/********************************************************************/

void moveRobo(playerc_position2d_t *position2d, double vel, double velA){
	
	playerc_position2d_set_cmd_vel(position2d, vel, 0, velA, 1);
	
}

void draw_point(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
  image->imageData[3*((y*WIDTH)+x)]=b;
  image->imageData[3*((y*WIDTH)+x)+1]=g;
  image->imageData[3*((y*WIDTH)+x)+2]=r;
  }
	
void clear_screen() {
  int x,y;

  for(x=0; x<WIDTH; x++)
  for(y=0; y<HEIGHT; y++){
  image->imageData[3*((y*WIDTH)+x)]=0;
  image->imageData[3*((y*WIDTH)+x)+1]=0;
  image->imageData[3*((y*WIDTH)+x)+2]=0;
}
  }

void draw_line(int x0, int y0, int x1, int y1) {
 

  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    //draw_point (x0, y0, 255, 255, 255);
    if(mapa[x0][y0] != DESTINO){
		mapa[x0][y0] -= 1;
		if(mapa[x0][y0] < 0)
			mapa[x0][y0] = 0;
	}
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

void refresh_screen(void) {
  cvNamedWindow("Test OpenCV", 1);
  cvShowImage("Test OpenCV", image);
  if (cvWaitKey(50)!=-1)  exit(0) ;
  }
  
void update_screen(void) {
	
	register int i, j;
	unsigned int cor = 0;
	for(i=0; i<WIDTH; i++)
		for(j=0; j<HEIGHT; j++){
			if(mapa[i][j] == DESTINO_POSSIVEL){
				draw_point(i, j, 250, 0, 0); 
			}else if(mapa[i][j] == DESTINO){				
				draw_point(i, j, 0, 250, 0);					
			}else{
				cor = 250 - mapa[i][j]*25;
				draw_point(i, j, cor, cor, cor);
			}
		}
	
	cvNamedWindow("Test OpenCV", 1);
	cvShowImage("Test OpenCV", image);
	if (cvWaitKey(50)!=-1)  exit(0) ;
}

int main(int argc, char const *argv[]) {

	double vel_max = atof(argv[1]); 				//VELOCIDADE MAXIMA QUANDO ELE ESTA ANDANDO LIVREMENTE
	int incremento_laser = (int) atof(argv[2]); 			//INCREMENTO DA VISUALIZACAO DO LASER
	int pixels_analisados = (int) atof(argv[3]); 		// QUANTIDADE DE PIXELS ANALISADOS PARA ACHAR CONCENTRACAO DE VERMELHO
	int angulo_visao_esq = (int) atof(argv[4]); 			//LIMITE DA VISAO DO LASER NA ESQUERDA 
	double peso = atof(argv[5]); 					//PESO DA REPULSAO
	int angulo_visao_dir = (int) atof(argv[6]); 			//LIMITE DA VISAO DO LASER NA DIREITA 
	int n_giro = (int) atof(argv[7]); 					// QUANTIDADE DE VEZES QUE O ROBO VAI GIRAR
	double delta_vel_linear = atof(argv[8]); 		//VARIACAO DA VELOCIDADE LINEAR AO FAZER A CURVA
	int repulsao_limite = (int) atof(argv[9]); 			//VARIAVEL DE REPULSAO 

 
	image  = cvCreateImage(cvSize(WIDTH,HEIGHT), IPL_DEPTH_8U, 3 );
	int k,l;
	for(k=0; k<WIDTH; k++){
		for(l=0; l<HEIGHT; l++){
			mapa[k][l] = 5;
		}
	}
	int i, a;
	int estado = 0;
	a=0;
	playerc_client_t *client;
	playerc_laser_t *laser;
	playerc_position2d_t *pos_Robo;

	client = playerc_client_create(NULL, "localhost", 6665);
	if (playerc_client_connect(client) != 0)
		return -1;

	laser = playerc_laser_create(client, 0);
	if (playerc_laser_subscribe(laser, PLAYERC_OPEN_MODE))
		return -1;
    
	pos_Robo = playerc_position2d_create(client, 0);
	
	if (playerc_position2d_subscribe(pos_Robo, PLAYERC_OPEN_MODE) != 0){
		fprintf(stderr, "error: %s\n", playerc_error_str());
		return -1;
	}

	double start_time, stop_time;
	double tempo=0;
	int contMapa = 0;
	int cont = 0;
	double robot_old_x = 0.0;
	double robot_old_y = 0.0;

	start_time = getUnixTime();

	while (tempo < 180.0) {

		for(k=0; k<WIDTH; k++){
			for(l=0; l<HEIGHT; l++){
				if (mapa[k][l] == 0)
					contMapa++;
			}
		}
		
		playerc_client_read(client);
		int destino_X, destino_Y;
		int n,m;
		int contador_maior, n_maior, m_maior;
		double destino_real_X, destino_real_Y;
		double dist_Destino;
		double vel_Robo;
		double rot_Robo;
		double ang_rot;
		double ang_destino;
		double repulsao;

		//MOVIMENTO DO ROBÔ E DECISÃO DE DESTINO
		switch(estado){
			case 0: // RODANDO
				moveRobo(pos_Robo, 0.0, 0.5);
				a++;
				if(a >= n_giro){ //*********************************************
					estado = 1;
					a = 0;
				}
				break;
			case 1: // ANALISANDO DESTINOS POSSIVEIS
				moveRobo(pos_Robo, 0.0, 0.0);
				for(k=0; k<WIDTH; k++){
					for(l=0; l<HEIGHT; l++){
						if((mapa[k][l] == 0) & ((mapa[k-1][l] == 5) || (mapa[k+1][l] == 5) || (mapa[k][l-1] == 5) || (mapa[k][l+1] == 5) ||
							(mapa[k-1][l-1] == 5) || (mapa[k-1][l+1] == 5) || (mapa[k+1][l-1] == 5) || (mapa[k+1][l+1] == 5))){
							mapa[k][l] = DESTINO_POSSIVEL;
						}
					}
				}
				estado = 2;
				break;
			case 2: //ESCOLHENDO DESTINO
				contador_maior = n_maior = m_maior = 0;
				for(k=0; k<WIDTH; k++){
					for(l=0; l<HEIGHT; l++){
						if(mapa[k][l] == DESTINO_POSSIVEL){
							int contador;
							contador = 0;
							for(n = k-pixels_analisados; n < k+pixels_analisados; n++){ // ***************************
								for(m = l-pixels_analisados; m < l+pixels_analisados; m++){
									if(mapa[n][m] == DESTINO_POSSIVEL)
										contador++;
								}
							}
							if(contador > contador_maior){
								n_maior = k;
								m_maior = l;
								contador_maior = contador;
							}
						}							
					}
				}
				
				for(n = n_maior-4; n <= n_maior+4; n++){
					for(m = m_maior-4; m <= m_maior+4; m++){
						mapa[n][m] = DESTINO;
					}
				}			
				
				destino_X = n_maior;
				destino_Y = m_maior;
				destino_real_X = destino_X - 120; 
				destino_real_Y = HEIGHT - destino_Y - 40;
				destino_real_X = destino_real_X/SCALE;
				destino_real_Y = destino_real_Y/SCALE;
				destino_real_X -=8;
				destino_real_Y -=8;
				estado = 3;
				break;
			case 3: //MOVIMENTO DO ROBÔ

				//printf ("CONTADOR %d \n", cont);

				if (cont == 5) {

					//printf (" POSICAO %f %f (OLD) %f %f (ATUAL)\n", robot_old_x, robot_old_y, pos_Robo->px, pos_Robo->py);

					if (robot_old_x == pos_Robo->px && robot_old_y == pos_Robo->py) {

						printf ("%d", contMapa);
						return 0;
					}
				}

				if (cont == 5) {
					robot_old_x = pos_Robo->px;
					robot_old_y = pos_Robo->py;
					cont = 0;
				}

				cont = cont + 1;

				dist_Destino = sqrt((pos_Robo->px - destino_real_X)*(pos_Robo->px - destino_real_X) + (pos_Robo->py - destino_real_Y)*(pos_Robo->py - destino_real_Y));
				vel_Robo;
				
				if (dist_Destino > 1.2) 
					vel_Robo = vel_max; //******************************************
				else{ 
					vel_Robo = 0.0;
					for(n = destino_X-4; n <= destino_X+4; n++){
						for(m = destino_Y-4; m <= destino_Y+4; m++){
							mapa[n][m] = 10; //era 0
						}
					}	
					moveRobo(pos_Robo, vel_Robo, rot_Robo);
					estado = 0;
				}
				ang_destino = atan2(destino_real_Y - pos_Robo->py, destino_real_X-pos_Robo->px);
				ang_rot = pos_Robo->pa - ang_destino;
				rot_Robo = -ang_rot;
				
				repulsao = 0;
				
				for(i=190; i<=angulo_visao_dir; i+=incremento_laser) //**************************************
					if (laser->scan[i][0] < 1.1 && vel_Robo < 2.0) {
						repulsao += repulsao_limite -laser->scan[i][0]; //**************************************
						vel_Robo -= delta_vel_linear; //**************************************
					}

				for(i=180; i>=angulo_visao_esq; i-=incremento_laser) //*************************************************** 
					if (laser->scan[i][0] < 1.1 && vel_Robo < 2.0) {
						repulsao -= repulsao_limite -laser->scan[i][0]; //**************************************
						vel_Robo -= delta_vel_linear; //**************************************
					}

					if (repulsao == 0 && vel_Robo < 0.5) {
						vel_Robo += delta_vel_linear; //**************************************
					}


				rot_Robo -= peso * repulsao; //**************************************

				//printf("%f\n", rot_Robo);

				moveRobo(pos_Robo, vel_Robo, rot_Robo);
				
				break;
			default:
				break;
		}
	  
	  //GERENCIAMENTO DA MATRIZ
		if((estado==0) || (estado==3)){
			for(i=0; i<360; i+=1) {
					
				float x0 = WIDTH/2 + pos_Robo->px*SCALE;
				float y0 = HEIGHT - (HEIGHT/2 + pos_Robo->py*SCALE);
					
				float nx = laser->point[i].px*cos(pos_Robo->pa) - laser->point[i].py*sin(pos_Robo->pa);
				float ny = laser->point[i].py*cos(pos_Robo->pa) + laser->point[i].px*sin(pos_Robo->pa);
				
				float x1 = WIDTH/2 + (pos_Robo->px + nx)*SCALE;
				float y1 = HEIGHT - (HEIGHT/2 + (pos_Robo->py + ny)*SCALE);
					
				draw_line(x0, y0, x1, y1);
					
					
					
				if(laser->scan[i][0] < 8.0){
					int ix, iy;
					for(ix = (int)x1 - 1; ix <= (int)x1 + 2; ix++){
						for(iy = (int)y1 - 1; iy <= (int)y1 + 2; iy++){
							mapa[ix][iy] += 3;
							if(mapa[ix][iy] > 10)
								mapa[ix][iy] = 10;
						}
					}
				}
			}
		}

		update_screen();
		clear_screen();
		
		while (!cvWaitKey(50));

		stop_time = getUnixTime();
		tempo = stop_time - start_time;
	}

	printf ("%d", contMapa);
	return 0;
}
