package com.picklemulmobile;

public class Chip8Bridge {
    static {
        System.loadLibrary("chip8_bridge");
    }

    public native void init();
    public native void loadRom(byte[] romData);
    public native void execCycle();
    public native void updateTimers();
    public native void setKey(int keyIndex, boolean isPressed);
    public native byte[] getDisplayBuffer();
}
