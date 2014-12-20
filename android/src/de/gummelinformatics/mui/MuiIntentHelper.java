package de.gummelinformatics.mui;

import org.qtproject.qt5.android.bindings.QtActivity;
import 	android.content.Intent;
import android.net.Uri;

public class MuiIntentHelper extends QtActivity 
{
	private static MuiIntentHelper instance;

        public MuiIntentHelper()
	{
		instance = this;
	}

        public void openUrl(String url) {
		Intent i = new Intent(Intent.ACTION_VIEW);
		i.setData(Uri.parse(url));
		startActivity(i);
	}
}

