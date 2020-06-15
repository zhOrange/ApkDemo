package com.zh.jninativedemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView tv = (TextView)findViewById(R.id.textV);

        tv.setText(NativeFunc.getStringFromJNI());

//        Person person = new Person();
//        NativeFunc.getNativePerson(person);
//        tv.setText(person.getName());

//        int age = NativeFunc.getNativePerson2().getAge();
//        tv.setText("" + age);
    }
}
