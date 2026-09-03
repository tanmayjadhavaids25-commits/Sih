package com.idr.demo.demo

import com.idr.demo.navigation.MockNavigationEngine
import com.idr.demo.navigation.NavigationMode
import com.idr.demo.navigation.NavigationState

class DemoEngine(
    private val navigationEngine: MockNavigationEngine,
    private val gnssSimulationController: GnssSimulationController
) {
    enum class PlaybackState { PLAYING, PAUSED, STOPPED }

    private var playbackState = PlaybackState.STOPPED
    private var elapsed = 0.0
    private var speedMultiplier = 1.0
    private var restorationTimer = 0.0

    fun play() {
        playbackState = PlaybackState.PLAYING
    }

    fun pause() {
        playbackState = PlaybackState.PAUSED
    }

    fun reset() {
        playbackState = PlaybackState.STOPPED
        elapsed = 0.0
        restorationTimer = 0.0
        speedMultiplier = 1.0
        gnssSimulationController.markAvailable()
        navigationEngine.reset()
    }

    fun setSpeed(multiplier: Double) {
        speedMultiplier = multiplier
    }

    fun getSpeed(): Double = speedMultiplier

    fun getPlaybackState(): PlaybackState = playbackState

    fun tick(deltaSeconds: Double): NavigationState {
        if (playbackState == PlaybackState.PLAYING) {
            val scaledDelta = deltaSeconds * speedMultiplier
            elapsed += scaledDelta
            navigationEngine.tick(scaledDelta)

            if (elapsed >= 15.0 && !gnssSimulationController.isBlackoutActive() && gnssSimulationController.currentState() == GnssSimulationState.AVAILABLE) {
                gnssSimulationController.startBlackout()
                navigationEngine.setMode(NavigationMode.AI_DEAD_RECKONING)
            }

            if (gnssSimulationController.isBlackoutActive() && elapsed >= 30.0) {
                gnssSimulationController.restoreGnss()
                restorationTimer = 2.0
                navigationEngine.setMode(NavigationMode.GNSS_RESTORING)
            }

            if (gnssSimulationController.currentState() == GnssSimulationState.RESTORING) {
                restorationTimer -= scaledDelta
                if (restorationTimer <= 0) {
                    gnssSimulationController.markAvailable()
                    navigationEngine.setMode(NavigationMode.GNSS_INS)
                }
            }
        }

        return navigationEngine.getNavigationState()
    }
}
