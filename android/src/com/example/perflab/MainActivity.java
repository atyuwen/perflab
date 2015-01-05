package com.example.perflab;

import com.example.perflab.R;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
	    TextView textView = (TextView)findViewById(R.id.text1);
	    textView.setTextSize(8);
	    textView.setText(stringFromJNI());
	}

    public native String stringFromJNI();

    static
    {
    	System.loadLibrary("perflab");
    }
}
