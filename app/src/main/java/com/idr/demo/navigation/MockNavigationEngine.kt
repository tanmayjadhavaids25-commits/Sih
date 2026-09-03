package com.idr.demo.navigation

import com.idr.demo.demo.DemoRoute
import com.idr.demo.sensors.ImuData
import kotlin.math.max
import kotlin.math.min

class MockNavigationEngine : NavigationEngine, BackendInterface {
    private val route = DemoRoute.points
    private var progress = 0.0
    private var mode: NavigationMode = NavigationMode.GNSS_INS
    private var gnssAvailable = true
    private var status = "SYSTEM READY"

    private fun currentState(): NavigationState {
        val idx = progress.toInt().coerceIn(0, route.lastIndex)
        val next = min(route.lastIndex, idx + 1)
        val frac = (progress - idx).coerceIn(0.0, 1.0)
        val p0 = route[idx]
        val p1 = route[next]

        val lat = p0.latitude + (p1.latitude - p0.latitude) * frac
        val lon = p0.longitude + (p1.longitude - p0.longitude) * frac
        val heading = p0.headingDeg + (p1.headingDeg - p0.headingDeg) * frac
        val speed = p0.speedKmh + (p1.speedKmh - p0.speedKmh) * frac

        val mapConfidence = when (mode) {
            NavigationMode.GNSS_INS -> 94.0
            NavigationMode.AI_DEAD_RECKONING -> 87.0
            NavigationMode.MAP_MATCHING -> 90.0
            NavigationMode.GNSS_RESTORING -> 91.0
            NavigationMode.ERROR -> 0.0
        }

        val accuracy = when (mode) {
            NavigationMode.GNSS_INS -> 8.2
            NavigationMode.AI_DEAD_RECKONING -> 12.8
            NavigationMode.MAP_MATCHING -> 10.4
            NavigationMode.GNSS_RESTORING -> 9.5
            NavigationMode.ERROR -> 99.0
        }

        return NavigationState(
            latitude = lat,
            longitude = lon,
            speed = speed,
            heading = heading,
            accuracy = accuracy,
            mode = mode,
            gnssAvailable = gnssAvailable,
            mapConfidence = mapConfidence,
            nhcActive = mode != NavigationMode.ERROR,
            aiVelocity = max(5.0, speed - 1.2),
            timestamp = System.currentTimeMillis(),
            sourceLabel = "DEMO / SIMULATED",
            ekfActive = mode != NavigationMode.ERROR,
            mapMatchingActive = mode != NavigationMode.ERROR,
            statusMessage = status
        )
    }

    override fun processImu(imuData: ImuData) = Unit

    override fun processGnss(latitude: Double, longitude: Double, speed: Double, heading: Double, accuracy: Double) = Unit

    override fun getNavigationState(): NavigationState = currentState()

    override fun setMode(mode: NavigationMode) {
        this.mode = mode
        status = when (mode) {
            NavigationMode.GNSS_INS -> "GNSS + INS active"
            NavigationMode.AI_DEAD_RECKONING -> "Dead reckoning active"
            NavigationMode.MAP_MATCHING -> "Map matching active"
            NavigationMode.GNSS_RESTORING -> "GNSS restoring"
            NavigationMode.ERROR -> "Engine error"
        }
        gnssAvailable = mode != NavigationMode.AI_DEAD_RECKONING && mode != NavigationMode.ERROR
    }

    override fun tick(deltaSeconds: Double) {
        progress += deltaSeconds * 0.8
        if (progress >= route.lastIndex) progress = route.lastIndex.toDouble()
    }

    fun reset() {
        progress = 0.0
        mode = NavigationMode.GNSS_INS
        gnssAvailable = true
        status = "SYSTEM READY"
    }

    override fun isAvailable(): Boolean = true

    override fun status(): String = "Mock navigation engine active"
}
