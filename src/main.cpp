/**
 * @author Sergiu-Petru Tabacariu
 * @date 2.6.2017
 *
 * @brief Simple program to control a RC car
 */

/** Includes */
#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "PCA9685.h"

/** Defines */
#define STEERING 0
#define STEERING_MAX 358
#define STEERING_MIN 256
#define STEERING_LEFT STEERING_MIN
#define STEERING_RIGHT STEERING_MAX
#define STEERING_STRAIGHT (STEERING_MIN+(STEERING_MAX-STEERING_MIN)/2)

#define ESC 1
#define ESC_MAX 409
#define ESC_MIN 204
#define ESC_D 358
#define ESC_R 250
#define ESC_N (ESC_MIN+(ESC_MAX-ESC_MIN)/2)

/** Namespaces */
using namespace std;
using namespace cv;

int main (int argc, char* argv[])
{
	/** Setup servo driver */
	PCA9685 pwm;
	pwm.init(1,0x40);
	pwm.setPWMFreq(50);
	
	printf("Starting ESC...\n");
	pwm.setPWM(ESC, ESC_N);
	sleep(5);
	printf("ESC started\n");
	
	char status = 'N';

	/** Setup camera */
	VideoCapture cam(CV_CAP_ANY);
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
	cam.set(CV_CAP_PROP_FPS, 25);
	if (!cam.isOpened()) {
		cerr << "ERROR: Could not open camera!" << endl;
	}
	namedWindow("Input");
	
	while (true) {
		
		Mat image;
		cam >> image;
		if (image.empty()) {
			cerr << "ERROR: Could not capture image!" << endl;
		}
		
		imshow("Input", image);
		char key = waitKey(40);
		if (key == 'q') {
			destroyAllWindows();
			cam.release();
			break;
		}
		else if (key == 'w') {
			pwm.setPWM(STEERING, STEERING_STRAIGHT);
			pwm.setPWM(ESC, ESC_D-30);
			status = 'D';
		}
		else if (key == 's') {
		    if (status == 'D') {
			pwm.setPWM(STEERING, STEERING_STRAIGHT);
			pwm.setPWM(ESC, ESC_N);
			status = 'N';
		    }
		    else if (status == 'N') {
			sleep(1);
			pwm.setPWM(ESC, ESC_R+35);
			status = 'R';
		    }
		    else if (status == 'R') {
			pwm.setPWM(ESC, ESC_N);
			status = 'N';
		    }
		}
		else if (key == 'a') {
			pwm.setPWM(STEERING, STEERING_LEFT);
		}
		else if (key == 'd') {
			pwm.setPWM(STEERING, STEERING_RIGHT);
		}
		printf("Status: %c\n", status);
	}

	/** Set motors to rest */
	pwm.setPWM(STEERING, 0);
	pwm.setPWM(ESC, 0);
	pwm.reset();
	return 0;
}
