package com.example.sharkwave

import okhttp3.ResponseBody
import retrofit2.Call
import retrofit2.http.GET

interface RestAPI {

    @GET("posts/1")
    fun getMethod(): Call<ResponseBody>
}