package de.gummelinformatics.mui;

import org.qtproject.qt5.android.bindings.QtActivity;
import 	android.content.Intent;
import android.net.Uri;
import android.os.Handler;

import android.os.Looper;
public class MuiIntentHelper extends QtActivity 
{
	private static MuiIntentHelper instance;

    public MuiIntentHelper() 	{
		instance = this;
	}

		//Handler handler = new Handler(Looper.getMainLooper());
		
    /* broken */
    public void openUrl(String url) {
		class myRunnable implements Runnable {
			String url;
			myRunnable (String u) {url=u;}
			public void run () {
				Intent i = new Intent(Intent.ACTION_VIEW);
				i.setData(Uri.parse(url));
				startActivity(i);
			}
		}
		this.runOnUiThread( new myRunnable(url) );	
		//this.runOnUiThread( new Runnable() {
			//public void run() 
			//{
				//getWindow().addFlags( WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON );
            //}        
		//} );
			//Intent i = new Intent(Intent.ACTION_VIEW);
			//i.setData(Uri.parse(url));
			//startActivity(i);
			//return 1;
	}

	public boolean test() {
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse("http://google.com"));
		startActivity(i);
                return true;
	}
}

