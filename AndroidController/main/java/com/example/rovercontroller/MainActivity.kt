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
        
        btnFwd.run {
            setOnTouchListener { v, event ->
                when (event?.action) {
                    MotionEvent.ACTION_DOWN -> {
                        changeBtnState(true, btnDwn, btnLft, btnRght)
                        ws?.apply {
                            val message : String = "0${seekSpeed.progress}"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                    MotionEvent.ACTION_UP -> {
                        changeBtnState(false, btnDwn, btnLft, btnRght)
                        ws?.apply {
                            val message : String = "999"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                }
                v?.onTouchEvent(event) != false
            }
        }


        btnDwn.run {
            setOnTouchListener { v, event ->
                when (event?.action) {
                    MotionEvent.ACTION_DOWN -> {
                        changeBtnState(true, btnFwd, btnLft, btnRght)
                        ws?.apply {
                            val message : String = "2${seekSpeed.progress}"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                    MotionEvent.ACTION_UP -> {
                        changeBtnState(false, btnFwd, btnLft, btnRght)
                        ws?.apply {
                            val message : String = "999"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                }
                v?.onTouchEvent(event) != false
            }
        }


        btnRght.run {
            setOnTouchListener { v, event ->
                when (event?.action) {
                    MotionEvent.ACTION_DOWN -> {
                        changeBtnState(true, btnDwn, btnLft, btnFwd)
                        ws?.apply {
                            val message : String = "1${seekSpeed.progress}"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                    MotionEvent.ACTION_UP -> {
                        changeBtnState(false, btnDwn, btnLft, btnFwd)
                        ws?.apply {
                            val message : String = "999"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                }
                v?.onTouchEvent(event) != false
            }
        }
        
        btnLft.run {
            setOnTouchListener { v, event ->
                when (event?.action) {
                    MotionEvent.ACTION_DOWN -> {
                        changeBtnState(true, btnDwn, btnRght, btnFwd)
                        ws?.apply {
                            val message : String = "3${seekSpeed.progress}"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                    MotionEvent.ACTION_UP -> {
                        changeBtnState(false, btnDwn, btnRght, btnFwd)
                        ws?.apply {
                            val message : String = "999"
                            send(message)
                        } ?: ping("Error: Restart the App to reconnect")
                    }
                }
                v?.onTouchEvent(event) != false
            }
        }

        seekSpeed.max = maxSpeed
        seekSpeed.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            @SuppressLint("SetTextI18n")
            override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {
                currentPercent = (seekSpeed.progress.toDouble() / maxSpeed) * 100
                currentPercent = floor(currentPercent)
                tvSpeed.text = "${currentPercent}%"
            }

            override fun onStartTrackingTouch(p0: SeekBar?) {

            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
                if(isGoing)
                {
                    ws?.apply {
                        val message : String = "5${seekSpeed.progress}"
                        send(message)
                    } ?: ping("Error: Restart the App to reconnect")
                }
            }
        })

        seekHH.max = maxAngle
        seekHH.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {

            }

            override fun onStartTrackingTouch(p0: SeekBar?) {

            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
                ws?.apply {
                    val message : String = "8${seekHH.progress}"
                    send(message)
                } ?: ping("Error: Restart the App to reconnect")
            }
        })

        seekHV.max = maxAngle
        seekHV.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {

            override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {

            }

            override fun onStartTrackingTouch(p0: SeekBar?) {

            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
                ws?.apply {
                    val message : String = "7${seekHV.progress}"
                    send(message)
                } ?: ping("Error: Restart the App to reconnect")
            }
        })

    }




    override fun onResume() {
        super.onResume()
        start()
    }

    override fun onPause() {
        super.onPause()
        stop()
    }

    private fun start() {
        val request: Request = Request.Builder().url("ws://10.0.0.1:8888/").build()
        val listener = EchoWebSocketListener(this::ping) { ws = null }
        ws = client.newWebSocket(request, listener)
    }

    private fun stop() {
        ws?.apply {
            val message : String = "999"
            send(message)
        } ?: ping("Error: Restart the App to reconnect")
        ws?.close(NORMAL_CLOSURE_STATUS, "Goodbye !")
    }

    override fun onDestroy() {
        super.onDestroy()
        client.dispatcher.executorService.shutdown()
    }

    private fun changeBtnState(state: Boolean, btn1 : Button, btn2: Button, btn3 : Button)
    {
        isGoing = state
        btn1.isEnabled = !state
        btn2.isEnabled = !state
        btn3.isEnabled = !state
    }

    private fun ping(txt: String) {
        runOnUiThread {
            Toast.makeText(this, txt, Toast.LENGTH_SHORT).show()
        }
    }
}