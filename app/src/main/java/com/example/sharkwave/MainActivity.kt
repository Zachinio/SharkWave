package com.example.sharkwave

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import retrofit2.Retrofit


open class MainActivity : AppCompatActivity(R.layout.activity_main) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val retrofit = Retrofit.Builder()
            .baseUrl("https://jsonplaceholder.typicode.com/")
            .build()

        val service: RestAPI = retrofit.create(RestAPI::class.java)
        Thread {
            while (true) {
                sendHttp(service);
                Thread.sleep(2000);
            }
        }.start()
    }

    private fun sendHttp(service: RestAPI) {
        service.getMethod().execute()
    }
}