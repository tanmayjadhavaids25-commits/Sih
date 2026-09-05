package com.idr.demo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

@Composable
fun TechnicalDashboard(vm: AppViewModel, onSensors: () -> Unit) {
    val t = vm.telemetrySnapshot
    val s = t.state

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(rememberScrollState())
            .padding(12.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Text("SYSTEM STATUS", fontWeight = FontWeight.Bold)
        Text("${s.mode.label} • ${s.sourceLabel}")

        DashboardCard(listOf(
            "GNSS" to if (s.gnssAvailable) "CONNECTED" else "LOST",
            "IMU" to "ACTIVE",
            "Accelerometer" to "100 Hz",
            "Gyroscope" to "100 Hz",
            "Navigation" to "10 Hz",
            "AI Velocity" to "ACTIVE",
            "EKF" to if (s.ekfActive) "ACTIVE" else "INACTIVE",
            "NHC" to if (s.nhcActive) "ACTIVE" else "INACTIVE",
            "MAP MATCH" to if (s.mapMatchingActive) "ACTIVE" else "INACTIVE",
            "HMM" to "ACTIVE",
            "Map Confidence" to "${"%.0f".format(s.mapConfidence)}%",
            "Processing" to "${"%.1f".format(t.processingMs)} ms"
        ))

        DashboardCard(listOf(
            "Latitude" to "${"%.6f".format(s.latitude)}",
            "Longitude" to "${"%.6f".format(s.longitude)}",
            "Forward Vel" to "${"%.1f".format(s.speed)} km/h",
            "Lateral Vel" to "0.8 km/h",
            "Heading" to "${"%.1f".format(s.heading)}°",
            "Covariance PX" to "0.82",
            "Covariance PY" to "0.90",
            "Covariance PV" to "0.51",
            "Covariance PHEADING" to "0.28"
        ))

        Button(onClick = onSensors, modifier = Modifier.fillMaxWidth()) {
            Text("OPEN SENSOR MONITOR")
        }
    }
}

@Composable
private fun DashboardCard(rows: List<Pair<String, String>>) {
    Card(modifier = Modifier.fillMaxWidth()) {
        Column(Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
            rows.forEach { (k, v) ->
                Row(modifier = Modifier.fillMaxWidth()) {
                    Text(k, modifier = Modifier.weight(1f))
                    Text(v, fontWeight = FontWeight.SemiBold)
                }
            }
        }
    }
}
