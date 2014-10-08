package de.gummelinformatics.mui;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.os.Bundle;
import android.app.Activity;

public class MuiActivity extends QtActivity 
{
        public MuiActivity()
	{
            //java.lang.RuntimeException: Unable to instantiate activity ComponentInfo{de.gummelinformatics.mui/de.gummelinformatics.mui.MuiActivity}: java.lang.NullPointerException
//                setKeepScreenOn(true);
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setKeepScreenOn(true);
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();

	}

	public void setKeepScreenOn(boolean keepScreenOn) {
            if(keepScreenOn) {
                this.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            } else {
		this.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            }
        }
}

