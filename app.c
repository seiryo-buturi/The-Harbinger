#include "ev3api.h"
#include "app.h"

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

/**
 * Define the connection ports of the sensors and motors.
 * By default, this application uses the following ports:
 * left color sensor: Port 2
 * right Color sensor: Port 3
 * Left motor:   Port B
 * Right motor:  Port C
 */
const int l_color = EV3_PORT_2, r_color = EV3_PORT_3, l_motor = EV3_PORT_B, r_motor = EV3_PORT_C;

//�߂�{�^���̉���
static void button_clicked_handler(intptr_t button) {
    switch(button) {
    case BACK_BUTTON:
#if !defined(BUILD_MODULE)
    	//LCD�ɕ\��
        syslog(LOG_NOTICE, "Back button clicked.");
#endif
        break;
    }
}

void main_task(intptr_t unused) {
    // �{�^���̃C�x���g
    ev3_button_set_on_clicked(BACK_BUTTON, button_clicked_handler, BACK_BUTTON);

    // ���[�^�[�ݒ�
    ev3_motor_config(l_motor, LARGE_MOTOR);
    ev3_motor_config(r_motor, LARGE_MOTOR);

    // �Z���T�[�ݒ�
    ev3_sensor_config(l_color, COLOR_SENSOR);
    ev3_sensor_config(r_color, COLOR_SENSOR);

    /**
     * Calibrate for light intensity of WHITE
     */
    ev3_speaker_play_tone(NOTE_C4, 100);
    //���s��ԁ[���҂�
	tslp_tsk(100);
    ev3_speaker_play_tone(NOTE_C4, 100);
    // TODO: Calibrate using maximum mode => 100
    //�ǂ����ɕ\��
	printf("Press the touch sensor to measure light intensity of WHITE.\n");
    //�����������s�� white��臒l����̂��߂�
	while(!ev3_touch_sensor_is_pressed(touch_sensor));
    while(ev3_touch_sensor_is_pressed(touch_sensor));
    int white = ev3_color_sensor_get_reflect(color_sensor);
    printf("WHITE light intensity: %d.\n", white);
	//��������������������������������������������������

    /**
     * Calibrate for light intensity of BLACK
     */
    ev3_speaker_play_tone(NOTE_C4, 100);
    tslp_tsk(100);
    ev3_speaker_play_tone(NOTE_C4, 100);
    // TODO: Calibrate using maximum mode => 100
    // TODO: Calibrate using minimum mode => 0
    printf("Press the touch sensor to measure light intensity of BLACK.\n");
    while(!ev3_touch_sensor_is_pressed(touch_sensor));
    while(ev3_touch_sensor_is_pressed(touch_sensor));
    int black = ev3_color_sensor_get_reflect(color_sensor);
    printf("BLACK light intensity: %d.\n", black);

    /**
     * PID controller
     */
    float lasterror = 0, integral = 0;
    float midpoint = (white - black) / 2 + black;
    while (1) {
        //�΍�
    	float error = midpoint - ev3_color_sensor_get_reflect(color_sensor);
        integral = error + integral * 0.5;
        //�����
    	float steer = 0.07 * error + 0.3 * integral + 1 * (error - lasterror);
        ev3_motor_steer(left_motor, right_motor, 10, steer);
        lasterror = error;
        tslp_tsk(1);
    }
}
