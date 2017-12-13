/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

package com.trustonic.samplepinpad;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.trustonic.util.tLog;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.util.Log;

public class PinpadTLCTask extends AsyncTask<Void, Void, String> {
    private static final String TAG        = PinpadTLCTask.class
                                                   .getSimpleName();

    private Context             appContext = null;
    private PinpadTUI           pinpadUI   = null;
    private PinpadProvision     provision  = null;
    FileOutputStream            mFOS       = null;
    private Screen              screen     = null;

    PinpadTLCTask(Context context) {
        appContext = context;
    }

    @Override
    protected String doInBackground(Void... arg0) {
        String result = null;
        /* Write binary layout in a file */
/*        tLog.d(TAG, "Write raw layout on fs");
        try {
 			mFOS = appContext.openFileOutput(pinpadUI.getLayoutFileName(),
                    Context.MODE_PRIVATE);
            mFOS.write(pinpadUI.getRawLayout());
            mFOS.flush();
            mFOS.close();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            result = new String("error opening the raw layout file!");
            return result;
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            result = new String("error writing the raw layout binary file!");
            return result;
        }*/
        /************************************************************/
        /* Connect to the TL Pinpad sample */
        if (PinpadTLCWrapper.tlcPinpadConnect()) {
            tLog.d(TAG, "Connected");
            /* Get screen informations */
            if (PinpadTLCWrapper.tlcPinpadGetResolution()) {
                tLog.d(TAG, "Get resolution");
                tLog.d(TAG, "screen width = " + PinpadTLCWrapper.screenWidth
                        + " height = " + PinpadTLCWrapper.screenHeight);

                screen = new Screen();

                provision = new PinpadProvision(appContext, screen);
                if (screen.IsSupported()) {

                    /* Provision all images to the TL Pinpad sample */
                    if (!provision.provisionAllImages()) {
                        tLog.e(TAG, " provisionAllImages failed!");
                        PinpadTLCWrapper.tlcPinpadDisconnect();
                        tLog.d(TAG, "Disconnected");
                        result = new String("Provisionning images failed!");
                        return result;
                    }
                    tLog.d(TAG, "Images successfully provisionned");
                    /* Provision the layout to the TL Pinpad sample */
                    if (!provision.item(PinpadTUI.IDX_LAYOUT)) {
                        tLog.e(TAG, "Provisionning layout failed!");
                        PinpadTLCWrapper.tlcPinpadDisconnect();
                        tLog.d(TAG, "Disconnected");
                        result = new String("Provisionning layout failed!");
                        return result;
                    }
                    tLog.d(TAG, "Layout successfully provisionned");
                    /************************************************************/
                    /* Launch the TUI session of the Pinpad */
                    tLog.d(TAG, "Verifying PIN ...");
                    result = PinpadTLCWrapper.tlcPinpadVerify();
                    tLog.d(TAG, "PIN verification done");
                    /* tLog the result of the TUI session */
                    tLog.d(TAG, result);
                } else {
                    tLog.e(TAG, "Unsupported resolution!");
                    result = new String("Unsupported resolution!");
                    return result;
                }
            } else {
                result = new String("No communication with the TUI secure driver! Please check that the secure driver is present and that the TuiService is running.");
                tLog.d(TAG, "No communication with the TUI secure driver! Please check that the secure driver is present and that the TuiService is running.");
            }
            PinpadTLCWrapper.tlcPinpadDisconnect();
            tLog.d(TAG, "Disconnected");
        } else {
            tLog.d(TAG, "No communication with the Secure World!");
            result = new String("No communication with the Secure World!");
        }
        return result;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        if(result != null) {
            AlertDialog.Builder alert = new AlertDialog.Builder(appContext);
            alert.setTitle(R.string.alert_title);
            alert.setMessage(result);
    
            alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {
    
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    tLog.d(TAG, "OK");
                }
            });
            AlertDialog alertDialog = alert.create();
            alertDialog.show();
        }
    }
}
