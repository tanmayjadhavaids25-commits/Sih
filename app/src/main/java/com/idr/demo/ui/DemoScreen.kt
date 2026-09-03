package com.idr.demo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

@Composable
fun DemoScreen(vm: AppViewModel, onNavigate: () -> Unit) {
    val s = vm.navState
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Text("DEMO MODE", fontWeight = FontWeight.Bold)
        Text("TEST / DEMO DATA", fontWeight = FontWeight.SemiBold)
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
                Text("GNSS: ${if (s.gnssAvailable) "CONNECTED" else "LOST"}")
                Text("Mode: ${s.mode.label}")
                Text("AI Estimated Speed: ${"%.1f".format(s.aiVelocity)} km/h")
                Text("EKF Position: ${"%.6f".format(s.latitude)}, ${"%.6f".format(s.longitude)}")
                Text("Heading: ${"%.1f".format(s.heading)}°")
                Text("Map Confidence: ${"%.0f".format(s.mapConfidence)}%")
                Text("NHC: ${if (s.nhcActive) "ACTIVE" else "INACTIVE"}")
                Text("Source: ${s.sourceLabel}")
            }
        }

        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = vm::playDemo, modifier = Modifier.weight(1f)) { Text("PLAY") }
            Button(onClick = vm::pauseDemo, modifier = Modifier.weight(1f)) { Text("PAUSE") }
            Button(onClick = vm::resetDemo, modifier = Modifier.weight(1f)) { Text("RESET") }
        }

        Text("SPEED")
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            listOf(0.5, 1.0, 2.0, 4.0).forEach { speed ->
                Button(onClick = { vm.setDemoSpeed(speed) }, modifier = Modifier.weight(1f)) {
                    Text("${speed}x")
                }
            }
        }
        Text("Current: ${vm.playbackState} @ ${vm.playbackSpeed}x")

        Button(onClick = onNavigate, modifier = Modifier.fillMaxWidth()) {
            Text("OPEN NAVIGATION VIEW")
        }
    }
}
