package com.zh.jninativedemo;

public class NativeFunc {

    static {
        System.loadLibrary("native-lib");
    }

    public static native String getStringFromJNI();
    public static native Person getNativePerson(Person person);
    public static native Person getNativePerson2();
}
