package com.idr.demo.ui

import androidx.compose.animation.core.Spring
import androidx.compose.animation.core.animateFloatAsState
import androidx.compose.animation.core.spring
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.idr.demo.map.MapLayers

@Composable
fun NavigationScreen(vm: AppViewModel, onTechnicalView: () -> Unit, onStop: () -> Unit) {
    val state = vm.navState
    val layers = vm.mapLayers
    val targetX = (((state.longitude - vm.routePoints.first().longitude) / 0.0065) * 800.0 + 80.0).toFloat()
    val targetY = (((vm.routePoints.last().latitude - state.latitude) / 0.0045) * 450.0 + 60.0).toFloat()
    val markerX = animateFloatAsState(targetValue = targetX, animationSpec = spring(stiffness = Spring.StiffnessVeryLow), label = "x")
    val markerY = animateFloatAsState(targetValue = targetY, animationSpec = spring(stiffness = Spring.StiffnessVeryLow), label = "y")

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(12.dp),
        verticalArrangement = Arrangement.spacedBy(10.dp)
    ) {
        Text("IDR NAVIGATION", fontWeight = FontWeight.Bold)
        Card(modifier = Modifier.fillMaxWidth()) {
            Text(
                "SYSTEM MODE: ${state.mode.label}",
                modifier = Modifier.padding(10.dp),
                fontWeight = FontWeight.Bold,
                color = when (state.mode) {
                    com.idr.demo.navigation.NavigationMode.GNSS_INS -> Color(0xFF44E17A)
                    com.idr.demo.navigation.NavigationMode.AI_DEAD_RECKONING -> Color(0xFFFFB74D)
                    com.idr.demo.navigation.NavigationMode.MAP_MATCHING -> Color(0xFF7C4DFF)
                    com.idr.demo.navigation.NavigationMode.GNSS_RESTORING -> Color(0xFF4FC3F7)
                    com.idr.demo.navigation.NavigationMode.ERROR -> Color(0xFFEF5350)
                }
            )
        }

        vm.bannerMessage?.let {
            Card(modifier = Modifier.fillMaxWidth()) {
                Text(it, modifier = Modifier.padding(10.dp), color = Color(0xFFFFD54F), fontWeight = FontWeight.Bold)
            }
        }

        Card(modifier = Modifier.fillMaxWidth().weight(1f)) {
            Canvas(
                modifier = Modifier
                    .fillMaxSize()
                    .background(Color(0xFF071018), RoundedCornerShape(8.dp))
            ) {
                drawMapLayers(layers, vm)
                if (layers.vehicle) {
                    drawCircle(Color(0xFF00E5FF), radius = 14f, center = Offset(markerX.value, markerY.value))
                }
            }
        }

        LayerToggles(vm)

        Card(modifier = Modifier.fillMaxWidth()) {
            Column(Modifier.padding(12.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
                Text("Speed: ${"%.1f".format(state.speed)} km/h")
                Text("Heading: ${"%.1f".format(state.heading)}°")
                Text("Accuracy: ${"%.1f".format(state.accuracy)} m")
                Text("Mode: ${state.mode.label}")
                Text("GNSS: ${if (state.gnssAvailable) "CONNECTED" else "LOST"}")
                Text("Map Confidence: ${"%.0f".format(state.mapConfidence)}%")
                Text("Source: ${state.sourceLabel}", color = Color(0xFF8FE8FF))
            }
        }

        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = vm::simulateGnssLoss, modifier = Modifier.weight(1f)) { Text("SIMULATE GNSS LOSS") }
            Button(onClick = vm::restoreGnss, modifier = Modifier.weight(1f)) { Text("RESTORE GNSS") }
        }
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = vm::centerVehicle, modifier = Modifier.weight(1f)) { Text("CENTER VEHICLE") }
            Button(onClick = onTechnicalView, modifier = Modifier.weight(1f)) { Text("TECHNICAL VIEW") }
        }
        Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            Button(onClick = onStop, modifier = Modifier.weight(1f)) { Text("STOP") }
        }
    }
}

@Composable
private fun LayerToggles(vm: AppViewModel) {
    val layers = vm.mapLayers
    Column(modifier = Modifier.fillMaxWidth(), verticalArrangement = Arrangement.spacedBy(4.dp)) {
        ToggleButtonRow("OSM Roads", layers.osmRoads) { vm.setLayerToggle { it.copy(osmRoads = !it.osmRoads) } }
        ToggleButtonRow("Ground Truth", layers.groundTruth) { vm.setLayerToggle { it.copy(groundTruth = !it.groundTruth) } }
        ToggleButtonRow("INS Trajectory", layers.insTrajectory) { vm.setLayerToggle { it.copy(insTrajectory = !it.insTrajectory) } }
        ToggleButtonRow("M7 Trajectory", layers.m7Trajectory) { vm.setLayerToggle { it.copy(m7Trajectory = !it.m7Trajectory) } }
        ToggleButtonRow("Vehicle", layers.vehicle) { vm.setLayerToggle { it.copy(vehicle = !it.vehicle) } }
        ToggleButtonRow("GNSS Points", layers.gnssPoints) { vm.setLayerToggle { it.copy(gnssPoints = !it.gnssPoints) } }
    }
}

@Composable
private fun ToggleButtonRow(label: String, enabled: Boolean, onToggle: () -> Unit) {
    Button(onClick = onToggle, modifier = Modifier.fillMaxWidth().height(36.dp)) {
        Text((if (enabled) "☑ " else "☐ ") + label)
    }
}

private fun androidx.compose.ui.graphics.drawscope.DrawScope.drawMapLayers(layers: MapLayers, vm: AppViewModel) {
    fun pt(lat: Double, lon: Double): Offset {
        val x = (((lon - vm.routePoints.first().longitude) / 0.0065) * size.width * 0.85f + size.width * 0.08f).toFloat()
        val y = (((vm.routePoints.last().latitude - lat) / 0.0045) * size.height * 0.7f + size.height * 0.15f).toFloat()
        return Offset(x, y)
    }

    if (layers.osmRoads) {
        for (i in 0..9) {
            val y = size.height * (0.15f + i * 0.07f)
            drawLine(Color(0xFF203040), Offset(size.width * 0.05f, y), Offset(size.width * 0.95f, y), strokeWidth = 2f)
        }
    }

    val path = Path()
    vm.routePoints.forEachIndexed { index, p ->
        val o = pt(p.latitude, p.longitude)
        if (index == 0) path.moveTo(o.x, o.y) else path.lineTo(o.x, o.y)
        if (layers.gnssPoints) drawCircle(Color(0xFF4FC3F7), radius = 3.5f, center = o)
    }

    if (layers.groundTruth) drawPath(path, Color(0xFF7C4DFF), style = Stroke(width = 7f))
    if (layers.insTrajectory) drawPath(path, Color(0xFF00E676), style = Stroke(width = 4f))
    if (layers.m7Trajectory) drawPath(path, Color(0xFFFFAB40), style = Stroke(width = 2f))
}
