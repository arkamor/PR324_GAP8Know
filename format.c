#include "format.h"


/******************* Image formating *******************/
unsigned char *formatage(uint8_t *imgBuff0)
{

  int taux_noir = 0;
  uint32_t im_length = CAMERA_WIDTH*CAMERA_HEIGHT;
  uint32_t lcd_length = LCD_WIDTH*LCD_HEIGHT;
  uint32_t neuronal_length = Neuronal_WIDTH*Neuronal_HEIGHT;
  uint32_t idx = 0;

  #ifdef __EMUL__
    FILE* fichier = NULL;
    fichier = fopen("/home/thibault/Documents/projet_SVN/free-gap8/gap8/test_camera_seq_uart/logs/log_chiffre_7sansbarre_2_newline.txt", "r");//_6_newline
    if (fichier != NULL)
    {
      char *lineptr = NULL;
      size_t n = 0;

      for(uint32_t idx = 0; idx < CAMERA_WIDTH * CAMERA_HEIGHT; idx++)
      {
        getline(&lineptr, &n, fichier);
        sscanf(lineptr, "%d ", &(imgBuff0[idx]));
      } 
      fclose(fichier);
    }
  #endif
  // Détection de la zone traitable de l'image (sans noir perturbant l'identification d'un chiffre)
  //printf("Nb_cycle fonction formatage (detection de la zone traitable) %10d \n", pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time); 
  int diag_1 = 0;
  int diag_2 = 0;
  int diag_3 = 0;
  int diag_4 = 0;
  int ab_1 = 0;
  int ab_2 = 0;
  int ab_3 = 0;
  int ab_4 = 0;
  int ord_1 = 0;
  int ord_2 = 0;
  int ord_3 = 0;
  int ord_4 = 0;
  int XA = 0;
  int XB = 0;
  int YA = 0;
  int YB = 0;
  int ab = 0;
  int ord = 0;
  int coord_x_max = 0;
  int coord_x_min = 0;
  int coord_y_max = 0;
  int coord_y_min = 0;

  int MOY_LUMINOSITE = 0;
  for(uint32_t id = 0; id < CAMERA_WIDTH*CAMERA_HEIGHT; id++)
  {
    MOY_LUMINOSITE = imgBuff0[id] + MOY_LUMINOSITE;
  }

  MOY_LUMINOSITE = MOY_LUMINOSITE / (CAMERA_WIDTH*CAMERA_HEIGHT);
  //printf("MOY_LUMINOSITE = %d \n", MOY_LUMINOSITE);
    
  for(uint32_t idx = 0; idx < CAMERA_HEIGHT; idx++)
  {
    if((imgBuff0[(idx+1)*CAMERA_WIDTH - idx] > (MOY_LUMINOSITE - 15)) && (diag_1 == 0))
    diag_1 = idx*CAMERA_WIDTH - idx;

    if((imgBuff0[(CAMERA_WIDTH*(CAMERA_HEIGHT-1-idx))+1+idx] > (MOY_LUMINOSITE - 15)) && (diag_2 == 0))
    diag_2 = (CAMERA_WIDTH*(CAMERA_HEIGHT-1-idx))+1+idx;

    if((imgBuff0[(idx*(CAMERA_WIDTH+1)) + idx] > (MOY_LUMINOSITE - 15)) && (diag_3 == 0))
    diag_3 = idx*(CAMERA_WIDTH+1) + idx;

    if((imgBuff0[(CAMERA_WIDTH*(CAMERA_HEIGHT-1))-idx] > (MOY_LUMINOSITE - 15)) && (diag_4 == 0))
    diag_4 = (CAMERA_WIDTH*(CAMERA_HEIGHT-1))-idx;

    if(diag_1 != 0 && diag_2 != 0 && diag_3 != 0 && diag_4 != 0)
    break;
  }

  ab_1 = (diag_1 % CAMERA_WIDTH);
  ab_2 = (diag_2 % CAMERA_WIDTH);
  ab_3 = (diag_3 % CAMERA_WIDTH);
  ab_4 = (diag_4 % CAMERA_WIDTH);

  ord_1 = (diag_1 / CAMERA_WIDTH); //valeur absolue : directement car int
  ord_2 = (diag_2 / CAMERA_WIDTH);
  ord_3 = (diag_3 / CAMERA_WIDTH);
  ord_4 = (diag_4 / CAMERA_WIDTH);

  if (abs(162 - ab_1) < abs(162 - ab_2))
  XA = ab_1;
  else
  XA = ab_2;

  if (abs(162 - ab_3) < abs(162 - ab_4))
  XB = ab_3;
  else
  XB = ab_4;

  if (abs(162 - XA) < abs(162 - XB))
  ab = XA;
  else
  ab = XB;

  if (abs(122 - ord_1) < abs(122 - ord_2))
  YA = ord_1;
  else
  YA = ord_2;

  if (abs(122 - ord_3) < abs(122 - ord_4))
  YB = ord_3;
  else
  YB = ord_4;

  if (abs(122 - YA) < abs(122 - YB))
  ord = YA;
  else
  ord = YB;

  if((162 - ab) < 0)
  {
    coord_x_max = ab;
    coord_x_min = 162 - (ab - 162);
  }
  else
  {
    coord_x_min = ab;
    coord_x_max = 162 + (162 - ab);
  }

  if((122 - ord) < 0)
  {
    coord_y_min = 122 - (ord - 122);
    coord_y_max = ord;
  }
  else
  {
    coord_y_min = ord;
    coord_y_max = 122 + (122 - ord);
  }
  //printf("Nb_cycle TEST %10d \n", pi_perf_read(PI_PERF_ACTIVE_CYCLES)); 
  // Definition image avec couleur adaptée
  int CAM_WIDTH_RESIZE = coord_x_max - coord_x_min;
  int CAM_HEIGHT_RESIZE = coord_y_max - coord_y_min;
  unsigned char *imgBuff_resize;
  imgBuff_resize = (unsigned char *) pi_l2_malloc(CAM_WIDTH_RESIZE*CAM_HEIGHT_RESIZE* sizeof(unsigned char));
  printf("CAM_HEIGHT_RESIZE = %d, CAM_WIDTH_RESIZE = %d, %d\n", CAM_HEIGHT_RESIZE,CAM_WIDTH_RESIZE, sizeof(unsigned char));
  if(!imgBuff_resize)
  {
    printf("imgBuff_resize malloc error\n");
    pmsis_exit(-1);
  }

  for(uint32_t idy = 0; idy < CAM_HEIGHT_RESIZE; idy++)
  {
    for (uint32_t idx = 0; idx < CAM_WIDTH_RESIZE; idx++)
    {
      imgBuff_resize[idx + idy*CAM_WIDTH_RESIZE] = imgBuff0[coord_x_min + idx + (coord_y_min+idy)*CAMERA_WIDTH];
    }
  }
  
  pi_l2_free(imgBuff0,(CAMERA_WIDTH * CAMERA_HEIGHT) * sizeof(uint8_t));

  //printImage(imgBuff_resize, CAM_WIDTH_RESIZE, CAM_HEIGHT_RESIZE);

  // Detection présence noir et binarisation
  uint32_t *indice_noir;
  indice_noir = pi_l2_malloc(CAM_WIDTH_RESIZE*CAM_HEIGHT_RESIZE* sizeof(uint32_t));
  //printf("CAM_HEIGHT_RESIZE = %d et CAM_WIDTH_RESIZE = %d\n", CAM_HEIGHT_RESIZE, CAM_WIDTH_RESIZE);
  if(!indice_noir)
  {
    printf("error malloc indice_noir\n");
    pmsis_exit(-1);
  }

  for(uint32_t id = 0; id < CAM_HEIGHT_RESIZE*CAM_WIDTH_RESIZE; id++)
  {
    if(imgBuff_resize[id] < MOY_LUMINOSITE - 20)
    {
      imgBuff_resize[id] = 255;
      indice_noir[taux_noir] = id;
      taux_noir++;
    }
    else{
    imgBuff_resize[id] = 0;
    }
  }
  //printf("taux noir : %d\n", taux_noir);
  
  int imin_x = CAM_WIDTH_RESIZE;
  int imax_x = 0;
  int imin_y = CAM_HEIGHT_RESIZE;
  int imax_y = 0;

  // Détection de la position du chiffre et définition de sa taille
  for(int i = 0; i < taux_noir; i++)
  {
      
    if (indice_noir[i] != (char)0)
    {
      if((indice_noir[i] % CAM_WIDTH_RESIZE) < imin_x)
      imin_x = indice_noir[i] % CAM_WIDTH_RESIZE;

      if((indice_noir[i] % CAM_WIDTH_RESIZE) > imax_x)
      imax_x = indice_noir[i] % CAM_WIDTH_RESIZE;

      if(indice_noir[i] / CAM_WIDTH_RESIZE > imax_y)
      imax_y = indice_noir[i] / CAM_WIDTH_RESIZE;

      if(indice_noir[i] / CAM_WIDTH_RESIZE < imin_y)
      imin_y = indice_noir[i] / CAM_WIDTH_RESIZE;
    }
  }

  pi_l2_free(indice_noir, CAM_WIDTH_RESIZE*CAM_HEIGHT_RESIZE*sizeof(uint32_t));

  int CAM_WIDTH_ADAPT = abs((imax_x+2) - (imin_x-2));
  int CAM_HEIGHT_ADAPT = abs((imax_y+2) - (imin_y-2));

  unsigned char *imgBuff_adapt;
  
  imgBuff_adapt = (unsigned char *) pi_l2_malloc(CAM_WIDTH_ADAPT*CAM_HEIGHT_ADAPT*sizeof(unsigned char));
  printf("CAM_WIDTH_ADAPT = %d, CAM_HEIGHT_ADAPT = %d\n", CAM_WIDTH_ADAPT, CAM_HEIGHT_ADAPT);
  if(!imgBuff_adapt)
  {
    printf("error malloc imgBuff_adapt\n");
    pmsis_exit(-1);
  }

  // Définition de la nouvelle image avec que le chiffre
  for(uint32_t idy = 0; idy < CAM_HEIGHT_ADAPT; idy++)
  {
    for(uint32_t idx = 0; idx < CAM_WIDTH_ADAPT; idx++)
    {
        
      imgBuff_adapt[idx + idy*CAM_WIDTH_ADAPT] = imgBuff_resize[ idx + imin_x - 2 + ((imin_y-2)*CAM_WIDTH_RESIZE) + (idy*CAM_WIDTH_RESIZE) ];
    }
  }
  

  pi_l2_free(imgBuff_resize, CAM_WIDTH_RESIZE*CAM_HEIGHT_RESIZE* sizeof(unsigned char));

  // Redimensionnage en fonction de 28*28 - moyenne (facteur*facteur) tendance de 1 ou de 0
  float facteur_size_width = ((float) CAM_WIDTH_ADAPT / Neuronal_WIDTH);
  float facteur_size_height = ((float) CAM_HEIGHT_ADAPT / Neuronal_HEIGHT);

  if (facteur_size_height - floor(facteur_size_height) > 0.6)
    facteur_size_height = ceil(facteur_size_height);
  else
    facteur_size_height = floor(facteur_size_height);

  if (facteur_size_width - floor(facteur_size_width) > 0.8)
    facteur_size_width = ceil(facteur_size_width);
  else
    facteur_size_width = floor(facteur_size_width);
  
  
  unsigned char *imgBuff_reseau;
  imgBuff_reseau = (unsigned char *) pi_l2_malloc( Neuronal_WIDTH * Neuronal_HEIGHT * sizeof(unsigned char));
  printf("Neuronal_WIDTH = %d, Neuronal_HEIGHT = %d\n", Neuronal_WIDTH, Neuronal_HEIGHT);

  for(uint32_t idy = 0; idy < Neuronal_HEIGHT; idy++)
  {
      
    for(uint32_t idx = 0; idx < Neuronal_WIDTH; idx++)
    {
      int moy = 0;
      for(uint32_t i = 0; i < facteur_size_height; i++)
      {
        for(uint32_t x = 0; x < facteur_size_width; x++)
        {
          if(imgBuff_adapt[x + idx*(int)facteur_size_width + i*CAM_WIDTH_ADAPT + idy*CAM_WIDTH_ADAPT*(int)facteur_size_height] != 0)
          moy++;
        }
      }

      if(moy > facteur_size_height*facteur_size_width/20)//40
        imgBuff_reseau[idx + idy*Neuronal_WIDTH] = 255;
      else
        imgBuff_reseau[idx + idy*Neuronal_WIDTH] = 0;
    }
  }

  pi_l2_free(imgBuff_adapt, CAM_WIDTH_ADAPT*CAM_HEIGHT_ADAPT*sizeof(unsigned char));
    
  unsigned char *imgBuff_reseau_bis;


  imgBuff_reseau_bis = pi_l2_malloc( Neuronal_WIDTH_bis * Neuronal_HEIGHT_bis * sizeof(unsigned char));
  printf("Neuronal_HEIGHT_bis = %d, Neuronal_WIDTH_bis = %d\n", Neuronal_HEIGHT_bis, Neuronal_WIDTH_bis);

  for(uint32_t idy = 0; idy < Neuronal_HEIGHT; idy++)
  {
    for(uint32_t idx = 0; idx < Neuronal_WIDTH; idx++)
    {
      imgBuff_reseau_bis[idx + 7 + idy*(Neuronal_WIDTH_bis) + 7*Neuronal_WIDTH_bis] = imgBuff_reseau[idx + idy*Neuronal_WIDTH];
    }
  }

  pi_l2_free(imgBuff_reseau, Neuronal_WIDTH * Neuronal_HEIGHT * sizeof(unsigned char));

  
  for(uint32_t idy = 0; idy < Neuronal_HEIGHT_bis; idy++)
  {
    for(uint32_t idx = 0; idx < Neuronal_WIDTH_bis; idx++)
    {
      imgBuff_reseau_bis[idx + idy*(Neuronal_WIDTH_bis)] /= 255;
      
      //printf("%d", imgBuff_reseau_bis[idx + idy*(Neuronal_WIDTH_bis)]);
    }
    //printf("\n");
  }
  
    printf("Out formatage \n");
  
  return(imgBuff_reseau_bis);

}
