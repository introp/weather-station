#ifndef AVALON_CAMERA_H_
#define AVALON_CAMERA_H_
//! @file

//! Camera BSP initialization (things that must get run ASAP after bootup).
void camera_bsp_init();

//! Camera task.
//! Must call camera_bsp_init before starting this task.
void camera_task(void *pvParameters);


#endif /* AVALON_CAMERA_H_ */
