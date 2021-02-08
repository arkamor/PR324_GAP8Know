#include "stdio.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

/* PMSIS includes. */
#include "pmsis.h"

/* PMSIS BSP includes. */
#include "bsp/bsp.h"
#include "bsp/camera.h"

/* Demo includes. */
#include "gaplib/ImgIO.h"


#ifdef SLICE_MODE
#define X                   60 
#define Y                   60 
#define CAMERA_WIDTH        260
#define CAMERA_HEIGHT       180
#else
#define CAMERA_WIDTH        320
#ifdef QVGA_IMG
#define CAMERA_HEIGHT       240
#else // max resolution of Himax camera
#define CAMERA_HEIGHT       324
#endif /* QVGA */
#endif

#define LCD_WIDTH 160
#define LCD_HEIGHT 120
#define Neuronal_WIDTH 14
#define Neuronal_HEIGHT 14
#define Neuronal_WIDTH_bis 28
#define Neuronal_HEIGHT_bis 28

static pi_task_t task;
static struct pi_device cam;

static char imgName[50];
static uint32_t idx = 0;

//static void cam_handler(void *arg);
//static void lcd_handler(void *arg);


//unsigned int Time = 0;
//unsigned int Time_N_1 = 0;


unsigned char *formatage(uint8_t *imgBuff0);