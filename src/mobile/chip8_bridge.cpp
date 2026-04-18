#include "../core/chip8.h"
#include<jni.h>

chip8 emulator;

extern "C" {
JNIEXPORT void JNICALL
Java_com_picklemul_Chip8Bridge_init() {
    emulator.init();
    emulator.reset();
}

JNIEXPORT void JNICALL
Java_com_picklemul_Chip8Bridge_loadRom(JNIEnv *env, jobject thiz, jbyteArray rom_data) {
    jsize length = env->GetArrayLength(rom_data);

    // Safety check: CHIP-8 workspace is 3584 bytes max
    if (length > (4096 - 0x200)) return;

    // Lock the Java array in memory and get a C++ pointer to it
    jbyte *buffer = env->GetByteArrayElements(rom_data, nullptr);

    // Copy the data into the CHIP-8 memory starting at 0x200
    for (int i = 0; i < length; i++) {
        emulator.main_memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
    }

    // Release the Java array to prevent memory leaks (JNI_ABORT means we didn't modify it)
    env->ReleaseByteArrayElements(rom_data, buffer, JNI_ABORT);
}

JNIEXPORT void JNICALL
Java_com_picklemul_Chip8Bridge_execCycle(JNIEnv *env, jobject thiz) {
    emulator.exec();
}

JNIEXPORT void JNICALL
Java_com_picklemul_Chip8Bridge_updateTimers(JNIEnv *env, jobject thiz) {
    emulator.update_timers();
}

JNIEXPORT void JNICALL
Java_com_picklemul_Chip8Bridge_setKey(JNIEnv *env, jobject thiz, jint key_index, jboolean is_pressed) {
    emulator.set_key(static_cast<uint8_t>(key_index), is_pressed);
}

JNIEXPORT jbyteArray JNICALL
Java_com_picklemul_Chip8Bridge_getDisplayBuffer(JNIEnv *env, jobject thiz) {
    jbyteArray result = env->NewByteArray(2048);
    env->SetByteArrayRegion(result, 0, 2048, reinterpret_cast<const jbyte *>(emulator.display_buffer));
    return result;
}
}
