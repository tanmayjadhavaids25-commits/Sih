package com.idr.demo.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
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
import kotlin.math.pow

@Composable
fun SensorMonitor(vm: AppViewModel, onBack: () -> Unit) {
    val history = vm.sensorHistory
    val ax = history.map { it.ax }
    val ay = history.map { it.ay }
    val az = history.map { it.az }
    val gx = history.map { it.gx }
    val gy = history.map { it.gy }
    val gz = history.map { it.gz }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(12.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp)
    ) {
        Text("SENSOR MONITOR", fontWeight = FontWeight.Bold)
        Text("DEMO DATA (SIMULATED)", color = Color(0xFFFFB74D))

        SensorGraphCard("Accelerometer X", ax, Color(0xFF4FC3F7))
        SensorGraphCard("Accelerometer Y", ay, Color(0xFF81C784))
        SensorGraphCard("Accelerometer Z", az, Color(0xFFFF8A65))
        SensorGraphCard("Gyroscope X", gx, Color(0xFFB39DDB))
        SensorGraphCard("Gyroscope Y", gy, Color(0xFFFFD54F))
        SensorGraphCard("Gyroscope Z", gz, Color(0xFF90CAF9))

        Button(onClick = onBack, modifier = Modifier.fillMaxWidth()) { Text("BACK") }
    }
}

@Composable
private fun SensorGraphCard(title: String, values: List<Float>, color: Color) {
    val mean = if (values.isEmpty()) 0.0 else values.map { it.toDouble() }.average()
    val variance = if (values.isEmpty()) 0.0 else values.map { (it - mean).pow(2) }.average()

    Card(modifier = Modifier.fillMaxWidth()) {
        Column(Modifier.padding(8.dp), verticalArrangement = Arrangement.spacedBy(4.dp)) {
            Text(title, fontWeight = FontWeight.SemiBold)
            Canvas(modifier = Modifier.fillMaxWidth().height(72.dp)) {
                if (values.isEmpty()) return@Canvas
                val maxAbs = (values.maxOf { kotlin.math.abs(it) }.coerceAtLeast(0.01f))
                val path = Path()
                values.forEachIndexed { i, v ->
                    val x = i.toFloat() / (values.size - 1).coerceAtLeast(1) * size.width
                    val y = size.height / 2 - (v / maxAbs) * (size.height * 0.42f)
                    if (i == 0) path.moveTo(x, y) else path.lineTo(x, y)
                }
                drawPath(path, color = color, style = Stroke(2.8f))
                drawLine(Color.DarkGray, Offset(0f, size.height / 2), Offset(size.width, size.height / 2), 1f)
            }
            Row(modifier = Modifier.fillMaxWidth()) {
                Text("Rate: 100 Hz", modifier = Modifier.weight(1f))
                Text("Current: ${"%.3f".format(values.lastOrNull() ?: 0f)}")
            }
            Row(modifier = Modifier.fillMaxWidth()) {
                Text("Mean: ${"%.3f".format(mean)}", modifier = Modifier.weight(1f))
                Text("Var: ${"%.3f".format(variance)}")
            }
        }
    }
}
