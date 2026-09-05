package com.idr.demo.ui

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.idr.demo.demo.DemoEngine
import com.idr.demo.demo.DemoRoute
import com.idr.demo.demo.GnssSimulationController
import com.idr.demo.demo.GnssSimulationState
import com.idr.demo.map.MapLayerManager
import com.idr.demo.map.MapLayers
import com.idr.demo.map.MapManager
import com.idr.demo.navigation.MockNavigationEngine
import com.idr.demo.navigation.NavigationMode
import com.idr.demo.navigation.NavigationState
import com.idr.demo.navigation.RealNavigationEngine
import com.idr.demo.sensors.ImuData
import com.idr.demo.sensors.SensorManager
import com.idr.demo.telemetry.TelemetryManager
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class AppViewModel : ViewModel() {
    private val mockEngine = MockNavigationEngine()
    private val realEngine = RealNavigationEngine()
    private val gnssController = GnssSimulationController()
    private val sensorManager = SensorManager()
    private val telemetryManager = TelemetryManager()
    private val mapManager = MapManager()
    private val layerManager = MapLayerManager()
    private val demoEngine = DemoEngine(mockEngine, gnssController)

    var useDemoEngine by mutableStateOf(true)
    var navState by mutableStateOf(mockEngine.getNavigationState())
        private set
    var telemetrySnapshot by mutableStateOf(telemetryManager.buildSnapshot(navState))
        private set
    var mapLayers by mutableStateOf(layerManager.layers)
        private set
    var bannerMessage by mutableStateOf<String?>(null)
        private set
    var playbackState by mutableStateOf(demoEngine.getPlaybackState())
        private set
    var playbackSpeed by mutableStateOf(1.0)
        private set
    var sensorHistory by mutableStateOf(List(60) { ImuData(0f, 0f, 9.8f, 0f, 0f, 0f, System.currentTimeMillis()) })
        private set

    val routePoints = DemoRoute.points
    val mapStatus = mapManager.mapStatus()

    init {
        viewModelScope.launch {
            var t = 0.0
            while (isActive) {
                t += 0.1
                sensorManager.tick(t)
                sensorHistory = (sensorHistory + sensorManager.imuData.value).takeLast(120)

                navState = if (useDemoEngine) {
                    val state = demoEngine.tick(0.1)
                    playbackState = demoEngine.getPlaybackState()
                    if (gnssController.currentState() == GnssSimulationState.DENIED && bannerMessage == null) {
                        bannerMessage = "⚠ GNSS SIGNAL LOST • DEAD RECKONING ACTIVE"
                    }
                    if (gnssController.currentState() == GnssSimulationState.RESTORING) {
                        bannerMessage = "✓ GNSS RESTORED • POSITION RE-SYNCHRONIZED"
                    }
                    if (gnssController.currentState() == GnssSimulationState.AVAILABLE && state.mode == NavigationMode.GNSS_INS) {
                        if (bannerMessage?.contains("RESTORED") == true) {
                            delay(400)
                            bannerMessage = null
                        }
                    }
                    state
                } else {
                    realEngine.getNavigationState()
                }

                telemetrySnapshot = telemetryManager.buildSnapshot(navState)
                delay(100)
            }
        }
    }

    fun clearBanner() {
        bannerMessage = null
    }

    fun playDemo() {
        demoEngine.play()
        playbackState = demoEngine.getPlaybackState()
    }

    fun pauseDemo() {
        demoEngine.pause()
        playbackState = demoEngine.getPlaybackState()
    }

    fun resetDemo() {
        demoEngine.reset()
        playbackState = demoEngine.getPlaybackState()
        bannerMessage = null
    }

    fun setDemoSpeed(multiplier: Double) {
        demoEngine.setSpeed(multiplier)
        playbackSpeed = multiplier
    }

    fun simulateGnssLoss() {
        gnssController.startBlackout()
        mockEngine.setMode(NavigationMode.AI_DEAD_RECKONING)
        bannerMessage = "⚠ GNSS SIGNAL LOST • DEAD RECKONING ACTIVE"
    }

    fun restoreGnss() {
        gnssController.restoreGnss()
        mockEngine.setMode(NavigationMode.GNSS_RESTORING)
        bannerMessage = "✓ GNSS RESTORED • POSITION RE-SYNCHRONIZED"
        viewModelScope.launch {
            delay(2000)
            gnssController.markAvailable()
            mockEngine.setMode(NavigationMode.GNSS_INS)
            delay(800)
            bannerMessage = null
        }

        fun centerVehicle() {
            bannerMessage = "Vehicle centered on map"
            viewModelScope.launch {
                delay(700)
                if (bannerMessage == "Vehicle centered on map") bannerMessage = null
            }
        }
    }

    fun setEngineMode(useDemo: Boolean) {
        useDemoEngine = useDemo
        if (useDemo) {
            resetDemo()
            mockEngine.setMode(NavigationMode.GNSS_INS)
            navState = mockEngine.getNavigationState()
        } else {
            navState = realEngine.getNavigationState()
        }
    }

    fun setLayerToggle(transform: (MapLayers) -> MapLayers) {
        val updated = transform(mapLayers)
        layerManager.update(updated)
        mapLayers = layerManager.layers
    }

    fun systemStatusCards(): List<Pair<String, String>> = listOf(
        "GNSS" to if (navState.gnssAvailable) "CONNECTED" else "LOST",
        "IMU" to "ACTIVE",
        "AI VELOCITY" to "READY",
        "INS" to "READY",
        "MAP" to mapStatus
    )
}
