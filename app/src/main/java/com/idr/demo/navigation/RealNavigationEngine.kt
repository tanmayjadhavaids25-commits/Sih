package com.idr.demo.navigation

import com.idr.demo.sensors.ImuData

class RealNavigationEngine : NavigationEngine, BackendInterface {
    private var state = NavigationState(
        latitude = 18.5204,
        longitude = 73.8567,
        speed = 0.0,
        heading = 0.0,
        accuracy = 15.0,
        mode = NavigationMode.ERROR,
        gnssAvailable = false,
        mapConfidence = 0.0,
        nhcActive = false,
        aiVelocity = 0.0,
        timestamp = System.currentTimeMillis(),
        sourceLabel = "REAL BACKEND PLACEHOLDER",
        ekfActive = false,
        mapMatchingActive = false,
        statusMessage = "Backend unavailable"
    )

    override fun processImu(imuData: ImuData) = Unit

    override fun processGnss(latitude: Double, longitude: Double, speed: Double, heading: Double, accuracy: Double) {
        state = state.copy(
            latitude = latitude,
            longitude = longitude,
            speed = speed,
            heading = heading,
            accuracy = accuracy,
            mode = NavigationMode.GNSS_INS,
            gnssAvailable = true,
            timestamp = System.currentTimeMillis(),
            statusMessage = "GNSS update received"
        )
    }

    override fun getNavigationState(): NavigationState = state

    override fun setMode(mode: NavigationMode) {
        state = state.copy(mode = mode, timestamp = System.currentTimeMillis())
    }

    override fun tick(deltaSeconds: Double) = Unit

    override fun isAvailable(): Boolean = false

    override fun status(): String = "M7 backend adapter pending integration"
}
