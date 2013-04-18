package pt.fraunhofer.pulse;

import android.app.Activity;
import android.content.Context;
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
import org.opencv.highgui.Highgui;
import pt.fraunhofer.pulse.Pulse.Face;
import pt.fraunhofer.pulse.view.BpmView;

public class App extends Activity implements CvCameraViewListener {

    private static final String TAG = "Pulse::App";

    private MyCameraBridgeViewBase camera;
    private BpmView bpm;
    private Pulse pulse;
    
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
        
        bpm = (BpmView) findViewById(R.id.bpm);
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
        for (final Face face : pulse.getFaces()) {
            bpm.setBpm(face.getBpm());
//            System.out.println("Face "+face.getId()+": "+face.getBpm());
//            System.out.println(face.getBox());
//            System.out.println(face.getPulse());
        }
        return frame;
    }

}
