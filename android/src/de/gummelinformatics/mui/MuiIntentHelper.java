package de.gummelinformatics.mui;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.util.Log;
import android.view.ViewConfiguration;
import android.os.Build;
import android.os.Looper;
import android.os.Environment;
import java.io.File;

public class MuiIntentHelper extends QtActivity
{
    private static final String TAG = MuiIntentHelper.class.getSimpleName();
    private static final boolean DEBUG = Config.DEBUG;

    private static MuiIntentHelper instance;

    static class MyIntentRunnable implements Runnable {
        private final String TAG = MyIntentRunnable.class.getSimpleName();
        private static final boolean DEBUG = Config.DEBUG;
        String url;

        MyIntentRunnable(String u) {url=u;}
        public void run () {
            Intent i = new Intent(Intent.ACTION_VIEW);
            i.setData(Uri.parse(url));
            MuiActivity.getActivityInstance().startActivity(i);
            if ( DEBUG )
                Log.d(TAG, "run()" );
        }
    }

    public MuiIntentHelper() 	{
        instance = this;
    }


    public static void openUrl(String url) {
        if (DEBUG)
            Log.d(TAG, "view intent");

        MuiActivity.getActivityInstance().runOnUiThread( new MyIntentRunnable(url) );

        Log.w(TAG, "view intent " + url);

        if (DEBUG)
            Log.d(TAG, "view intent fired");
    }


    public static void testOpenUrl() {
        if (DEBUG)
            Log.d(TAG, "openUrl() view intent");

        //}
        MuiActivity.getActivityInstance().runOnUiThread( new MyIntentRunnable("http://mdforum.designer2k2.at") );

        Log.w(TAG, "view intent ");

        if (DEBUG)
            Log.d(TAG, "view intent fired");
    }

    public static void telTest() {
        class TestIntentRunnable implements Runnable {
            private final String TAG = TestIntentRunnable.class.getSimpleName();
            private static final boolean DEBUG = Config.DEBUG;

            public void run () {
                try {
                    Intent i = new Intent(Intent.ACTION_VIEW);
                    i.setData(Uri.parse("tel:6666"));
                    MuiActivity.getActivityInstance().startActivity(i);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                if ( DEBUG )
                    Log.d(TAG, "run()" );
            }
        }
        MuiActivity.getActivityInstance().runOnUiThread( new TestIntentRunnable() );
    }

    public static boolean test() {
        Intent i = new Intent(Intent.ACTION_VIEW);
        i.setData(Uri.parse("http://google.com"));
        MuiActivity.getActivityInstance().startActivity(i);
        return true;
    }

    public static boolean hasPermanentMenuKey()
    {
        // action bar: http://developer.android.com/guide/topics/ui/actionbar.html
        try {
            boolean pk = Build.VERSION.SDK_INT < 11 || (Build.VERSION.SDK_INT >= 14 &&
                    (Boolean)ViewConfiguration.class.getMethod("hasPermanentMenuKey").invoke(ViewConfiguration.get( MuiActivity.getActivityInstance() )));
            if ( DEBUG )
                Log.d(TAG, "hasPermanentMenuKey(): " + (pk==true?"true":"false"));

            return pk;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }
    
    public static String getPublicDocumentPath() 
    {
        /* Checks if external storage is available for read and write */
        //isExternalStorageWritable()
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // oder getExternalStorageDirectory
            File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS);
            return path.getAbsolutePath() ;
        }
        return "";
    }

    public static String getPublicDownloadsPath() 
    {
        /* Checks if external storage is available for read and write */
        //isExternalStorageWritable()
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            // oder getExternalStorageDirectory
            File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
            return path.getAbsolutePath() ;
        }
        return "";
    }

}

