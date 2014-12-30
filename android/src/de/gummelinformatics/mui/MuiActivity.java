package de.gummelinformatics.mui;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;

public class MuiActivity extends QtActivity 
{
	private static MuiActivity myActivity;
        private static final String TAG = MuiActivity.class.getSimpleName();
        private static final boolean DEBUG = Config.DEBUG;

	public static MuiActivity getActivityInstance() {
		if ( DEBUG )
                    Log.d(TAG, "getActivityInstance() " + myActivity );


		return myActivity;
	}


	/* https://www.qtdeveloperdays.com/sites/default/files/BogdanVatra_Extending_Qt_Android_Apps_with_JNI.pdf */
        public MuiActivity()
	{
            //java.lang.RuntimeException: Unable to instantiate activity ComponentInfo{de.gummelinformatics.mui/de.gummelinformatics.mui.MuiActivity}: java.lang.NullPointerException
//                setKeepScreenOn(true);
		if ( DEBUG )
                        Log.d(TAG, "constructor " );
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		myActivity = this;
		super.onCreate(savedInstanceState);
		setKeepScreenOn(true);
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		myActivity = null;
	}

	public void setKeepScreenOn(boolean keepScreenOn) {
            if(keepScreenOn) {
                this.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            } else {
		this.getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            }
        }
        
        
}

