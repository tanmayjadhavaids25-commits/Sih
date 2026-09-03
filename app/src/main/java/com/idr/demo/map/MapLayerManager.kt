package com.idr.demo.map

data class MapLayers(
    val osmRoads: Boolean = true,
    val groundTruth: Boolean = true,
    val insTrajectory: Boolean = true,
    val m7Trajectory: Boolean = true,
    val vehicle: Boolean = true,
    val gnssPoints: Boolean = true
)

class MapLayerManager {
    var layers: MapLayers = MapLayers()
        private set

    fun update(newLayers: MapLayers) {
        layers = newLayers
    }
}
