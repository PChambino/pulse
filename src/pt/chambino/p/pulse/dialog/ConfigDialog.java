package pt.chambino.p.pulse.dialog;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import pt.chambino.p.pulse.App;
import pt.chambino.p.pulse.Pulse;
import pt.chambino.p.pulse.R;

public class ConfigDialog extends DialogFragment {

    private Switch faceDetectionSwitch;
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

        faceDetectionSwitch = ((Switch)dialogView.findViewById(R.id.face_detection));
        faceDetectionSwitch.setOnCheckedChangeListener(new FaceDetectionSwitchConfig());

        magnificationSwitch = ((Switch)dialogView.findViewById(R.id.magnification));
        magnificationSwitch.setOnCheckedChangeListener(new MagnificationSwitchConfig());

        magnificationSeekBar = ((SeekBar)dialogView.findViewById(R.id.magnificationFactor));
        magnificationSeekBar.setOnSeekBarChangeListener(new MagnificationSeekBarConfig());

        fpsSwitch = ((Switch)dialogView.findViewById(R.id.fps));
        fpsSwitch.setOnCheckedChangeListener(new FpsSwitchConfig());

        Pulse pulse = getApp().getPulse();
        // on configuration change Pulse may not have loaded yet
        if (pulse != null) {
            faceDetectionSwitch.setChecked(pulse.hasFaceDetection());
            magnificationSwitch.setChecked(pulse.hasMagnification());
            magnificationSeekBar.setEnabled(pulse.hasMagnification());
            magnificationSeekBar.setProgress(pulse.getMagnificationFactor());
        }
        fpsSwitch.setChecked(getApp().getCamera().isFpsMeterEnabled());

        return builder.create();
    }

    private App getApp() {
        return (App)getActivity();
    }

    private class FaceDetectionSwitchConfig implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            if (getApp().getPulse() != null) getApp().getPulse().setFaceDetection(isChecked);
        }
    }

    private class MagnificationSwitchConfig implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            magnificationSeekBar.setEnabled(isChecked);
            if (getApp().getPulse() != null) getApp().getPulse().setMagnification(isChecked);
        }
    }

    private class MagnificationSeekBarConfig implements SeekBar.OnSeekBarChangeListener {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (getApp().getPulse() != null) getApp().getPulse().setMagnificationFactor(progress);
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
            getApp().getCamera().setFpsMeter(isChecked);
        }
    }
}
