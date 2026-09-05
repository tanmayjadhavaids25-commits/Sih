package com.idr.demo.sensors

import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlin.math.sin

class SensorManager {
    private val _imuData = MutableStateFlow(
        ImuData(0f, 0f, 9.8f, 0f, 0f, 0f, System.currentTimeMillis())
    )
    val imuData: StateFlow<ImuData> = _imuData

    fun tick(timeSeconds: Double) {
        val ax = (sin(timeSeconds) * 0.7).toFloat()
        val ay = (sin(timeSeconds * 0.7) * 0.6).toFloat()
        val az = (9.5 + sin(timeSeconds * 1.2) * 0.4).toFloat()
        val gx = (sin(timeSeconds * 1.3) * 0.2).toFloat()
        val gy = (sin(timeSeconds * 0.8) * 0.25).toFloat()
        val gz = (sin(timeSeconds * 0.4) * 0.3).toFloat()
        _imuData.value = ImuData(ax, ay, az, gx, gy, gz, System.currentTimeMillis())
    }
}
