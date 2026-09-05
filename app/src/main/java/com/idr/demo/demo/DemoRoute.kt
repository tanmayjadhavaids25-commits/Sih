package com.idr.demo.demo

data class RoutePoint(
    val latitude: Double,
    val longitude: Double,
    val headingDeg: Double,
    val speedKmh: Double
)

object DemoRoute {
    val points = listOf(
        RoutePoint(18.5204, 73.8567, 80.0, 42.0),
        RoutePoint(18.5206, 73.8573, 84.0, 46.0),
        RoutePoint(18.5208, 73.8579, 87.0, 51.0),
        RoutePoint(18.5210, 73.8584, 92.0, 55.0),
        RoutePoint(18.5212, 73.8590, 95.0, 58.0),
        RoutePoint(18.5214, 73.8596, 98.0, 61.0),
        RoutePoint(18.5216, 73.8601, 101.0, 60.0),
        RoutePoint(18.5218, 73.8606, 104.0, 57.0),
        RoutePoint(18.5220, 73.8611, 108.0, 54.0),
        RoutePoint(18.5222, 73.8616, 112.0, 50.0),
        RoutePoint(18.5224, 73.8620, 118.0, 48.0),
        RoutePoint(18.5226, 73.8624, 124.0, 46.0)
    )
}
