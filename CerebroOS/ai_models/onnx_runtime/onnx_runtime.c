#include "onnx_runtime.h"
#include <onnxruntime_c_api.h>
#include <stdio.h>
#include <stdlib.h>

static OrtEnv* env;
static OrtSession* session;
static OrtMemoryInfo* memory_info;

void init_onnx_model() {
    OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "test", &env);

    OrtSessionOptions* session_options;
    g_ort->CreateSessionOptions(&session_options);
    g_ort->SetIntraOpNumThreads(session_options, 1);
    g_ort->SetGraphOptimizationLevel(session_options, ORT_ENABLE_BASIC);

    g_ort->CreateMemoryInfo(ORT_MEMORY_INFO_DEFAULT, OrtArenaAllocator, 0, &memory_info);

    const char* model_path = "model.onnx";
    g_ort->CreateSession(env, model_path, session_options, &session);

    g_ort->ReleaseSessionOptions(session_options);
}

float run_onnx_inference(float input_value) {
    OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    OrtValue* input_tensor = NULL;
    float input_data[] = {input_value};
    g_ort->CreateTensorWithDataAsOrtValue(memory_info, input_data, sizeof(input_data), (int64_t[]){1}, 1, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &input_tensor);

    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};
    OrtValue* output_tensor = NULL;

    g_ort->Run(session, NULL, input_names, (const OrtValue* const*)&input_tensor, 1, output_names, 1, &output_tensor);

    float* output_data;
    g_ort->GetTensorMutableData(output_tensor, (void**)&output_data);
    float result = output_data[0];

    g_ort->ReleaseValue(input_tensor);
    g_ort->ReleaseValue(output_tensor);

    return result;
}
