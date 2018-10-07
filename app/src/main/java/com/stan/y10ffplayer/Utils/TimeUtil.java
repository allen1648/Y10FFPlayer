package com.stan.y10ffplayer.Utils;

public class TimeUtil {
    public static String secondsToDateFormat(int scds, boolean showHours) {
        long hours = scds / (60 * 60);
        long minutes = (scds % (60 * 60)) / (60);
        long seconds = scds % (60);

        String sh = "00";
        if (hours > 0) {
            if (hours < 10) {
                sh = "0" + hours;
            } else {
                sh = hours + "";
            }
        }

        String sm = "00";
        if (minutes > 0) {
            if (minutes < 10) {
                sm = "0" + minutes;
            } else {
                sm = minutes + "";
            }
        }

        String ss = "00";
        if (seconds > 0) {
            if (seconds < 10) {
                ss = "0" + seconds;
            } else {
                ss = seconds + "";
            }
        }

        if(showHours) {
            return sh + ":" + sm + ":" + ss;
        }

        return sm + ":" + ss;

    }

}
