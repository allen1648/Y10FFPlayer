package com.stan.y10ffplayer.Utils;

import android.os.Environment;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;

/** 控制台日志工具类 */
public class LogUtil {
    private static final String TAG = "yyl";

    private static final int TO_NONE = 0x0;
    private static final int TO_CONSOLE = 0x1;
    private static final int TO_SCREEN = 0x10;
    private static final int TO_FILE = 0x100;
    private static final int FROM_LOGCAT = 0x1000;

    private static final int DEBUG_ALL =TO_CONSOLE;
    /** 统一控制打印的等级 */
    private static final int LOG_LEVEL = Log.VERBOSE;
    private static final int LOG_MAXSIZE = 2 * 1024 * 1024; // double the size

    private static final String LOG_PATH = Environment.getExternalStorageState();
    private static final String LOG_OLD_FILE = LOG_PATH + "log.log";
    private static final String LOG_TEMP_FILE = LOG_PATH + "log.tmp";
    //上面两个文件压缩到一个zip包内，日志上传使用
    private static final String LOG_ZIP_FILE = LOG_PATH + "log.zip";

    private static Object lockObj = new Object();
    public static PaintLogThread mPaintLogThread = null;
    //static Calendar mDate = Calendar.getInstance();
    static StringBuffer mBuffer = new StringBuffer();
    static OutputStream mLogStream;
    static long mFileSize;

    public static void v(String tag, String msg) {
        log(tag, msg, DEBUG_ALL, Log.VERBOSE);
    }

    public static void i(String tag, String msg) {
        log(tag, msg, DEBUG_ALL, Log.INFO);
    }

    public static void d(String tag, String msg) {
        log(tag, msg, DEBUG_ALL, Log.DEBUG);
    }

    public static void w(String tag, String msg) {
        log(tag, msg, DEBUG_ALL, Log.WARN);
    }

    public static void e(String tag, String msg) {
        log(tag, msg, DEBUG_ALL, Log.ERROR);
    }

    public static void log(String msg) {
        d(TAG, msg);
    }
    
    public static void v(boolean debugType, String tag, String msg) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }
        
        if (debugType) {
            Log.v(tag, msg);
        }
    }
    
    public static void i(boolean debugType, String tag, String msg) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }
        
        if (debugType) {
            Log.i(tag, msg);
        }
    }
    
    public static void d(boolean debugType, String tag, String msg) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }
        
        if (debugType) {
            Log.d(tag, msg);
        }
    }
    
    
    
    public static void w(boolean debugType, String tag, String msg) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }
        
        if (debugType) {
            Log.w(tag, msg);
        }
    }
    
    public static void e(boolean debugType, String tag, String msg) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }
        
        if (debugType) {
            Log.e(tag, msg);
        }
    }

    private static void log(String tag, String msg, int outdest, int level) {
        if (tag == null) {
            tag = "TAG_NULL";
        }

        if (msg == null) {
            msg = "MSG_NULL";
        }

        if (level >= LOG_LEVEL) {
            if ((outdest & TO_CONSOLE) != TO_NONE) {
                logToConsole(tag, msg, level);
            }

            if ((outdest & TO_SCREEN) != TO_NONE) {
                logToScreen(tag, msg, level);
            }

            if ((outdest & FROM_LOGCAT) != TO_NONE) {
                if (mPaintLogThread == null) {
                    LogUtil log = new LogUtil();
                    mPaintLogThread = log.new PaintLogThread();
                    mPaintLogThread.start();
                }
            }

            if ((outdest & TO_FILE) != TO_NONE) {
                logToFile(tag, msg, level);
            }
        }
    }

    /**
     * 组成Log字符串.添加时间信息.
     *
     * @param tag
     * @param msg
     * @return
     */
    private static String getLogStr(String tag, String msg) {
        mBuffer.setLength(0);
        mBuffer.append("[");
        mBuffer.append(tag);
        mBuffer.append("] ");
        mBuffer.append(msg);

        return mBuffer.toString();
    }

    /**
     * 将log打到控制台
     * 
     * @param tag
     * @param msg
     * @param level
     */
    private static void logToConsole(String tag, String msg, int level) {
        switch (level) {
        case Log.DEBUG:
            Log.d(tag, msg);
            break;
        case Log.ERROR:
            Log.e(tag, msg);
            break;
        case Log.INFO:
            Log.i(tag, msg);
            break;
        case Log.VERBOSE:
            Log.v(tag, msg);
            break;
        case Log.WARN:
            Log.w(tag, msg);
            break;
        default:
            break;
        }
    }

    /**
     * 将log打到文件日志
     *
     * @param tag
     * @param msg
     * @param level
     */
    private static void logToFile(String tag, String msg, int level) {
        synchronized (lockObj) {
            OutputStream outStream = openLogFileOutStream();

            if (outStream != null) {
                try {
                    byte[] d = getLogStr(tag, msg).getBytes("utf-8");
                    // byte[] d = msg.getBytes("utf-8");
                    if (mFileSize < LOG_MAXSIZE) {
                        outStream.write(d);
                        outStream.write("\r\n".getBytes());
                        outStream.flush();
                        mFileSize += d.length;

                    } else {
                        closeLogFileOutStream();
                        renameLogFile();
                        logToFile(tag, msg, level);
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }

            } else {
                Log.w(TAG, "Log File open fail: [AppPath]=");
            }
        }
    }

    private static void logToScreen(String tag, String msg, int level) {

    }

    /**
     * 获取日志临时文件输入流
     *
     * @return
     */
    private static OutputStream openLogFileOutStream() {
        if (mLogStream == null) {
            try {
                File file = openAbsoluteFile(LOG_TEMP_FILE);

                if (file == null) {
                    return null;
                }

                if (file.exists()) {
                    mLogStream = new FileOutputStream(file, true);
                    mFileSize = file.length();
                } else {
                    // file.createNewFile();
                    mLogStream = new FileOutputStream(file);
                    mFileSize = 0;
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
        }
        return mLogStream;
    }

    /**
     * 关闭日志输出流
     */
    private static void closeLogFileOutStream() {
        try {
            if (mLogStream != null) {
                mLogStream.close();
                mLogStream = null;
                mFileSize = 0;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static File openAbsoluteFile(String name) {
        File file = new File(name);
        return file;
    }

    /**
     * rename log file
     */
    private static void renameLogFile() {
        synchronized (lockObj) {
            File file = openAbsoluteFile(LOG_TEMP_FILE);
            File destFile = openAbsoluteFile(LOG_OLD_FILE);

            if (destFile.exists()) {
                destFile.delete();
            }

            file.renameTo(destFile);
        }
    }

    public static void startCatchLogcat() {
        synchronized (lockObj) {
            if (mPaintLogThread == null) {
                LogUtil log = new LogUtil();
                mPaintLogThread = log.new PaintLogThread();
                mPaintLogThread.start();
            }
        }
    }

    public static void stopCatchLogcat() {
        if (mPaintLogThread != null) {
            mPaintLogThread.shutdown();
            mPaintLogThread = null;
        }
    }

    class PaintLogThread extends Thread {
        Process mProcess;
        boolean mStop = false;

        public void shutdown() {
            LogUtil.i("PaintLogThread:", "shutdown");
            mStop = true;
            if (mProcess != null) {
                mProcess.destroy();
                mProcess = null;
            }
        }

        public void run() {
            try {
                LogUtil.i("PaintLogThread:", "start");
                ArrayList<String> commandLine = new ArrayList<String>();
                commandLine.add("logcat");
                commandLine.add("-v");
                commandLine.add("time");
                mProcess = Runtime.getRuntime().exec(commandLine.toArray(new String[commandLine.size()]));

                BufferedReader bufferedReader = new BufferedReader
                // ( new InputStreamReader(mProcess.getInputStream()), 1024);
                (new InputStreamReader(mProcess.getInputStream()));

                String line = null;
                while (!mStop) {
                    line = bufferedReader.readLine();
                    if (line != null) {
                        logToFile("SysLog", line, Log.VERBOSE);
                    } else {
                        if (line == null) {
                            break;
                        }
                    }
                }

                bufferedReader.close();
                if (mProcess != null) {
                    mProcess.destroy();
                    mProcess = null;
                }

                mPaintLogThread = null;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
