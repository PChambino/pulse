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
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

public class App extends Activity implements CvCameraViewListener {

    private static final String TAG = "Pulse::App";

    private CameraBridgeViewBase camera;
    private CascadeClassifier faceDetector;
    
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
        File dir = getDir("cascade", Context.MODE_PRIVATE);
        File file = createFileFromResource(dir, R.raw.lbpcascade_frontalface, "xml");
        
        faceDetector = new CascadeClassifier(file.getAbsolutePath());
        if (faceDetector.empty()) {
            Log.e(TAG, "Failed to load cascade classifier");
            faceDetector = null;
        }

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

        camera = (CameraBridgeViewBase) findViewById(R.id.camera);
        camera.setCvCameraViewListener(this);
    }

    @Override
    public void onResume() {
        super.onResume();
        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, loaderCallback);
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

    private static final Scalar RED = new Scalar(255, 0, 0, 255);
    private static final Scalar BLUE = new Scalar(0, 0, 255, 255);

    private static final float minFaceSizePercentage = 0.3f;

    private Mat gray;
    private Mat output;
    private Point center;
    private Size minFaceSize;
    private Size maxFaceSize;

    @Override
    public void onCameraViewStarted(int width, int height) {
        gray = new Mat();
        output = new Mat();
        center = new Point();
        minFaceSize = new Size(minFaceSizePercentage * width, minFaceSizePercentage * height);
        maxFaceSize = new Size();
    }

    @Override
    public void onCameraViewStopped() {
        gray.release();
        output.release();
    }

    @Override
    public Mat onCameraFrame(Mat frame) {
        MatOfRect faces = new MatOfRect();
        
        if (faceDetector != null) {
            Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGB2GRAY);
            faceDetector.detectMultiScale(gray, faces, 1.1, 2, 0, minFaceSize, maxFaceSize);
        }

        frame.copyTo(output);

        for (Rect face : faces.toArray()) {
            Core.rectangle(output, face.tl(), face.br(), RED, 4);
            
            // forehead point
            center.x = face.tl().x + face.size().width * 0.5;
            center.y = face.tl().y + face.size().width * 0.2;
            Core.circle(output, center, 4, BLUE, 8);

            // left cheek point
            center.x = face.tl().x + face.size().width * 0.7;
            center.y = face.tl().y + face.size().width * 0.6;
            Core.circle(output, center, 4, BLUE, 8);

            // right cheek point
            center.x = face.tl().x + face.size().width * 0.3;
            Core.circle(output, center, 4, BLUE, 8);
        }

        return output;
    }

}
