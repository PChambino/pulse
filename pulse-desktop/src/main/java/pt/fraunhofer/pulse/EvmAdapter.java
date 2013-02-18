package pt.fraunhofer.pulse;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.IVideoPictureEvent;
import com.xuggle.xuggler.IVideoPicture;
import java.awt.image.BufferedImage;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class EvmAdapter extends MediaToolAdapter {

    static {
        System.loadLibrary("opencv_java");
    }

    private Evm evm = new Evm("res/lbpcascade_frontalface.xml");
    private boolean initialize = true;

    private byte[] data;
    private Mat matData;
    private Mat frame = new Mat();

    @Override
    public void onVideoPicture(IVideoPictureEvent event) {
        IVideoPicture p = event.getPicture();
        if (initialize) {
            data = new byte[p.getSize()];
            matData = new Mat(p.getHeight() + p.getHeight() / 2, p.getWidth(), CvType.CV_8UC1);
        }

        p.get(0, data, 0, data.length);
        matData.put(0, 0, data);
        Imgproc.cvtColor(matData, frame, Imgproc.COLOR_YUV420p2RGBA, 4);

        if (initialize) {
            evm.start(frame.width(), frame.height());
            initialize = false;
        }

        frame = evm.onFrame(frame);

        BufferedImage i = event.getImage();
        for (int y = 0; y < frame.rows(); y++) {
            for (int x = 0; x < frame.cols(); x++) {
                double[] pixel = frame.get(y, x);
                i.setRGB(x, y,
                        (int)pixel[3] << 24 |
                        (int)pixel[2] << 16 |
                        (int)pixel[1] << 8  |
                        (int)pixel[0]);
            }
        }

        super.onVideoPicture(event);
    }

}
