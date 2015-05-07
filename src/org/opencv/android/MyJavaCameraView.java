package org.opencv.android;

import java.util.List;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ViewGroup.LayoutParams;
import org.opencv.core.Core;

import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

/**
 * This class is an implementation of the Bridge View between OpenCV and Java Camera.
 * This class relays on the functionality available in base class and only implements
 * required functions:
 * connectCamera - opens Java camera and sets the PreviewCallback to be delivered.
 * disconnectCamera - closes the camera and stops preview.
 * When frame is delivered via callback from Camera - it processed via OpenCV to be
 * converted to RGBA32 and then passed to the external callback for modifications if required.
 */
public class MyJavaCameraView extends MyCameraBridgeViewBase implements PreviewCallback {

    private static final int MAGIC_TEXTURE_ID = 10;
    private static final String TAG = "MyJavaCameraView";

    private byte mBuffer[];
    private Mat[] mFrameChain;
    private int mChainIdx = 0;
    private Thread mThread;
    private boolean mStopThread;

    protected Camera mCamera;
    protected MyJavaCameraView.JavaCameraFrame mCameraFrame;
    private SurfaceTexture mSurfaceTexture;

    public static class JavaCameraSizeAccessor implements MyCameraBridgeViewBase.ListItemAccessor {

        public int getWidth(Object obj) {
            Camera.Size size = (Camera.Size) obj;
            return size.width;
        }

        public int getHeight(Object obj) {
            Camera.Size size = (Camera.Size) obj;
            return size.height;
        }
    }

    public MyJavaCameraView(Context context, int cameraId) {
        super(context, cameraId);
    }

    public MyJavaCameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    protected boolean initializeCamera(int width, int height) {
        Log.d(TAG, "Initialize java camera");
        boolean result = true;
        synchronized (this) {
            mCamera = null;

            if (mCameraIndex == -1) {
                Log.d(TAG, "Trying to open camera with old open()");
                try {
                    mCamera = Camera.open();
                }
                catch (Exception e){
                    Log.e(TAG, "Camera is not available (in use or does not exist): " + e.getLocalizedMessage());
                }

                if(mCamera == null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
                    boolean connected = false;
                    for (int camIdx = 0; camIdx < Camera.getNumberOfCameras(); ++camIdx) {
                        Log.d(TAG, "Trying to open camera with new open(" + Integer.valueOf(camIdx) + ")");
                        try {
                            mCamera = Camera.open(camIdx);
                            connected = true;
                        } catch (RuntimeException e) {
                            Log.e(TAG, "Camera #" + camIdx + "failed to open: " + e.getLocalizedMessage());
                        }
                        if (connected) break;
                    }
                }
            } else {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD) {
                    Log.d(TAG, "Trying to open camera with new open(" + Integer.valueOf(mCameraIndex) + ")");
                    try {
                        mCamera = Camera.open(mCameraIndex);
                    } catch (RuntimeException e) {
                        Log.e(TAG, "Camera #" + mCameraIndex + "failed to open: " + e.getLocalizedMessage());
                    }
                }
            }

            if (mCamera == null)
                return false;

            /* Now set camera parameters */
            try {
                Camera.Parameters params = mCamera.getParameters();
                Log.d(TAG, "getSupportedPreviewSizes()");
                List<android.hardware.Camera.Size> sizes = params.getSupportedPreviewSizes();

                if (sizes != null) {
                    for (android.hardware.Camera.Size s : sizes) Log.d(TAG, s.width+"x"+s.height);

                    /* Select the size that fits surface considering maximum size allowed */
                    Size frameSize = calculateCameraFrameSize(sizes, new MyJavaCameraView.JavaCameraSizeAccessor(), width, height);

                    params.setPreviewFormat(ImageFormat.NV21);
                    Log.d(TAG, "Set preview size to " + Integer.valueOf((int)frameSize.width) + "x" + Integer.valueOf((int)frameSize.height));
                    params.setPreviewSize((int)frameSize.width, (int)frameSize.height);

                    List<String> FocusModes = params.getSupportedFocusModes();
                    if (FocusModes != null && FocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                    {
                        params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                    }

                    if (params.isVideoStabilizationSupported()) {
                        params.setVideoStabilization(true);
                    }

                    params.setRecordingHint(true);

                    mCamera.setParameters(params);
                    params = mCamera.getParameters();

                    mFrameWidth = params.getPreviewSize().width;
                    mFrameHeight = params.getPreviewSize().height;

                    int size = mFrameWidth * mFrameHeight;
                    size  = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
                    mBuffer = new byte[size];

                    mCamera.addCallbackBuffer(mBuffer);
                    mCamera.setPreviewCallbackWithBuffer(this);

                    mFrameChain = new Mat[2];
                    mFrameChain[0] = new Mat(mFrameHeight + (mFrameHeight/2), mFrameWidth, CvType.CV_8UC1);
                    mFrameChain[1] = new Mat(mFrameHeight + (mFrameHeight/2), mFrameWidth, CvType.CV_8UC1);

                    mCameraFrame = new MyJavaCameraView.JavaCameraFrame(mFrameChain[mChainIdx], mFrameWidth, mFrameHeight);

                    if(getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
                        mFrameWidth = params.getPreviewSize().height;
                        mFrameHeight = params.getPreviewSize().width;
                    }

                    AllocateCache();

                    if ((getLayoutParams().width == LayoutParams.MATCH_PARENT) && (getLayoutParams().height == LayoutParams.MATCH_PARENT)) {
                        mScale = Math.max(((float)width)/mFrameWidth, ((float)height)/mFrameHeight);
                    }

                    if (mFpsMeter != null) {
                        mFpsMeter.setResolution(mFrameWidth, mFrameHeight);
                    }

                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
                        mSurfaceTexture = new SurfaceTexture(MAGIC_TEXTURE_ID);
                        mCamera.setPreviewTexture(mSurfaceTexture);
                    } else
                       mCamera.setPreviewDisplay(null);

                    /* Finally we are ready to start the preview */
                    Log.d(TAG, "startPreview");
                    mCamera.startPreview();
                }
                else
                    result = false;
            } catch (Exception e) {
                result = false;
                e.printStackTrace();
            }
        }

        return result;
    }

    protected void releaseCamera() {
        synchronized (this) {
            if (mCamera != null) {
                mCamera.stopPreview();
                mCamera.release();
            }
            mCamera = null;
            if (mFrameChain != null) {
                mFrameChain[0].release();
                mFrameChain[1].release();
            }
            if (mCameraFrame != null)
                mCameraFrame.release();
        }
    }

    @Override
    protected boolean connectCamera(int width, int height) {

        /* 1. We need to instantiate camera
         * 2. We need to start thread which will be getting frames
         */
        /* First step - initialize camera connection */
        Log.d(TAG, "Connecting to camera");
        if (!initializeCamera(width, height))
            return false;

        /* now we can start update thread */
        Log.d(TAG, "Starting processing thread");
        mStopThread = false;
        mThread = new Thread(new MyJavaCameraView.CameraWorker());
        mThread.start();

        return true;
    }

    protected void disconnectCamera() {
        /* 1. We need to stop thread which updating the frames
         * 2. Stop camera and release it
         */
        Log.d(TAG, "Disconnecting from camera");
        try {
            mStopThread = true;
            Log.d(TAG, "Notify thread");
            synchronized (this) {
                this.notify();
            }
            Log.d(TAG, "Wating for thread");
            if (mThread != null)
                mThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            mThread =  null;
        }

        /* Now release camera */
        releaseCamera();
    }

    public void onPreviewFrame(byte[] frame, Camera arg1) {
        Log.d(TAG, "Preview Frame received. Frame size: " + frame.length);
        synchronized (this) {
            mFrameChain[1 - mChainIdx].put(0, 0, frame);
            this.notify();
        }
        if (mCamera != null)
            mCamera.addCallbackBuffer(mBuffer);
    }

    public class JavaCameraFrame implements MyCameraBridgeViewBase.CvCameraViewFrame {
        public Mat gray() {
            mGray = mYuvFrameData.submat(0, mHeight, 0, mWidth);
            return rotate(mGray);
        }

        public Mat rgba() {
            Imgproc.cvtColor(mYuvFrameData, mRgba, Imgproc.COLOR_YUV2BGR_NV12, 4);
            return rotate(mRgba);
        }

        public Mat rgb() {
            Imgproc.cvtColor(mYuvFrameData, mRgb, Imgproc.COLOR_YUV2BGR_NV12);
            return rotate(mRgb);
        }

        private Mat rotate(Mat frame) {
            Camera.CameraInfo info = new Camera.CameraInfo();
            Camera.getCameraInfo(mCameraIndex, info);

            // FIXME all this flipping and transposing is very performance expensive
            if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) {
                if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                    Core.flip(frame.t(), frame, -1);
                } else {
                    Core.flip(frame.t(), frame, 1);
                }
            } else {
                if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                    Core.flip(frame, frame, 1);
                }
            }
            return frame;
        }

        public JavaCameraFrame(Mat Yuv420sp, int width, int height) {
            super();
            mWidth = width;
            mHeight = height;
            mYuvFrameData = Yuv420sp;
            mRgba = new Mat();
            mRgb = new Mat();
            mGray = new Mat();
        }

        public void release() {
            mRgba.release();
            mRgb.release();
            mGray.release();
        }

        private JavaCameraFrame(MyCameraBridgeViewBase.CvCameraViewFrame obj) {
        }

        private Mat mYuvFrameData;
        private Mat mRgba;
        private Mat mRgb;
        private Mat mGray;
        private int mWidth;
        private int mHeight;
    };

    private class CameraWorker implements Runnable {

        public void run() {
            do {
                synchronized (MyJavaCameraView.this) {
                    try {
                        MyJavaCameraView.this.wait();
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

                if (!mStopThread) {
                    if (!mFrameChain[mChainIdx].empty())
                        deliverAndDrawFrame(mCameraFrame);
                    mChainIdx = 1 - mChainIdx;
                }
            } while (!mStopThread);
            Log.d(TAG, "Finish processing thread");
        }
    }
}
