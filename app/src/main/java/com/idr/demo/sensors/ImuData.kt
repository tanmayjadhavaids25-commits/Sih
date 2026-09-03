package com.idr.demo.sensors

data class ImuData(
    val ax: Float,
    val ay: Float,
    val az: Float,
    val gx: Float,
    val gy: Float,
    val gz: Float,
    val timestamp: Long
)
