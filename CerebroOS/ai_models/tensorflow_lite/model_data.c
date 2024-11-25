// This file contains the TensorFlow Lite model data as a C array.
const unsigned char model_tflite[] = {
    0x1C, 0x00, 0x00, 0x00, 0x54, 0x46, 0x4C, 0x33, 0x00, 0x00, 0x0E, 0x00,
    0x18, 0x00, 0x04, 0x00, 0x08, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x14, 0x00,
    0x0E, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ... (more model data)
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
    0xA5, 0xB4, 0xC3, 0xD2, 0xE1, 0xF0, 0x1E, 0x2D,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Get the size of the model array
const unsigned int model_tflite_len = sizeof(model_tflite);