package com.example.sharkwave

import android.content.Context
import android.net.wifi.WifiManager
import android.os.Bundle
import android.os.StrictMode
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import retrofit2.Retrofit
import java.io.IOException
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress


open class MainActivity : AppCompatActivity(R.layout.activity_main) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val retrofit = Retrofit.Builder()
            .baseUrl("https://jsonplaceholder.typicode.com/")
            .build()

        val service: RestAPI = retrofit.create(RestAPI::class.java)
        Thread {
            while (true) {
                sendHttp(service)
                sendBroadcast()
                sendPing()
                Thread.sleep(2000)
            }
        }.start()
    }

    private fun sendPing() {
        val address = InetAddress.getByName("1.1.1.1")
        val reachable = address.isReachable(10000)
    }

    private fun sendHttp(service: RestAPI) {
        service.getMethod().execute()
    }

    open fun sendBroadcast() {
        // Hack Prevent crash (sending should be done using an async task)
        val policy = StrictMode.ThreadPolicy.Builder().permitAll().build()
        StrictMode.setThreadPolicy(policy)
        val messageStr = "check"
        try {
            //Open a random port to send the package
            val socket = DatagramSocket()
            socket.broadcast = true
            val sendData = messageStr.toByteArray()
            val sendPacket =
                DatagramPacket(sendData, sendData.size, getBroadcastAddress(), 35757)
            socket.send(sendPacket)
        } catch (e: IOException) {
            Log.e("sharkwave", "error: ${e.message}")
        }
    }

    open fun getBroadcastAddress(): InetAddress {
        val dhcp = (applicationContext.getSystemService(Context.WIFI_SERVICE) as WifiManager).dhcpInfo
        // handle null somehow
        val broadcast = dhcp.ipAddress and dhcp.netmask or dhcp.netmask.inv()
        val quads = ByteArray(4)
        for (k in 0..3) quads[k] = (broadcast shr k * 8 and 0xFF).toByte()
        return InetAddress.getByAddress(quads)
    }




}