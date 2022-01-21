package com.example.rovercontroller

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import com.example.rovercontroller.EchoWebSocketListener.Companion.NORMAL_CLOSURE_STATUS
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.WebSocket
import org.w3c.dom.Text
import kotlin.math.floor

class MainActivity : AppCompatActivity() {
    private val client by lazy {
        OkHttpClient()
    }
    private val btnFwd by lazy { findViewById<Button>(R.id.btn_fwd) }
    private val btnDwn by lazy { findViewById<Button>(R.id.btn_dwn) }
    private val btnRght by lazy { findViewById<Button>(R.id.btn_rght) }
    private val btnLft by lazy { findViewById<Button>(R.id.btn_lft) }

    private val seekSpeed by lazy { findViewById<SeekBar>(R.id.seekSpeed)}
    private val seekHH by lazy { findViewById<SeekBar>(R.id.seekHH)}
    private val seekHV by lazy { findViewById<SeekBar>(R.id.seekHV)}

    private val tvSpeed by lazy {findViewById<TextView>(R.id.tvSpeed)}

    private val maxSpeed : Int = 255
    private val maxAngle : Int = 180
    private var currentPercent : Double = 0.0

    private var isGoing = false

    private var ws: WebSocket? = null

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        /*
        Fwd = 0
        Rght = 1
        Dwn = 2
        Lft = 3
        update = 5
        hv = 7
        hh = 8
        allstop = 999
         */
        
}

