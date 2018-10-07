package com.stan.y10ffplayer.Utils;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

public class PermissionUtil {
     public static void requestPermission(Context context, String permission) {
         if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
             if (ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
                 ActivityCompat.requestPermissions((Activity) context, new String[]{permission}, 1);
             }
         }
     }
}
