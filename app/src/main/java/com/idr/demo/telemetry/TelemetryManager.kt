package com.idr.demo.telemetry

import com.idr.demo.navigation.NavigationState

data class TelemetrySnapshot(
    val state: NavigationState,
    val processingMs: Double,
    val sourceLabel: String
)

class TelemetryManager {
    fun buildSnapshot(state: NavigationState): TelemetrySnapshot {
        val processingMs = when (state.mode.label) {
            "GNSS + INS" -> 5.2
            "DEAD RECKONING" -> 4.8
            else -> 5.0
        }
        return TelemetrySnapshot(state = state, processingMs = processingMs, sourceLabel = state.sourceLabel)
    }
}
