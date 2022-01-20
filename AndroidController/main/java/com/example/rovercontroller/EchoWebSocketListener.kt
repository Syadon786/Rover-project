package com.example.rovercontroller


import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Base64
import okhttp3.WebSocketListener
import okhttp3.WebSocket
import okio.ByteString
import okhttp3.Response


internal class EchoWebSocketListener(
    val ping: (String) -> Unit,
    val closing: () -> Unit
) : WebSocketListener() {
    override fun onOpen(webSocket: WebSocket, response: Response) {
        ping("Connected!")
    }

    override fun onMessage(webSocket: WebSocket, text: String) {

    }

    
}