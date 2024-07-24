#ifndef TENSORFLOW_LITE_H
#define TENSORFLOW_LITE_H

#include "tensorflow/lite/c/common.h"

void init_tflite_model();
float run_tflite_inference(float input);

#endif // TENSORFLOW_LITE_H
