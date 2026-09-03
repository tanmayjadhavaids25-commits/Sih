package com.idr.demo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

@Composable
fun HomeScreen(
    vm: AppViewModel,
    onStartNavigation: () -> Unit,
    onDemoMode: () -> Unit,
    onSettings: () -> Unit
) {
    val cards = vm.systemStatusCards()
    LazyColumn(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        item {
            Text("IDR NAVIGATION", fontWeight = FontWeight.Bold)
            Text("SYSTEM READY")
            Text(vm.navState.sourceLabel)
        }
        items(cards) { (title, value) ->
            Card(modifier = Modifier.fillMaxWidth()) {
                Row(Modifier.padding(16.dp), horizontalArrangement = Arrangement.SpaceBetween) {
                    Text(title, modifier = Modifier.weight(1f))
                    Text(value, fontWeight = FontWeight.Bold)
                }
            }
        }
        item {
            Column(verticalArrangement = Arrangement.spacedBy(10.dp)) {
                Button(onClick = onStartNavigation, modifier = Modifier.fillMaxWidth()) {
                    Text("START NAVIGATION")
                }
                Button(onClick = onDemoMode, modifier = Modifier.fillMaxWidth()) {
                    Text("DEMO MODE")
                }
                Button(onClick = onSettings, modifier = Modifier.fillMaxWidth()) {
                    Text("SETTINGS / ABOUT")
                }
            }
        }
    }
}
