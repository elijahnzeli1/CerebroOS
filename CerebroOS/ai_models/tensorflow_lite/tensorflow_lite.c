#include "tensorflow_lite.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// External model data
extern const unsigned char model_tflite[];

const int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

static tflite::MicroInterpreter* interpreter;
static TfLiteTensor* input;
static TfLiteTensor* output;
static tflite::MicroErrorReporter micro_error_reporter;

void init_tflite_model() {
    tflite::InitializeTarget();

    const tflite::Model* model = tflite::GetModel(model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        micro_error_reporter.Report("Model provided is schema version %d not equal to supported version %d.",
                                    model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    static tflite::MicroMutableOpResolver<10> micro_op_resolver;
    interpreter = new tflite::MicroInterpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize, &micro_error_reporter);

    interpreter->AllocateTensors();

    input = interpreter->input(0);
    output = interpreter->output(0);
}

float run_tflite_inference(float input_value) {
    input->data.f[0] = input_value;

    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        micro_error_reporter.Report("Invoke failed.");
        return -1.0f;
    }

    return output->data.f[0];
}
