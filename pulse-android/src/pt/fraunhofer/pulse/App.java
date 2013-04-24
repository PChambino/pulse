package pt.fraunhofer.pulse;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.MyCameraBridgeViewBase;
import org.opencv.android.MyCameraBridgeViewBase.CvCameraViewListener;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.highgui.Highgui;
import pt.fraunhofer.pulse.Pulse.Face;
import pt.fraunhofer.pulse.view.BpmView;
import pt.fraunhofer.pulse.view.PulseView;

public class App extends Activity implements CvCameraViewListener {

    private static final String TAG = "Pulse::App";

    private MyCameraBridgeViewBase camera;
    private BpmView bpmView;
    private PulseView pulseView;
    private Pulse pulse;
    
    private Paint faceBoxPaint;
    
    private BaseLoaderCallback loaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {

                case LoaderCallbackInterface.SUCCESS:
                    loaderCallbackSuccess();
                    break;

                default:
                    super.onManagerConnected(status);
                    break;
            }
        }
    };

    private void loaderCallbackSuccess() {
        System.loadLibrary("pulse");
        
        pulse = new Pulse();
        
        File dir = getDir("cascade", Context.MODE_PRIVATE);
        File file = createFileFromResource(dir, R.raw.lbpcascade_frontalface, "xml");
        pulse.load(file.getAbsolutePath());
        dir.delete();
        
        pulseView.setGridSize(pulse.getMaxSignalSize());

        camera.enableView();
    }
    
    private File createFileFromResource(File dir, int id, String extension) {
        String name = getResources().getResourceEntryName(id) + "." + extension;
        InputStream is = getResources().openRawResource(id);
        File file = new File(dir, name);

        try {
            FileOutputStream os = new FileOutputStream(file);

            byte[] buffer = new byte[4096];
            int bytesRead;
            while ((bytesRead = is.read(buffer)) != -1) {
                os.write(buffer, 0, bytesRead);
            }
            is.close();
            os.close();
        } catch (IOException ex) {
            Log.e(TAG, "Failed to create file: " + file.getPath(), ex);
        }

        return file;
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.app);

        camera = (MyCameraBridgeViewBase) findViewById(R.id.camera);
        camera.setCvCameraViewListener(this);
        camera.SetCaptureFormat(Highgui.CV_CAP_ANDROID_COLOR_FRAME_RGB);
        camera.setMaxFrameSize(650, 650);
        
        bpmView = (BpmView) findViewById(R.id.bpm);
        pulseView = (PulseView) findViewById(R.id.pulse);
        
        faceBoxPaint = initFaceBoxPaint();
    }

    @Override
    public void onResume() {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_5, this, loaderCallback);
    }

    @Override
    public void onPause() {
        if (camera != null) {
            camera.disableView();
        }
        super.onPause();
    }

    @Override
    public void onDestroy() {
        if (camera != null) {
            camera.disableView();
        }
        super.onDestroy();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        Log.d(TAG, "onCameraViewStarted("+width+", "+height+")");
        pulse.start(width, height);
    }

    @Override
    public void onCameraViewStopped() {
    }

    @Override
    public Mat onCameraFrame(Mat frame) {
        pulse.onFrame(frame);
        return frame;
    }

    @Override
    public void onCameraFrame(Canvas canvas) {
        Face[] faces = pulse.getFaces();
        if (faces.length > 0) {
            onFace(canvas, faces[0]); // TODO support multiple faces
        } else {
            // no faces
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    bpmView.setNoBpm();
                    pulseView.setNoPulse();
                }
            });
        }
    }
        
    private void onFace(Canvas canvas, Face face) {
        // draw face box
        canvas.drawPath(createFaceBoxPath(face.getBox()), faceBoxPaint);
        
        // update views
        if (face.existsPulse()) {
            final double bpm = face.getBpm();
            final double[] signal = face.getPulse();
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    bpmView.setBpm(bpm);
                    pulseView.setPulse(signal);
                }
            });
        } else {
            // no pulse
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    bpmView.setNoBpm();
                    pulseView.setNoPulse();
                }
            });
        }
    }
    
    private Paint initFaceBoxPaint() {
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        p.setColor(Color.WHITE);
        p.setStyle(Paint.Style.STROKE);
        p.setStrokeWidth(4);
        p.setStrokeCap(Paint.Cap.ROUND);
        p.setStrokeJoin(Paint.Join.ROUND);
        return p;
    }
    
    private Path createFaceBoxPath(Rect box) {
        float size = box.width * 0.25f;
        Path path = new Path();
        // top left
        path.moveTo(box.x, box.y + size);
        path.lineTo(box.x, box.y);
        path.lineTo(box.x + size, box.y);
        // top right
        path.moveTo(box.x + box.width, box.y + size);
        path.lineTo(box.x + box.width, box.y);
        path.lineTo(box.x + box.width - size, box.y);
        // bottom left
        path.moveTo(box.x, box.y + box.height - size);
        path.lineTo(box.x, box.y + box.height);
        path.lineTo(box.x + size, box.y + box.height);
        // bottom right
        path.moveTo(box.x + box.width, box.y + box.height - size);
        path.lineTo(box.x + box.width, box.y + box.height);
        path.lineTo(box.x + box.width - size, box.y + box.height);
        return path;
    }
}
