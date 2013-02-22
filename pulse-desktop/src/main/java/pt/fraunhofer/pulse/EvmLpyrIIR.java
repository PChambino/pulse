package pt.fraunhofer.pulse;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

public class EvmLpyrIIR extends Evm {

    public EvmLpyrIIR() {
        super();
    }

    public EvmLpyrIIR(String filename) {
        super(filename);
    }

    private static final double MIN_FACE_SIZE   = 0.3;
    private static final double F_LOW           = 45/60.0/10;
    private static final double F_HIGH          = 240/60.0/10;
    private static final double CUTOFF          = 30;
    private static final Scalar ALPHA           = Scalar.all(20);

    private int pyrLevel;

    private double initialLambdaCutoff;
    private double cutoffDelta;

    private int t;
    private Point point;
    private Size minFaceSize;
    private Size maxFaceSize;
    private MatOfRect faces;
    private Mat gray;
    private Mat blurred;
    private Mat output;
    private Mat outputFloat;
    private Mat frameFloat;
    private Mat buildLpyrAux;
    private Mat[] lowpassHigh;
    private Mat[] lowpassLow;
    private Mat[] lowpassHighAux;
    private Mat[] lowpassLowAux;
    private Mat[] pyr;

    @Override
    public void start(int width, int height) {
        // max pyramid level (5 is kernel size used for filter)
        pyrLevel = (int) (Math.log(Math.min(width, height) / 5.0) / Math.log(2));

        // expressions from EVM_Matlab demo
        initialLambdaCutoff = Math.sqrt(width * width + height * height) / 3.0;
        cutoffDelta = CUTOFF / 8.0 / (1 + ALPHA.val[0]);

        t = 0;
        point = new Point();
        minFaceSize = new Size(MIN_FACE_SIZE * width, MIN_FACE_SIZE * height);
        maxFaceSize = new Size();
        faces = new MatOfRect();
        gray = new Mat();
        blurred = new Mat();
        output = new Mat();
        outputFloat = new Mat();
        frameFloat = new Mat();
        buildLpyrAux = new Mat();
        lowpassHigh = new Mat[pyrLevel + 1];
        lowpassLow = new Mat[pyrLevel + 1];
        lowpassHighAux = new Mat[pyrLevel + 1];
        lowpassLowAux = new Mat[pyrLevel + 1];
        pyr = new Mat[pyrLevel + 1];
        for (int i = 0; i < pyr.length; i++) {
            lowpassHigh[i] = new Mat();
            lowpassLow[i] = new Mat();
            lowpassHighAux[i] = new Mat();
            lowpassLowAux[i] = new Mat();
            pyr[i] = new Mat();
        }
    }

    @Override
    public void stop() {
        t = 0;
        faces.release();
        gray.release();
        blurred.release();
        output.release();
        outputFloat.release();
        frameFloat.release();
        buildLpyrAux.release();
        for (int i = 0; i < pyr.length; i++) {
            lowpassHigh[i].release();
            lowpassLow[i].release();
            lowpassHighAux[i].release();
            lowpassLowAux[i].release();
            pyr[i].release();
        }
        lowpassHigh = null;
        lowpassLow = null;
        lowpassHighAux = null;
        lowpassLowAux = null;
        pyr = null;
    }

    @Override
    public Mat onFrame(Mat frame) {
        // face detection
        Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGB2GRAY);
        faceDetector.detectMultiScale(gray, faces, 1.1, 2, 0, minFaceSize, maxFaceSize);

        // convert to float and remove alpha channel
        frame.convertTo(frameFloat, CvType.CV_32F);
        Imgproc.cvtColor(frameFloat, frameFloat, Imgproc.COLOR_RGBA2RGB);

        // apply spatial filter: Laplacian pyramid
        buildLpyr(frameFloat, pyr, pyrLevel);

        // apply temporal filter: substraction of two IIR lowpass filters
        if (0 == t) {
            for (int i = 0; i < pyr.length; i++) {
                pyr[i].copyTo(lowpassHigh[i]);
                pyr[i].copyTo(lowpassLow[i]);
            }

            frame.copyTo(output);
        } else {
            double lambda = initialLambdaCutoff;

            for (int i = pyr.length - 1; i > 0; i--) {
                Core.multiply(lowpassHigh[i], Scalar.all(1-F_HIGH), lowpassHigh[i]);
                Core.multiply(pyr[i], Scalar.all(F_HIGH), lowpassHighAux[i]);
                Core.add(lowpassHigh[i], lowpassHighAux[i], lowpassHigh[i]);

                Core.multiply(lowpassLow[i], Scalar.all(1-F_LOW), lowpassLow[i]);
                Core.multiply(pyr[i], Scalar.all(F_LOW), lowpassLowAux[i]);
                Core.add(lowpassLow[i], lowpassLowAux[i], lowpassLow[i]);

                Core.subtract(lowpassHigh[i], lowpassLow[i], pyr[i]);

                // amplify
                double alpha = lambda / cutoffDelta / 8 - 1;
                alpha *= 2; // exaggeration for better visualization
                if (0 == i || pyr.length - 1 == i) {
                    Core.multiply(pyr[i], ZERO, pyr[i]);
                } else if (alpha > ALPHA.val[0]) {
                    Core.multiply(pyr[i], ALPHA, pyr[i]);
                } else {
                    Core.multiply(pyr[i], ZERO, pyr[i]);
                }
                lambda /= 2.0;
            }

            // rebuild frame from Laplacian pyramid
            reconLpyr(pyr, outputFloat);

            // add back to original frame
            Core.add(frameFloat, outputFloat, outputFloat);

            // add back alpha channel and convert to 8 bit
            Imgproc.cvtColor(outputFloat, outputFloat, Imgproc.COLOR_RGB2RGBA);
            outputFloat.convertTo(output, CvType.CV_8U);
        }

        // draw some rectangles and points
        for (Rect face : faces.toArray()) {
            Core.rectangle(output, face.tl(), face.br(), BLUE, 4);

            // forehead point
            point.x = face.tl().x + face.size().width * 0.5;
            point.y = face.tl().y + face.size().width * 0.2;
            point(output, point);

            // left cheek point
            point.x = face.tl().x + face.size().width * 0.7;
            point.y = face.tl().y + face.size().width * 0.6;
            point(output, point);

            // right cheek point
            point.x = face.tl().x + face.size().width * 0.3;
            point(output, point);
        }

        t++;

        return output;
    }

    private void buildLpyr(Mat a, Mat[] pyr, int level) {
        a.copyTo(pyr[0]);
        for (int i = 0; i < level; i++) {
            Imgproc.pyrDown(pyr[i], pyr[i+1]);
            Imgproc.pyrUp(pyr[i+1], buildLpyrAux);
            Imgproc.resize(buildLpyrAux, buildLpyrAux, pyr[i].size());
            Core.subtract(pyr[i], buildLpyrAux, pyr[i]);
        }
    }

    private void reconLpyr(Mat[] pyr, Mat o) {
        pyr[pyr.length - 1].copyTo(o);
        for (int i = pyr.length - 1; i > 0; i--) {
            Imgproc.pyrUp(o, o);
            Imgproc.resize(o, o, pyr[i-1].size());
            Core.add(pyr[i-1], o, o);
        }
    }

}
