package com.idr.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Surface
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.idr.demo.ui.AboutScreen
import com.idr.demo.ui.AppViewModel
import com.idr.demo.ui.DemoScreen
import com.idr.demo.ui.HomeScreen
import com.idr.demo.ui.IdrTheme
import com.idr.demo.ui.NavigationScreen
import com.idr.demo.ui.SensorMonitor
import com.idr.demo.ui.SplashScreen
import com.idr.demo.ui.TechnicalDashboard

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            IdrTheme {
                Surface(modifier = Modifier.fillMaxSize()) {
                    val navController = rememberNavController()
                    val vm: AppViewModel = viewModel()
                    val start = remember { "splash" }

                    NavHost(navController = navController, startDestination = start) {
                        composable("splash") {
                            SplashScreen()
                            LaunchedEffect(Unit) {
                                kotlinx.coroutines.delay(1700)
                                navController.navigate("home") {
                                    popUpTo("splash") { inclusive = true }
                                }
                            }
                        }
                        composable("home") {
                            HomeScreen(
                                vm = vm,
                                onStartNavigation = { navController.navigate("navigation") },
                                onDemoMode = { navController.navigate("demo") },
                                onSettings = { navController.navigate("settings") }
                            )
                        }
                        composable("navigation") {
                            NavigationScreen(
                                vm = vm,
                                onTechnicalView = { navController.navigate("technical") },
                                onStop = { navController.popBackStack("home", false) }
                            )
                        }
                        composable("technical") {
                            TechnicalDashboard(vm = vm, onSensors = { navController.navigate("sensors") })
                        }
                        composable("sensors") {
                            SensorMonitor(vm = vm, onBack = { navController.popBackStack() })
                        }
                        composable("demo") {
                            DemoScreen(vm = vm, onNavigate = { navController.navigate("navigation") })
                        }
                        composable("settings") {
                            AboutScreen(vm = vm, onBack = { navController.popBackStack() })
                        }
                    }
                }
            }
        }
    }
}
