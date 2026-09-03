package com.idr.demo.navigation

import com.idr.demo.sensors.ImuData

interface NavigationEngine {
    fun processImu(imuData: ImuData)
    fun processGnss(latitude: Double, longitude: Double, speed: Double, heading: Double, accuracy: Double)
    fun getNavigationState(): NavigationState
    fun setMode(mode: NavigationMode)
    fun tick(deltaSeconds: Double)
}
