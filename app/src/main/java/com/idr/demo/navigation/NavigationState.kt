package com.idr.demo.navigation

data class NavigationState(
    val latitude: Double,
    val longitude: Double,
    val speed: Double,
    val heading: Double,
    val accuracy: Double,
    val mode: NavigationMode,
    val gnssAvailable: Boolean,
    val mapConfidence: Double,
    val nhcActive: Boolean,
    val aiVelocity: Double,
    val timestamp: Long,
    val sourceLabel: String,
    val ekfActive: Boolean = true,
    val mapMatchingActive: Boolean = true,
    val statusMessage: String = "SYSTEM READY"
)
