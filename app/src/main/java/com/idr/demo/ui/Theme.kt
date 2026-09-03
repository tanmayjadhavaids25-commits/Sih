package com.idr.demo.ui

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val IdrColors = darkColorScheme(
    primary = Color(0xFF00D5FF),
    secondary = Color(0xFF44E17A),
    background = Color(0xFF0B0F14),
    surface = Color(0xFF111A24),
    onPrimary = Color.Black,
    onBackground = Color(0xFFE5EEF9),
    onSurface = Color(0xFFE5EEF9)
)

@Composable
fun IdrTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = IdrColors,
        content = content
    )
}
