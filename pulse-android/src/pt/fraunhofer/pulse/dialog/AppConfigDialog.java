package pt.fraunhofer.pulse.dialog;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;
import pt.fraunhofer.pulse.Pulse;
import pt.fraunhofer.pulse.R;

public class AppConfigDialog extends DialogFragment {

    private Pulse pulse;

    public AppConfigDialog(Pulse pulse) {
        this.pulse = pulse;
    }

    private Switch magnificationView;
    private SeekBar magnificationFactorView;
        
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(R.string.config);
        View dialogView = getActivity().getLayoutInflater().inflate(R.layout.config, null);
        builder.setView(dialogView);

        magnificationView = ((Switch)dialogView.findViewById(R.id.magnification));
        magnificationView.setChecked(pulse.hasMagnification());
        magnificationView.setOnCheckedChangeListener(new MagnificationViewConfig());

        magnificationFactorView = ((SeekBar)dialogView.findViewById(R.id.magnificationFactor));
        magnificationFactorView.setProgress(pulse.getMagnificationFactor());
        magnificationFactorView.setOnSeekBarChangeListener(new MagnificationFactorViewConfig());
        
        return builder.create();
    }
    
    private class MagnificationViewConfig implements CompoundButton.OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            magnificationFactorView.setEnabled(isChecked);
            pulse.setMagnification(isChecked);
        }
    }
    
    private class MagnificationFactorViewConfig implements SeekBar.OnSeekBarChangeListener {
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
    
}
