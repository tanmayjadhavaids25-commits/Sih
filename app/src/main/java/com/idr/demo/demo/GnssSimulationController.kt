package com.idr.demo.demo

enum class GnssSimulationState {
    AVAILABLE,
    DENIED,
    RESTORING
}

class GnssSimulationController {
    private var state = GnssSimulationState.AVAILABLE

    fun startBlackout() {
        state = GnssSimulationState.DENIED
    }

    fun restoreGnss() {
        state = GnssSimulationState.RESTORING
    }

    fun markAvailable() {
        state = GnssSimulationState.AVAILABLE
    }

    fun isBlackoutActive(): Boolean = state == GnssSimulationState.DENIED

    fun currentState(): GnssSimulationState = state
}
