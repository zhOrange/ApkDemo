//
// Created by zh on 20-6-3.
//

#include <jni.h>
#include <string>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <android/log.h>
#include <android/bitmap.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define ASSERT(status, ret)     if (!(status)) { return ret; }
#define ASSERT_FALSE(status)    ASSERT(status, false)

#define  LOG_TAG    "NATIVE_LOG"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
    /**
     * 根据输入尺寸,生成一个ARGB_8888格式的bitmap对象,
     * @param env
     * @param width 宽度
     * @param height 高度
     * @return bitmap对象.
     */
    jobject createBitmap(JNIEnv *env, uint32_t width, uint32_t height) {

        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
            bitmapConfigClass, "valueOf",
            "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction, configName);

        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    width,
                                                    height, bitmapConfig);
        return newBitmap;
}


    bool BitmapToMatrix(JNIEnv * env, jobject obj_bitmap, cv::Mat & matrix) {
        void * bitmapPixels;                                            // 保存图片像素数据
        AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

        ASSERT_FALSE( AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数
        ASSERT_FALSE( bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                      || bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565 );          // 只支持 ARGB_8888 和 RGB_565
        ASSERT_FALSE( AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0 );  // 获取图片像素（锁定内存块）
        ASSERT_FALSE( bitmapPixels );

        if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);    // 建立临时 mat
            tmp.copyTo(matrix);                                                         // 拷贝到目标 matrix
        } else {
            cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
            cv::cvtColor(tmp, matrix, cv::COLOR_BGR5652RGB);
        }

        AndroidBitmap_unlockPixels(env, obj_bitmap);            // 解锁
        return true;
    }



    bool MatrixToBitmap(JNIEnv * env, cv::Mat & matrix, jobject obj_bitmap) {
        void * bitmapPixels;                                            // 保存图片像素数据
        AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

        ASSERT_FALSE( AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // 获取图片参数

        ASSERT_FALSE( bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                      || bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565 );          // 只支持 ARGB_8888 和 RGB_565
        ASSERT_FALSE( matrix.dims == 2
                      && bitmapInfo.height == (uint32_t)matrix.rows
                      && bitmapInfo.width == (uint32_t)matrix.cols );                   // 必须是 2 维矩阵，长宽一致
        ASSERT_FALSE( matrix.type() == CV_8UC1 || matrix.type() == CV_8UC3 || matrix.type() == CV_8UC4 );
        ASSERT_FALSE( AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0 );  // 获取图片像素（锁定内存块）
        ASSERT_FALSE( bitmapPixels );

        if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);
            switch (matrix.type()) {
                case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2RGBA);     break;
                case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2RGBA);      break;
                case CV_8UC4:   matrix.copyTo(tmp);                                 break;
                default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
            }
        } else {
            cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
            switch (matrix.type()) {
                case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2BGR565);   break;
                case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2BGR565);    break;
                case CV_8UC4:   cv::cvtColor(matrix, tmp, cv::COLOR_RGBA2BGR565);   break;
                default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
            }
        }
        AndroidBitmap_unlockPixels(env, obj_bitmap);                // 解锁
        return true;
    }

	// 注意这里的函数名格式：Java_各级包名_类名_函数名(参数...),需严格按照这种格式，否则会出错
	JNIEXPORT jstring JNICALL Java_com_zh_jninativedemo_NativeFunc_getStringFromJNI(
	        JNIEnv* env,
	        jobject Instance /* this */) {
	    std::string hello = "Hello from C++";
        LOGD("log info, the int value is %d.", 32);
	    return env->NewStringUTF(hello.c_str());
	}

	/**
	 * JNI返回一个Java中的对象,由Java传入一个对象,由JNI进行赋值后,返回.
	 * @param env
	 * @param Instance
	 * @param person 传入java对象
	 * @return 返回计算后的java对象.
	 */
	JNIEXPORT jobject JNICALL Java_com_zh_jninativedemo_NativeFunc_getNativePerson(
            JNIEnv* env,
            jobject Instance,
            jobject person/* this */){
        jclass personClass = env->FindClass("com/zh/jninativedemo/Person");

        jfieldID name = env->GetFieldID(personClass, "name", "Ljava/lang/String;");
        jfieldID age = env->GetFieldID(personClass, "age", "I");

        env->SetObjectField(person, name, env->NewStringUTF("peek"));
        env->SetIntField(person, age, 20);
        return person;
    }

    /**
     * JNI返回一个Java中的对象.直接由Jni生成一个Java对象,赋值后返回.
     * @param env
     * @param Instance
     * @return Java对象.
     */
    JNIEXPORT jobject JNICALL Java_com_zh_jninativedemo_NativeFunc_getNativePerson2(
            JNIEnv* env,
            jobject Instance /* this */){

        jclass personClass = env->FindClass("com/zh/jninativedemo/Person");

        jmethodID id = env->GetMethodID(personClass, "<init>", "()V");

        jobject person = env->NewObject(personClass, id);

        jfieldID name = env->GetFieldID(personClass, "name", "Ljava/lang/String;");
        jfieldID age = env->GetFieldID(personClass, "age", "I");

        env->SetObjectField(person, name, env->NewStringUTF("peek"));
        env->SetIntField(person, age, 20);
        return person;
    }

}
