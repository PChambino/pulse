package pt.fraunhofer.pulse;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.IVideoPictureEvent;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt4;

public class EvmAdapter extends MediaToolAdapter {

    private Evm evm;

    @Override
    public void onVideoPicture(IVideoPictureEvent event) {
        super.onVideoPicture(event);
    }

}
