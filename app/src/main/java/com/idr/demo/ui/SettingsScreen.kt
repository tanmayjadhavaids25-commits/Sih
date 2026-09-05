package com.idr.demo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
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
fun AboutScreen(vm: AppViewModel, onBack: () -> Unit) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Text("IDR", fontWeight = FontWeight.Bold)
        Text("Intelligent Dead Reckoning")
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
                Text("Technology: AI/ML, IMU, GNSS, EKF, NHC, OSM, HMM, Map Matching")
                Text("Target: Continuous navigation during GNSS outage")
                Text("Dataset: IO-VNBD")
                Text("Platform: Android Smartphone")
                Text("Backend: ${if (vm.useDemoEngine) "DEMO ENGINE" else "REAL ENGINE"}")
            }
        }
        Button(onClick = { vm.setEngineMode(true) }, modifier = Modifier.fillMaxWidth()) { Text("USE DEMO ENGINE") }
        Button(onClick = { vm.setEngineMode(false) }, modifier = Modifier.fillMaxWidth()) { Text("USE REAL ENGINE (PLACEHOLDER)") }
        Button(onClick = onBack, modifier = Modifier.fillMaxWidth()) { Text("BACK") }
    }
}
