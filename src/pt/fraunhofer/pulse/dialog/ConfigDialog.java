package pt.fraunhofer.pulse.dialog;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import org.opencv.android.MyCameraBridgeViewBase;
import pt.fraunhofer.pulse.App;
import pt.fraunhofer.pulse.Pulse;
import pt.fraunhofer.pulse.R;

public class ConfigDialog extends DialogFragment {

    private MyCameraBridgeViewBase camera;
    private Pulse pulse;

    private Switch magnificationSwitch;
    private SeekBar magnificationSeekBar;
    private Switch fpsSwitch;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(R.string.config);
        builder.setNeutralButton(R.string.done, null);

        View dialogView = getActivity().getLayoutInflater().inflate(R.layout.config, null);
        builder.setView(dialogView);

        magnificationSwitch = ((Switch)dialogView.findViewById(R.id.magnification));
        magnificationSwitch.setOnCheckedChangeListener(new MagnificationSwitchConfig());

        magnificationSeekBar = ((SeekBar)dialogView.findViewById(R.id.magnificationFactor));
        magnificationSeekBar.setOnSeekBarChangeListener(new MagnificationSeekBarConfig());

        fpsSwitch = ((Switch)dialogView.findViewById(R.id.fps));
        fpsSwitch.setOnCheckedChangeListener(new FpsSwitchConfig());

        magnificationSwitch.setChecked(pulse.hasMagnification());
        magnificationSeekBar.setEnabled(pulse.hasMagnification());
        magnificationSeekBar.setProgress(pulse.getMagnificationFactor());
        fpsSwitch.setChecked(camera.isFpsMeterEnabled());

        return builder.create();
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        App app = (App)activity;
        camera = app.getCamera();
        pulse = app.getPulse();
    }

    private class MagnificationSwitchConfig implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            magnificationSeekBar.setEnabled(isChecked);
            pulse.setMagnification(isChecked);
        }
    }

    private class MagnificationSeekBarConfig implements SeekBar.OnSeekBarChangeListener {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            pulse.setMagnificationFactor(progress);
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
        }
    }

    private class FpsSwitchConfig implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            camera.setFpsMeter(isChecked);
        }
    }
}
