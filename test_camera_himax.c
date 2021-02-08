#include "format.h"

void printImage(uint8_t *imgBuff, int height, int width);



static void dump_reg()
{
    printf("reg CFG_GLOB: %x \n", *(volatile unsigned int *)(long)(0x1A1024A0));
    printf("reg CFG_LL: %x \n", *(volatile unsigned int *)(long)(0x1A1024A4));
    printf("reg CFG_UR: %x \n", *(volatile unsigned int *)(long)(0x1A1024A8));
    printf("reg CFG_SIZE: %x \n", *(volatile unsigned int *)(long)(0x1A1024AC));
    printf("reg CFG_FILTER: %x \n", *(volatile unsigned int *)(long)(0x1A1024B0));
}

/*
static void cam_handler(void *arg)
{
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

    sprintf(imgName, "../../../img_OUT_%ld.ppm", idx);
    //WriteImageToFile(imgName, CAMERA_WIDTH, CAMERA_HEIGHT, sizeof(uint8_t), imgBuff0, GRAY_SCALE_IO);
    WriteImageToFile(imgName, CAMERA_WIDTH, CAMERA_HEIGHT, sizeof(uint8_t), NULL, GRAY_SCALE_IO);
    idx++;
}
*/

static int32_t open_camera_himax(struct pi_device *device)
{
    struct pi_himax_conf cam_conf;
    pi_himax_conf_init(&cam_conf);

#ifdef SLICE_MODE
    cam_conf.roi.slice_en = 1;
    cam_conf.roi.x = X;
    cam_conf.roi.y = Y;
    cam_conf.roi.w = CAMERA_WIDTH;
    cam_conf.roi.h = CAMERA_HEIGHT;
#endif

    pi_open_from_conf(device, &cam_conf);
    if (pi_camera_open(device))
    {
        return -1;
    }

    /* Let the camera AEG work for 100ms */
    /* Can be optimized */
    pi_time_wait_us(100000);
    pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);

    return 0;
}

void img2file(uint8_t *imgBuff, int height, int width, char* fileName)
{
    if(!strcmp(fileName, NULL)) fileName = "OUT";

    sprintf(imgName, "../../../img_%s.ppm", fileName);
    printf("Ecriture de l'image dans le ficher : %s\n", imgName);
    //WriteImageToFile(imgName, CAMERA_WIDTH, CAMERA_HEIGHT, sizeof(uint8_t), imgBuff, GRAY_SCALE_IO);
    WriteImageToFile(imgName, width, height, sizeof(uint8_t), imgBuff, GRAY_SCALE_IO);

}

uint8_t *get_image()
{
    printf("Capture de l'image\n");
    int32_t errors = 0;
    
    uint8_t *imgBuff;
    imgBuff = (uint8_t *) pi_l2_malloc((CAMERA_WIDTH * CAMERA_HEIGHT) * sizeof(uint8_t));
    printf("CAMERA_HEIGHT = %d et CAMERA_WIDTH = %d, %d\n", CAMERA_HEIGHT, CAMERA_WIDTH, sizeof(uint8_t));
    
    if (imgBuff == NULL)
    {
        printf("Failed to allocate Memory for Image, asking for: %d * %d\n", CAMERA_WIDTH, CAMERA_HEIGHT);
        pmsis_exit(-1);
    }

    errors = open_camera_himax(&cam);
    if (errors)
    {
        printf("Failed to open camera : %ld\n", errors);
        pmsis_exit(-2);
    }


    #if defined(ASYNC)
    printf("On est en assynchrone\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
        pi_task_callback(&task, cam_handler, NULL);
        pi_camera_capture_async(&cam, imgBuff, CAMERA_WIDTH * CAMERA_HEIGHT, &task);
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_task_wait_on(&task);
    #else
        printf("On est en synchrone\n");
        pi_camera_control(&cam, PI_CAMERA_CMD_START, 0);
        pi_camera_capture(&cam, imgBuff, CAMERA_WIDTH * CAMERA_HEIGHT);
        pi_camera_control(&cam, PI_CAMERA_CMD_STOP, 0);
    #endif  /* ASYNC */




    return(imgBuff);
}

void printImage(uint8_t *imgBuff, int height, int width)
{
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            printf("%02X", imgBuff[width*i + j]);
        }
        printf("\n");
    }
}

void run()
{
    uint8_t *my_imgBuff = NULL;

    pi_perf_conf(1<<PI_PERF_ACTIVE_CYCLES);
    pi_perf_reset();
    pi_perf_start();
    my_imgBuff = get_image();
    img2file(my_imgBuff, CAMERA_HEIGHT, CAMERA_WIDTH, "big");

    printf("Nb_cycle fonction get_image %10d \n", pi_perf_read(PI_PERF_ACTIVE_CYCLES));   
    printf("image pointer = %p\n", my_imgBuff);
    
    uint8_t *imgBuff_resized = NULL;
    pi_perf_reset();
    pi_perf_start();
    imgBuff_resized = formatage(my_imgBuff);
    printf("Nb_cycle fonction formatage %10d \n", 		   pi_perf_read(PI_PERF_ACTIVE_CYCLES)); 

    printImage(imgBuff_resized, 28, 28);

    
    img2file(imgBuff_resized, 28, 28, "little");

    printf("Exiting...\n");
    pmsis_exit(0);
}

int main(void)
{
    printf("\n\t*** PMSIS Camera with Himax ***\n\n");

    return pmsis_kickoff((void *) run);
}

