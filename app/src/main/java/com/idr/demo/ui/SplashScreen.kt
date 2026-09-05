package com.idr.demo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp

@Composable
fun SplashScreen() {
    Column(
        modifier = Modifier.fillMaxSize(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text("IDR", fontSize = 50.sp, fontWeight = FontWeight.Bold)
        Text("INTELLIGENT DEAD RECKONING", fontSize = 16.sp)
        Text("GNSS + AI + INS + MAP MATCHING", fontSize = 13.sp)
        Text("SIH 2026", fontSize = 12.sp)
    }
}
