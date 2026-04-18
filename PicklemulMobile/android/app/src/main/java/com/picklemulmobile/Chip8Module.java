package com.picklemulmobile;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.WritableArray;
import com.facebook.react.bridge.Arguments;

public class Chip8Module extends ReactContextBaseJavaModule {
    private final Chip8Bridge emulator;

    Chip8Module(ReactApplicationContext context) {
        super(context);
        emulator = new Chip8Bridge();
    }

    @NonNull
    @Override
    public String getName() {
        return "Chip8Native";
    }

    // FIX: Changed from void to boolean
    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean init() {
        emulator.init();
        return true;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean execCycle() {
        emulator.execCycle();
        return true;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean loadRom(com.facebook.react.bridge.ReadableArray romData) {
        // Convert JS array to Java byte array
        byte[] bytes = new byte[romData.size()];
        for (int i = 0; i < romData.size(); i++) {
            bytes[i] = (byte) romData.getInt(i);
        }

        // Pass to your C++ bridge
        emulator.loadRom(bytes);
        return true;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean updateTimers() {
        emulator.updateTimers();
        return true;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean setKey(int keyIndex, boolean isPressed) {
        emulator.setKey(keyIndex, isPressed);
        return true;
    }

    // This one was already fine because it returns a WritableArray!
    @ReactMethod(isBlockingSynchronousMethod = true)
    public WritableArray getDisplayBuffer() {
        byte[] rawBuffer = emulator.getDisplayBuffer();
        WritableArray jsArray = Arguments.createArray();

        for (byte b : rawBuffer) {
            jsArray.pushInt(b);
        }
        return jsArray;
    }
}